/* $Id: runmain.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <getopt.h>

#include "Controller.h"
#include "ControllerLib.h"
#include "modelerr.h"
#include "Logger.h"

static void printUsage(const char *prog)
{
	printf("Usage : %s\n", prog);
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
	c.attach(server, port, name);

	c.loopMain();

	delete lib;
	return 0;
}	

