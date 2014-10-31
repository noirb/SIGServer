// ***********************************************
//      SIGVerse Server (sigserver) main function
// ***********************************************
/*
 * Modified by inamura on 2013-12-29
 */

// TODO: IRWAS related macro should be replaced: by inamura
#ifdef IRWAS_SIMSERVER

#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>

#include "SSimWorld.h"
#include "WorldXMLReader.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"
#include "../x3d/parser/cpp/X3DParser/CJNIUtil.h"
#include "Logger.h"
#include "ServerAcceptProc.h"
#include "SimWorldProvider.h"
#include "WorldSimulator.h"
#include "ServiceNameServer.h"
#include "fname.h"
#include "FilenameDB.h"

static bool fileExist(const char *fname)
{
  return true;
}

#define ARRAY_SIZE(ARY) (int)(sizeof(ARY)/sizeof(ARY[0]))

static SSimWorld *s_world = 0;

static std::string s_worldfile = "MyWorld.xml";
static FilenameDB s_fdb;

typedef std::vector<int> PidC;
static PidC s_childpids;
static WorldSimulator *s_sim = NULL;

// Termination of SIGVerse world
static void quit(int )
{
	LOG_SYS(("recv SIGINT. Die"));
	CJNIUtil::destroy();
	typedef PidC T;
	for (T::iterator i=s_childpids.begin(); i!=s_childpids.end(); i++) {
		int pid = *i;
		kill(pid, SIGINT);
	}
	s_childpids.clear();
	if (s_sim != NULL) {
		s_sim->terminate();
	}
}

static bool runControllers(SSimWorld &w, int port)
{
	typedef SSimWorld::M M;
	//	const M &objs = w.objs();
	char *runprog = getenv("SIGVERSE_RUNAC");

	typedef SSimWorld::C C;
	C objs;
	w.copyObjs(objs);
	if (objs.size() > 0 && !runprog) {
		LOG_ERR(("Cannot get SIGVERSE_RUNAC parameter"));
		LOG_ERR(("Fail to create model processes"));
		return false;
	}

	for (C::iterator j=objs.begin(); j!=objs.end(); j++) {

		SSimObj *obj = *j;

		std::string values[2];

		const char *attrs[] = { "language", "implementation", };
		try {
			for (int i=0; i<ARRAY_SIZE(attrs); i++) {
				Attribute &attr = obj->getAttr(attrs[i]);
				values[i] = attr.value().getString();

			}
		} catch(SSimObj::NoAttributeException &) {;}

		std::string ctrl = values[1];
		if (ctrl.length() <= 0) { continue; }

		if (!fileExist(ctrl.c_str())) {
			LOG_ERR(("%s : Controller file DOES NOT exist !!", ctrl.c_str()));
			return false;
		}

		if (values[0] == "c++") {

			int pid = fork();
			if (pid < 0) {
				LOG_ERR(("Cannot create process"));
				exit(1);
			} else if (pid == 0) { // child process
				nice(10);

				LOG_DEBUG1(("run controller proc : \"%s\" -> %s\n", obj->name(), ctrl.c_str()));

				char portbuf[128];
				sprintf(portbuf, "%d", port);
				char *argv[] = {runprog,
				                (char *)"-h", "127.0.0.1",
				                (char *)"-p", portbuf,
				                (char *)"-n", strdup(obj->name()),
				                (char *)"-l", strdup(ctrl.c_str()),
				                (char *)"-s", // silent
				                NULL };

				delete s_world; s_world = 0;

				execvp(argv[0], argv);

				LOG_ERR(("Failed to create controller process"));
				exit(1);
			}
		}
	}

	return true;
}


static X3DDB s_db;

// FIX20110421(ExpSS)
static SimpleShapeDB s_ssdb;


static bool  createWorld(int port)
{
#ifndef _DEBUG
	try
#endif
		{
			s_db.clear();

			// XML reader
			WorldXMLReader read(s_fdb, s_db, s_ssdb);

			// Reading the world file
			if (!read(s_worldfile.c_str())) {
				fprintf(stderr, "cannot read world file : %s\n", s_worldfile.c_str());
				return false;
			}
			// Construction of SIGVerse world from reading result
			SSimWorld *w = read.release();

			if (w && port >= 0) {
				// Initialization of controller
				if (!runControllers(*w, port)) {
					delete w;
					return false;
				}
				delete s_world;
			}
			s_world = w;
#ifndef _DEBUG
		} catch(SSimObj::Exception &e) {
		fprintf(stderr, "%s\n", e.msg());
		return false;
#endif
	}
	return true;
}


static bool  createWorld()
{
	return createWorld(-1);
}


static void printUsage()
{
	fputs("USAGE:\n", stdout);
	fputs(" %  simserver [-v <number>] [-p <number>] [-c <dir>] [-w <file>]\n", stdout);
	fputs("\n", stdout);
	fputs(" OPTION :\n", stdout);
	fputs("    -p <number>  : server port number\n", stdout);
	fputs("    -c <dir>     : data directory (default: ./conf)\n", stdout);
	fputs("    -w <file>    : world file (default: ./conf/MyWorld.xml)\n", stdout);
	fputs("    -v <number>  : configure file directory\n", stdout);
}


#define FREE(P) if (P) { delete P; P = 0; }

inline double CALC_LENGTH(double vx, double vy, double vz)
{
	return sqrt(vx*vx + vy*vy + vz*vz);
}

// Class to access the SIGVerse world
class SimWorldProviderImpl : public SimWorldProvider
{
private:
	// Get the SIGVerse world information
	SSimWorld *get() { return s_world; }

	// Create the SIGVerse world
	SSimWorld *create() {
		createWorld();
		return s_world;
	}

	// Get X3D Database
	X3DDB & getX3DDB() {
		return s_db;
	}
	double calcDistance(const char *name1, const char *name2)
	{
		assert(s_world);
		SSimObj *obj1 = s_world->getSObj(name1);
		SSimObj *obj2 = s_world->getSObj(name2);
		if (!obj1 || !obj2) { return -1; }

		double vx = obj1->x() - obj2->x();
		double vy = obj1->y() - obj2->y();
		double vz = obj1->z() - obj2->z();

		return CALC_LENGTH(vx, vy, vz);
	}
};

int main(int argc, char **argv)
{
	int port = SIGSERVER_DEFAULT_PORT_NUM; // Default port number
	int loglevel = LOG_MSG;   // Log level
	double endT = -1;         // Time to terminate the simulation, -1 means the simulation never ends

	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(loglevel);

	// Whether the port No is given or not
	bool setPort = false;

	// Time measurement
#ifdef TDEBUG  
	file = fopen("tdebug.txt","w");
#endif
	int ch;
	extern char *optarg;
	while ((ch = getopt(argc, argv, "p:v:c:w:t:")) != -1) {
		switch(ch) {

		// Port number is given
		case 'p': {
			port = atoi(optarg);
			setPort = true;
			break;
		}
		// Log level is given
		case 'v': {
			int v = atoi(optarg);
			LOG_LEVEL_INCR(loglevel, v);
		}
		break;

		// Directory of configuration files such as world file
		case 'c':
			if (!s_fdb.pushDirectory(optarg)) {
				fprintf(stderr, "%s is not directory\n", optarg);
				return 1;
			}
		break;
		case 'w': {
			s_worldfile = optarg;
			// get a directory of world file from optarg
			char *dir = fname_get_parent(optarg);
			if (dir != NULL) {
				// Store the filename into database
				s_fdb.pushDirectory(dir);
			} 
			// If the directory is not given, use the default directory
			else {
				s_fdb.pushDirectory("./conf");
			}

			char buf[1024];
			strcpy(buf, s_worldfile.c_str());

			// Get the world file name
			char *fname = fname_get_file(buf);
			if (fname != NULL) {
				s_worldfile = fname;
			}
		}
		break;
		// Get the time to be terminated
		case 't':
			endT = atof(optarg); break;
		default:
			printUsage();
			return 1;
		}
	}

	// Get directory from the environmental variable SIGVERSE_DATADIR
	char *dir = getenv("SIGVERSE_DATADIR");
	if (dir != NULL) {
		std::string d = dir;
		d += "/xml";
		s_fdb.pushDirectory(d.c_str());
		d = dir;
		d += "/shape";
		s_fdb.pushDirectory(d.c_str());
	}

	// Creation of socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("cannot open socket");
		return 1;
	}

	struct sockaddr_in addr;

	// Configuration of socket
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
  
	// avoid "Address already in use"
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, NULL, 0);

	bool connect = false;
	if(!setPort) {
		while(connect == false) {
			if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
				//perror("cannot bind socket");
				port += SIGSERVER_DEFAULT_PORT_BAND;
				addr.sin_port = htons(port);
				//return 1;
			}
			else  connect = true;
		}
		LOG_SYS(("///////////////////////////////////////////////////////////"));
		LOG_SYS(("////////////// World number %d  (port %d) ///////////////", (port - SIGSERVER_DEFAULT_PORT_NUM)/SIGSERVER_DEFAULT_PORT_BAND, port));
		LOG_SYS(("///////////////////////////////////////////////////////////"));
	}
	else {
		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			perror("cannot bind socket");
		}
	}
  
	// Use bigger queue for listen
	listen(sock, 20); //TODO: Magic number should be removed: by inamura

	dInitODE();

	// Displaying the directories for reading
	LOG_SYS(("data directory : "));
	int n = s_fdb.getSize();
	for (int i=0; i<n; i++) {
		const char *fname = s_fdb.getDirectory(i);
		LOG_SYS(("\t[%d]  %s", i+1, fname));
	}

	LOG_SYS(("world file : %s", s_worldfile.c_str()));

	// Initialization of Data transport
	CommDataEncoder::setSendProc(CommUtil::sendData);

	// Creat instance which can access to the SIGVerse world
	SimWorldProviderImpl provider;

	// Instace to respond to controller, and manage Service Provider
	ServiceNameServer ns;

	// Instance for request acception
	ServerAcceptProc accept(sock, provider, ns);

	// Creation of simulator
	s_sim = new WorldSimulator(accept, provider, ns);

	// Request acception is executed in another thread
	AcceptThread thread;
	thread.run(&accept);


	if(SIG_ERR == signal(SIGINT, quit)) {
		LOG_SYS(("failed to set signal handler."));
	}

	// Creation of SIGVerse world
	bool bWorldCreated = createWorld(port);

#if 0
	// konao
	//	s_ssdb.dump();
	if (s_world)
		{
			printf("**** world dump ****\n");
			s_world->dump();
			printf("done\n");
		}
#endif

	if (bWorldCreated)
		{
			LOG_SYS(("waiting for connection..."));
			s_sim->loop(endT);
		}

	accept.close();
	delete s_sim; s_sim = NULL;

	//thread.terminate();
	//thread.wait();
#ifdef TDEBUG
	fclose(tdebug);
#endif

	exit(1);
	//close(sock);
	return 0;
}

#endif  // #ifdef IRWAS_SIMSERVER
