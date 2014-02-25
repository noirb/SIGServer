#include "julius_sp.h"
#include "Jsp.h"
#include "JspStringUtil.h"
#include "JspConfigFile.h"
#include "JspLog.h"
#include <stdio.h>
#include <signal.h>
#include <string>

Jsp::JuliusServiceProvider jsp;

int main(int argc, char *argv[])
{
	// ------------------------------------
	//	parameter default values
	// ------------------------------------
	std::string simServerHostName("localhost");
	int simServerPort = 7000;
	std::string jspServiceProviderName("");
	int jspServiceAcceptPort = 7500;
	int juliusModulePort = 10500;
	int juliusAdinnetPort = 5530;
	std::string juliusPath = "/usr/local/bin/julius";
	std::string juliusConfFile = "test.jconf";
	int delayTimeForStartJulius = 3000;		// msec
	int logLevel = 1;
	Jsp::ConfigFile conf;
	int bConfigFileLoaded = 0;	// false

	// ------------------------------------
	//	get config file path
	// ------------------------------------
	char *jspConfigPath = getJspConfigPath();

	// ------------------------------------
	//	read config file
	// ------------------------------------
	if (conf.load(jspConfigPath))
	{
		bConfigFileLoaded = 1;

		const char *server = conf.getStringValue("SERVER");
		if (server)
		{
			simServerHostName = server;
		}

		const char *port = conf.getStringValue("PORT");
		if (port)
		{
			simServerPort = atoi(port);
		}

		const char *serviceProviderName = conf.getStringValue("SERVICE_PROVIDER_NAME");
		if (serviceProviderName)
		{
			jspServiceProviderName = serviceProviderName;
		}

		const char *servicePort = conf.getStringValue("SERVICE_PORT");
		if (servicePort)
		{
			jspServiceAcceptPort = atoi(servicePort);
		}

		const char *modulePort = conf.getStringValue("JULIUS_MODULE_PORT");
		if (modulePort)
		{
			juliusModulePort = atoi(modulePort);
		}

		const char *adinnetPort = conf.getStringValue("JULIUS_ADINNET_PORT");
		if (adinnetPort)
		{
			juliusAdinnetPort = atoi(adinnetPort);
		}

		const char *path = conf.getStringValue("JULIUS_PATH");
		if (path)
		{
			juliusPath = path;
		}

		const char *confFile = conf.getStringValue("JULIUS_CONFIG_FILE");
		if (confFile)
		{
			juliusConfFile = confFile;
		}

		const char *delay = conf.getStringValue("DELAY_TIME_FOR_START_JULIUS");
		if (delay)
		{
			delayTimeForStartJulius = atoi(delay);
		}

		const char *pLogLevel = conf.getStringValue("LOG_LEVEL");
		if (pLogLevel)
		{
			logLevel = atoi(pLogLevel);
		}
	}

	// ------------------------------------
	//	analyze command line options
	// ------------------------------------
	if ((argc == 2) && (strcmp(argv[1], "-h") == 0))
	{
		fprintf(stderr, "usage: %s [-s serverName] [-p port] [-sn serviceProviderName] [-sp servicePort] [-mp juliusModulePort] [-ap julisAdinnetPort] [-e julius path] [-C julius config file] [-log logLevel(1-3)]\n", argv[0]);
		return 0;
	}
	else
	{
		bool bServerName = false;
		bool bPort = false;
		bool bServiceProviderName = false;
		bool bServicePort = false;
		bool bModulePort = false;
		bool bAdinnetPort = false;
		bool bJuliusPath = false;
		bool bJuliusConfFile = false;
		bool bLogLevel = false;

		for (int i=1; i<argc; i++)
		{
			if (strcmp(argv[i], "-s") == 0)
			{
				bServerName = true;
			}
			else if (strcmp(argv[i], "-p") == 0)
			{
				bPort = true;
			}
			else if (strcmp(argv[i], "-sn") == 0)
			{
				bServiceProviderName = true;
			}
			else if (strcmp(argv[i], "-sp") == 0)
			{
				bServicePort = true;
			}
			else if (strcmp(argv[i], "-mp") == 0)
			{
				bModulePort = true;
			}
			else if (strcmp(argv[i], "-ap") == 0)
			{
				bAdinnetPort = true;
			}
			else if (strcmp(argv[i], "-e") == 0)
			{
				bJuliusPath = true;
			}
			else if (strcmp(argv[i], "-C") == 0)
			{
				bJuliusConfFile = true;
			}
			else if (strcmp(argv[i], "-log") == 0)
			{
				bLogLevel = true;
			}
			else
			{
				if (bServerName)
				{
					simServerHostName = argv[i];
					bServerName = false;
				}
				else if (bPort)
				{
					simServerPort = atoi(argv[i]);
					bPort = false;
				}
				else if (bServiceProviderName)
				{
					jspServiceProviderName = argv[i];
					bServiceProviderName = false;
				}
				else if (bServicePort)
				{
					jspServiceAcceptPort = atoi(argv[i]);
					bServicePort = false;
				}
				else if (bModulePort)
				{
					juliusModulePort = atoi(argv[i]);
					bModulePort = false;
				}
				else if (bAdinnetPort)
				{
					juliusAdinnetPort = atoi(argv[i]);
					bAdinnetPort = false;
				}
				else if (bJuliusPath)
				{
					juliusPath = argv[i];
					bJuliusPath = false;
				}
				else if (bJuliusConfFile)
				{
					juliusConfFile = argv[i];
					bJuliusConfFile = false;
				}
				else if (bLogLevel)
				{
					logLevel = atoi(argv[i]);
					bLogLevel = false;
				}
			}
		}
	}

	Jsp::setLogLevel(logLevel);

	// ------------------------------------
	//	set parameter to jsp
	// ------------------------------------
	jsp.setLoadedSigJspConfFile(bConfigFileLoaded ? jspConfigPath : NULL);
	jsp.setSimServerHostName(simServerHostName);
	jsp.setSimServerPort(simServerPort);

	if (jspServiceProviderName.size() == 0)
	{
		jspServiceProviderName = jsp.getHostName();
	}
	jsp.setJspServiceProviderName(jspServiceProviderName);

	jsp.setJspServiceAcceptPort(jspServiceAcceptPort);
	jsp.setJuliusModulePort(juliusModulePort);
	jsp.setJuliusAdinnetPort(juliusAdinnetPort);
	jsp.setJuliusPath(juliusPath);
	jsp.setJuliusConfFile(juliusConfFile);
	jsp.setDelayTimeForStartJulius(delayTimeForStartJulius);

	jsp.printSettings();

	// ------------------------------------
	//	start julius
	// ------------------------------------
	jsp.invokeJulius();

	if (!jsp.isJuliusAvairable())
	{
		Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++++++\n");
		Jsp::printLog(LOG_FATAL, "+++ sorry, failed to start julius. +++\n");
		Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++++++\n");

		return 1;
	}

	Jsp::printLog(LOG_ALWAYS, "***************************************\n");
	Jsp::printLog(LOG_ALWAYS, "***   julius successfully started   ***\n");
	Jsp::printLog(LOG_ALWAYS, "***************************************\n");

	// ------------------------------------
	//	setup signal handler
	// ------------------------------------
	signal(SIGKILL, quitJuliusSp);
	signal(SIGINT, quitJuliusSp);

	// ------------------------------------------------
	//	connect to SimServer
	// ------------------------------------------------
	if (!jsp.connectToSimServer())
	{
		Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++\n");
		Jsp::printLog(LOG_FATAL, "  failed to connect to SimServer\n");
		Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++\n");

		return 1;
	}

	Jsp::printLog(LOG_ALWAYS, "*******************************************************\n");
	Jsp::printLog(LOG_ALWAYS, "  connect to SimServer (%s:%d) ok\n", simServerHostName.c_str(), simServerPort);
	Jsp::printLog(LOG_ALWAYS, "*******************************************************\n");

	// ------------------------------------------------
	//	send COMM_REQUEST_PROVIDE_SERVICE to SimServer
	// ------------------------------------------------
	if (!jsp.sendProvideServiceRequest())
	{
		Jsp::printLog(LOG_FATAL, "failed to send COMM_REQUEST_PROVIDE_SERVICE\n");
		return 1;
	}

	Jsp::printLog(LOG_ALWAYS, "send COMM_REQUEST_PROVIDE_SERVICE ok\n");

	// ------------------------------------
	//	wait for connection request
	// ------------------------------------
	if (!jsp.setupServiceAcceptSocket())
	{
		Jsp::printLog(LOG_FATAL, "failed to setup service accept socket\n");
		return 1;
	}

	// ------------------------------------
	//	service process loop
	// ------------------------------------
	jsp.recognizeLoop();

	// ------------------------------------
	//	kill julius
	// ------------------------------------
	jsp.killJulius();

	return 0;
}

void quitJuliusSp(int sig)
{
	std::string strSignal = getSignalString(sig);

	Jsp::printLog(LOG_FATAL, "**** signal (%d:%s) received ****\n", sig, strSignal.c_str());

	jsp.killJulius();

	exit(1);
}

std::string getSignalString(int sig)
{
	std::string strSignal;

	switch (sig)
	{
		case SIGHUP:
			strSignal = "SIGHUP";
			break;

		case SIGINT:
			strSignal = "SIGINT";
			break;

		case SIGQUIT:
			strSignal = "SIGQUIT";
			break;

		case SIGKILL:
			strSignal = "SIGKILL";
			break;

		default:
			strSignal = "** unknown **";
	}

	return strSignal;
}

char *getJspConfigPath()
{
	static char jspConfigPath[1024];

	// ------------------------------------
	//	get environment variable
	// ------------------------------------
	char *jspConfDir = getenv("SIGVERSE_JSP_CONF_DIR");

	if (jspConfDir)
	{
		// env var found
		strcpy(jspConfigPath, jspConfDir);
	}
	else
	{
		// env var not found
		strcpy(jspConfigPath, "");
	}

	// ------------------------------------------------------
	//	append "/" to the last of buffer
	//	only if buffer is not empty and last char is not "/"
	// ------------------------------------------------------
	int n = strlen(jspConfigPath);
	if (n>0 && jspConfigPath[n-1]!='/')
	{
		strcat(jspConfigPath, "/");
	}

	// ------------------------------------------
	//	append config file name
	// ------------------------------------------
	strcat(jspConfigPath, "sigjsp.conf");

	return jspConfigPath;
}

