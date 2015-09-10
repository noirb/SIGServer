/*
 * Modified by okamoto on 2011-03-25
 */

#include <sigverse/commonlib/MessageSender.h>

#ifdef WIN32
#include <windows.h>	// for winsock
#endif

// ./sendmsg -h <server> -p <port> -n <agent> msg1 msg2 msg3 ...
static void printUsage(char *progname)
{
	fprintf(stderr, "Usage :\n");
	fprintf(stderr, "\t%s -s <server> -p <port> -n <agent> msg1 msg2 msg3 ...\n", progname);
	fprintf(stderr, "\t%s -h\n", progname);
}

int main(int argc, char **argv)
{
	char *server = "localhost";
	bool bcast = false;
	char *target = NULL;
	int  port = 5678;  //TODO: Magic number

#ifdef WIN32
	// initialize WinSock
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		fprintf(stderr, "winsock init error\n");
		return 1;
	}
#endif

	int i=1;
	while (i < argc) {
		char *opt = argv[i];
		if (*opt != '-') { break; }
		switch(opt[1]) {
		case 's':
			i++;
			server = argv[i]; i++;
			break;
		case 'p':
			i++;
			port = atoi(argv[i]); i++;
			break;
		case 'n':
			i++;
			target = argv[i]; i++;
			break;
		case 'b':
			bcast = true;
			i++;
			break;
		case 'h':
			printUsage(argv[0]);
			return 0;
		default:
			fprintf(stderr, "bad option : %s\n", opt);
			printUsage(argv[0]);
			return 1;
		}
	}
	if (i >= argc) {
		fprintf(stderr, "too few arguments\n");
		return 1;
	}
	if (bcast && target) {
		fprintf(stderr, "send to target or boardcast ?\n");
		return 1;
	}

	int n = argc - i;
	char **pargv = &argv[i];

	MessageSender sender(argv[0]);
	bool b = sender.connect(server, port);
	if (!b) {
		fprintf(stderr, "cannot connect to server : %s:%d\n", server, port);
		return 1;
	}
	b = sender.sendto(target, n, pargv);
	if (!b) {
		fprintf(stderr, "failed\n");
		return 1;
	}

#ifdef WIN32
	// clean up WinSock
	WSACleanup();
#endif

	return 0;
}
