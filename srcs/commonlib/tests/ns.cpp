#include <getopt.h>

#include "CommUtil.h"
#include "ct/NSLookup.h"
#include "CommDataEncoder.h"

int main(int argc, char **argv)
{

	char hostname[128] = "localhost";
	int port = 5678;
	
	extern char *optarg;
	int ch;
	while ((ch = getopt(argc, argv, "h:p:")) != -1) {
		switch(ch) {
		case 'h':
			strcpy(hostname, optarg);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		default:
			fprintf(stderr, "bad option : -%c\n", ch);
			return 1;
		}
	}

	SOCKET sock = CommUtil::connectServer(hostname, port);
	if (sock < 0) {
		fprintf(stderr, "cannot connect server %s:%d\n", hostname, port);
		return 1;

	}
	CommData::ConnectDataPortRequest enc("lookup");
	enc.send(sock);

	NSLookup ns(sock);

	printf("Looking up capture view service provider...\n");
	NSLookup::Provider *r = ns.lookup(Service::CAPTURE_VIEW);
	if (r) {
		printf("\tprovider -> %s:%d\n",
		       r->service().hostname(),
		       r->service().port());
	} else {
		printf("cannot find provider\n");
	}
	return 0;
}
