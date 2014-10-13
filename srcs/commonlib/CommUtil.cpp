/*
 * Written by okamoto on 2011-03-31
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-17
 *    Modify indent
 */

#include "CommUtil.h"
#include "Logger.h"

#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>


// sekikawa(FIX20100826)
//SOCKET CommUtil::connectServer(const char *hostname, int port)
SOCKET CommUtil::connectServer(const char *hostname, int port, int retry)
{

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0) {
		perror("cannot open socket");
		return -1;
	}

	struct hostent *hp = gethostbyname(hostname);
	if (!hp) {
		fprintf(stderr, "unknown host : %s\n", hostname);
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;

	// **** CAUTION ********************************************
	// argument's order of bcopy and memcpy is different
	//
	// memcpy(dest, src, nbytes)
	// bcopy(src, dest, nbytes)
	// *********************************************************
#ifndef WIN32
	bcopy((char *)hp->h_addr, (char*)&addr.sin_addr, hp->h_length);
#else
	memcpy((char*)&addr.sin_addr, (char *)hp->h_addr,  hp->h_length);
#endif
	addr.sin_port = htons(port);

	// begin(sekikawa)(FIX20100826)
	//fprintf(stderr, "trying to connect to [%s:%d]\n", hostname, port);
	for(;;)
	{
		if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			retry--;
			if (retry >= 0)
			{
				fprintf(stderr, "connect failed. retrying .. (%d) [%s:%d]\n", retry, __FILE__, __LINE__);
#ifndef WIN32
				usleep(1000000);	// microsec
#else
				Sleep(1000);
#endif
				continue;
			}
			else
			{
				// retry failed
				perror("cannot connect server");
				return -1;

			}
		}
		else
		{
			// connect ok
			//fprintf(stderr, "connected.\n");
			break;
		}
	}
// end(sekikawa)(FIX20100826)

	return sock;
}

void CommUtil::disconnectServer(SOCKET sock)
{
#ifndef WIN32
		close(sock);
#else
		closesocket(sock);
#endif
}

int CommUtil::sendData(SOCKET sock, const char *data, int bytes)
{
	int sent = 0;
	while (sent < bytes) {
		const char *h = data + sent;
		// linux socket has easy non-blocking mode flag (MSG_DONTWAIT)
#ifndef WIN32
		int r = send(sock, h, bytes - sent, MSG_DONTWAIT);
#else
		int r = send(sock, h, bytes - sent, 0);
#endif
		if (r < 0) {
			if (errno == EINTR ||
				errno == EAGAIN ||
				errno == EWOULDBLOCK) {
				continue;
			}
		}

		if (r <= 0) {
			return r;
		}

		//IrcApp *app = getApp();
		//if (app) app->printLog("%d bytes sent \n", r);

		sent += r;
	}
	return sent == bytes? sent: -1;
}


void CommUtil::openTimeDebugLogFp(const char *fname)
{
	tdebug = fopen(fname, "w");
}

FILE *CommUtil::getTimeDebugLogFp() { return tdebug; }

void CommUtil::closeTimeDebugLogFp()
{
	if (tdebug) fclose(tdebug);
	tdebug = NULL;
}


