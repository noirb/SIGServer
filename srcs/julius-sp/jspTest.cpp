/////////////////////////////////////////////////////////////
//
//	sigjsp test program
//
//	this program does following:
//
//	(1) connect to sigjsp
//	(2) send COMM_NS_PINGER_REQUEST or REQUEST_SOUND_RECOG to sigjsp
//
//	by sekikawa
//	2009/04/07

#include "JspTestClient.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>	// for usleep()

#define JULIUS_SP_PORT		7500

int main(int argc, char *argv[])
{
	// ----------------------------------------
	//	default port no
	// ----------------------------------------
	std::string julius_sp_hostname = "localhost";
	int julius_sp_portno = JULIUS_SP_PORT;

	// ----------------------------------------
	//	help
	// ----------------------------------------
	if ((argc == 2) && (strcmp(argv[1], "-h") == 0)) {
		fprintf(stderr, "usage: %s [-sh <hostname>] [-sp <portno>]\n", argv[0]);
		return 0;
	}

	// ----------------------------------------
	//	check for port specification
	// ----------------------------------------
	bool bJuliusSpHostSpecified = false;
	bool bJuliusSpPortSpecified = false;
	for (int i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-sh") == 0)
		{
			bJuliusSpHostSpecified = true;
		}
		else if (strcmp(argv[i], "-sp") == 0)
		{
			bJuliusSpPortSpecified = true;
		}
		else if (bJuliusSpHostSpecified)
		{
			julius_sp_hostname = argv[i];
			bJuliusSpHostSpecified = false;
		}
		else if (bJuliusSpPortSpecified)
		{
			julius_sp_portno = atoi(argv[i]);
			bJuliusSpPortSpecified = false;
		}
	}

	// ----------------------------------------
	//	setup for connection to sigjsp
	// ----------------------------------------
	Jsp::TestClient jspClient;

	jspClient.setJuliusSpHostName(julius_sp_hostname.c_str());
	jspClient.setJuliusSpPort(julius_sp_portno);

	// ----------------------------------------
	//	do command loop
	// ----------------------------------------
	jspClient.commandLoop();

	return 0;
}
