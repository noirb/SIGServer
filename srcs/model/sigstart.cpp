/*
 * Added TODO comments by Tetsunari Inamura on 2014-03-02
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define SOCKET int
#endif
#include "binary.h"

int main(int argc, char **argv)
{
	char *servername = "localhost";
	unsigned short port = 9000;

	int i=1;
	while (i < argc) {
		char *opt = argv[i];
		if (*opt != '-') { break; }
		switch(opt[1]) {
		case 's':
			i++;
			servername = argv[i]; i++;
			break;
		case 'p':
			i++;
			port = atoi(argv[i]); i++;
			break;
		default:
			fprintf(stderr, "bad option : %s\n", opt);
			return 1;
		}
	}

	struct sockaddr_in server;
	SOCKET sock;
	int n;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	// Preparation of structure for the socket connection
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(servername);
	if (server.sin_addr.s_addr == 0xffffffff) {
		struct hostent *host;

		host = gethostbyname(servername);
		if (host == NULL) {
			return 1;
		}
		server.sin_addr.s_addr = *(unsigned int *)host->h_addr_list[0];
	}
  
	// Connecting to the server
	int ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if (ret < 0) {
		fprintf(stderr, "failed to quit\n");
		return 0;
	}
	send(sock, "SIGViewer,SIGSTART,", 18, 0); //TODO: Use exclusive library should be used
	//recv(sock, buf, sizeof(buf), 0);

	char sendBuf[4];
	char *p = sendBuf;
#ifndef WIN32
	sleep(1);
#else
	Sleep(1000);
#endif

	//TODO: Magic numbers around the following lines should be removed
	BINARY_SET_DATA_S_INCR(p, unsigned short, 2);
	BINARY_SET_DATA_S_INCR(p, unsigned short, 4);
	send(sock, sendBuf, sizeof(sendBuf), 0);

	p = sendBuf;
	BINARY_SET_DATA_S_INCR(p, unsigned short, 6);
	BINARY_SET_DATA_S_INCR(p, unsigned short, 4);
	send(sock, sendBuf, sizeof(sendBuf), 0);
	fprintf(stderr, "SIGVerse simulation (port[%d]) started\n", port);  

	return 0;
}
