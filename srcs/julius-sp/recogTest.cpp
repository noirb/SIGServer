/////////////////////////////////////////////////////////////
//
//	julius module mode test program
//
//	this program does following:
//
//	(1) connect to julius running in module mode.
//	(2) send .wav data to julius via adinnet.
//	(3) get XML-based recognition result from julius via adinnet.
//	(4) extract plain text from XML.
//
//	by sekikawa
//	2009/03/24

#include "JspRecog.h"
#include "JspSocket.h"
#include "JspWave.h"
#include "JspXMLParser.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>	// for usleep()

#define DEFAULT_MODULE_PORT		10500
#define DEFAULT_ADINNET_PORT	5530

int main(int argc, char *argv[]) {
	Jsp::Socket sockAdinnet;
	Jsp::Socket sockModule;

	// ----------------------------------------
	//	default port no
	// ----------------------------------------
	int module_portno = DEFAULT_MODULE_PORT;
	int adinnet_portno = DEFAULT_ADINNET_PORT;

	// ----------------------------------------
	//	help
	// ----------------------------------------
	if ((argc == 2) && (strcmp(argv[1], "-h") == 0)) {
		fprintf(stderr, "usage: %s [-mp <portno>] [-ap <portno>]\n", argv[0]);
		return 0;
	}

	// ----------------------------------------
	//	check for port specification
	// ----------------------------------------
	bool bModulePortSpecified = false;
	bool bAdinnetPortSpecified = false;
	for (int i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-mp") == 0)
		{
			bModulePortSpecified = true;
		}
		else if (strcmp(argv[i], "-ap") == 0)
		{
			bAdinnetPortSpecified = true;
		}
		else if (bModulePortSpecified)
		{
			module_portno = atoi(argv[i]);
			bModulePortSpecified = false;
		}
		else if (bAdinnetPortSpecified)
		{
			adinnet_portno = atoi(argv[i]);
			bAdinnetPortSpecified = false;
		}
	}

	int status;

	// ----------------------------------------
	//	connect to julius in module mode
	// ----------------------------------------
	status = sockModule.connect("localhost", module_portno);

	if (status < 0) {
		printf("[ERROR] module port (%d) connect failed.\n", module_portno);
		return 1;
	} else {
		printf("module port (%d) connection established.\n", module_portno);
	}

	// ----------------------------------------
	//	wait some time
	// ----------------------------------------
	usleep(500000);	// microsecond

	// ----------------------------------------
	//	connect to julius adinnet port
	// ----------------------------------------
	status	= sockAdinnet.connect("localhost", adinnet_portno);

	if (status < 0) {
		printf("[ERROR] adinnet port (%d) connect failed.\n", adinnet_portno);
		return 1;
	} else {
		printf("adinnet port (%d) connection established.\n", adinnet_portno);
	}

	// ----------------------------------------
	//	setup recog object
	// ----------------------------------------
	Jsp::Recog recog(&sockModule, &sockAdinnet);

	for (;;)
	{
		char waveFile[256];

		printf("*** enter wav file name ***\n");
		printf("> ");
		fgets(waveFile, sizeof(waveFile), stdin);

		if (strcmp(waveFile, "quit")==0) break;

		// strip off NL at the end of line
		waveFile[strlen(waveFile)-1]='\0';

		// ----------------------------------------
		//	load wave file
		// ----------------------------------------
		Jsp::Wave wave;
		if (!wave.loadFromFile(waveFile)) break;

		char *p = wave.getWaveDataPtr();
		int len = wave.getWaveDataLen();

		// ----------------------------------------
		//	recognize
		// ----------------------------------------
		std::vector<std::string> outTexts;
		if (recog.recognize(p, len, outTexts)) {

			// ----------------------------------------
			//	output result
			// ----------------------------------------
			printf("******** RECOGNIZE RESULT *********\n");

			int n = outTexts.size();
			for (int i=0; i<n; i++) {
				std::string outText = outTexts[i];

				printf("[%d] (%s)\n", i, outText.c_str());
			}
		}
	}

	printf("end\n");

	// ----------------------------------------
	//	close connection to julius
	// ----------------------------------------
	sockAdinnet.close();
	sockModule.close();

	return 0;
}
