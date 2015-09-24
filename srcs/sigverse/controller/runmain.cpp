/*
 * Modified by okamoto on 2011-03-25
 */

#ifdef WIN32
#include "wingetopt.h"
#else
#include <getopt.h>
#endif

#include <sigverse/commonlib/Controller.h>
#include <sigverse/controller/ControllerLib.h>
#include <sigverse/commonlib/modelerr.h>
#include <sigverse/commonlib/Logger.h>

static void printUsage(const char *prog)
{
	fprintf(stderr, "Usage : %s\n", prog);
}

int main(int argc, char **argv)
{
	char *libname = 0;
	char *server  = "localhost";
	char *name    = 0;
	int  port     = 5678; // TODO: Magic number should be removed
	bool silent   = false;

	int opt;
	while ((opt = getopt(argc, argv, "l:h:p:n:s")) != -1) {

		switch(opt) {
			case 'l':
				libname = optarg; break;
			case 'h':
				server = optarg; break;
			case 'p':
				port = atoi(optarg); break;
			case 'n':
				name = optarg; break;
			case 's':
				silent = true; break;
			default:
				printUsage(argv[0]);
				return 1;
		}
	}
	if (!name) {
		err(("no name\n"));
		return 1;
	}

	if (!libname) {
		err(("no library\n"));
		return 1;
	}

	if (port < 0) {
		err(("no port"));
		return 1;
	}

	LOG_OUTPUT_LEVEL(LOG_MSG);
	if (!silent) {
		LOG_STDOUT();
	}

	ControllerLib *lib = new ControllerLib();
	if (!lib->load(libname)) { return 1; }

	Controller::init();
	Controller &c = lib->getController();
	printf("ATTACH %s %d %s\n", server, port,name);
	bool ret = c.attach(server, port, name);

	if (ret){
		c.loopMain();
	}else{
		printf("Fail to run ... \n");
	}
	delete lib;
#ifdef WIN32
	// clean up WinSock
	WSACleanup();
#endif
	return 0;
}

