#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>

int main(int argc, char **argv)
{
	char *hostname = "localhost";
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("cannot open socket");
		return 1;
	}

	struct hostent * hp;
	hp = gethostbyname(hostname);
	if (hp == 0) {
		fprintf(stderr, "unknown host : %s\n", hostname);
		return 1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	bcopy((char*)hp->h_addr, (char *)&addr.sin_addr, hp->h_length);
	addr.sin_port = htons(5678);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("cannot connect to server");
		return 1;
	}

	char buf[1024];
	int r;
	for (;;) {
		int r = read(sock, buf, sizeof(buf));
		if (r < 0) { break; }
		if (r > 0) {
			buf[r] = 0;
			printf("%s\n", buf);
		}
	}

	close(sock);
	return 0;
}
