/* $Id: glview.cpp,v 1.2 2011-09-09 07:38:05 okamoto Exp $ */
#include "systemdef.h"

#ifdef USE_OPENGL

#include <GL/gl.h>
#include <GL/glut.h>
#include <assert.h>
#include "SimWorld.h"
#include "WorldXMLReader.h"
#include "SimViewPoint.h"

#ifdef IRWAS_TEST_CLIENT
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>
#include <signal.h>

#include "CommDataType.h"
#include "CommDataDecoder.h"
#include "CommDataEncoder.h"
#include "SimGround.h"
#include "Source.h"
#include "CommUtil.h"
#include "Logger.h"
#include "ct/CTReader.h"
#include "X3DDB.h"

#endif

static pid_t s_servicPID = -1;
static void quit(int)
{
	if (s_servicPID > 0) {
		kill(s_servicPID, SIGKILL);
		s_servicPID = -1;
	}
	exit (0);
}

#define FREE(P) if (P) { delete P; P = 0; }

static void resize(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(30.0, (double)w/(double)h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

static SimViewPoint *s_viewpoint = 0;
static SimWorld *s_world = 0;

#ifdef IRWAS_TEST_CLIENT

static Source *s_viewSource = 0;

const Service::Kind SERVICES = Service::CAPTURE_VIEW | Service::DETECT_ENTITIES | Service::DISTANCE_SENSOR;

class CaptureServiceProviderDataListener : public CommDataDecoder::Listener
{
private:
	ViewImage *m_img;
public:
	CaptureServiceProviderDataListener() : m_img(0) {
		m_img = ViewImage::createSample();
	}
	~CaptureServiceProviderDataListener() {
		delete m_img;
	}
		
	void recvRequestCaptureViewImage(Source &from, RequestCaptureViewImageEvent &evt)
	{
		printf("capture request from %s\n", evt.getAgentName());
		const ViewImageInfo &info = evt.getImageInfo();
		LOG_DEBUG1(("size = (%d, %d)", info.getWidth(), info.getHeight()));
		CommResultCaptureViewImageEncoder enc(*m_img);
		int n = enc.send(from.socket());
		printf("%d bytes sent\n", n);
	}
	void recvRequestDetectEntities(Source &from, RequestDetectEntitiesEvent &evt)
	{
		CommResultDetectEntitiesEncoder enc;
		enc.pushDetectedEntity("Robot1");
		enc.pushDetectedEntity("Robot2");
		enc.pushDetectedEntity("Agent1");
		enc.send(from.socket());
	}

	void recvRequestNSPinger(Source &from, RequestNSPingerEvent &evt)
	{
		bool active = evt.kind() & SERVICES? true: false;
		CommResultNSPingerEncoder enc(active);
		enc.send(from.socket());
	}
		//void recvResultNSPinger(Source &from, ResultNSPingerEvent &) = 0;
	
};

static pid_t startCaptureService(const char *server, int simPort, int servicePort)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("cannot open service provider port");
		return false;
	}

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, NULL, 0);
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(servicePort);

		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			perror("cannot bind socket");
			return 1;
		}
	}

	listen(sock, 5);

	int pid = fork();
	if (pid == 0) { // child process

		{
			int sock = CommUtil::connectServer(server, simPort);
			Service::Kind kind = SERVICES;
			Service s("glview", kind, servicePort);
			CommRequestProvideServiceEncoder enc(s);
			enc.send(sock);
			close(sock);
		}


		for (;;) {
			struct sockaddr_in addr;
			socklen_t len;
			int s = accept(sock, (sockaddr*)&addr, &len);
			if (fork() == 0) {
				close(sock);
				
				if (s < 0) {
					perror("service provider : cannot accept");
					exit (1);
				}

				char buf[256];
				int r = read(s, buf, sizeof(buf));
				if (r > 0) {
					Source src(s, inet_ntoa(addr.sin_addr));
					CaptureServiceProviderDataListener l;
					CommDataDecoder decoder;
					decoder.setListener(&l);
					int rbytes = decoder.push(src, buf, r);
					printf("capture device : rbytes = %d\n", rbytes);
					
				} else {
					perror("read error");
				}
				close(s);
				exit(0);
			} else {
				close(s);
			}
		}
		exit(0);
	}

	close(sock);

	// parent
	return pid;
}

static bool initNET(char *hostname, int port)
{
	CommDataEncoder::setSendProc(CommUtil::sendData);
	int sock = CommUtil::connectServer(hostname, port);

	{

		CommRequestAttachViewEncoder enc("testview", true);
		int r = enc.send(sock);
		if (r <= 0) {
			close(sock);
			return false;
		}

		CommDataDecoder decoder;
		CTReader * reader = new CTReader(sock, decoder, 20000);

		CommDataDecoder::Result *result = reader->readSync();
		if (result) { delete result; }
		delete reader;

	}

	s_viewSource = new Source(sock, hostname);
	s_viewSource->set(SOURCE_TYPE_SIMSERVER, "simserver");

	return true;
}
#else
static bool initNET(char *hostname, int port) { return true; }
#endif

static void initGL()
{
#ifdef USE_ODE
	dInitODE();
#endif
	glClearColor(0.8, 0.8, 0.9, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}



static void idle()
{
	//       	s_world->nextStep();
	//#ifdef IRWAS_TEST_CLIENT
#if 0
	if (s_viewSource  != NULL) {
		SOCKET sock = s_viewSource->socket();

		CommRequestGetAllEntitiesEncoder enc;
		enc.send(sock);

		CommDataDecoder decoder;
		CTReader * r = new CTReader(sock, decoder, 20000);

		CommDataDecoder::Result *result = r->readSync();
		if (result) {
			//				fprintf(stderr, "data type = %d\n", result->type());
			if (result->type() == COMM_RESULT_GET_ALL_ENTITIES) {
				ResultGetAllEntitiesEvent *evt = (ResultGetAllEntitiesEvent*)result->data();
				SimWorld *w = evt->releaseWorld();
				w->setGround(new SimGround());
				SimWorld *tmp = s_world;
				s_world = w;
				delete tmp;
				glutPostRedisplay();
				//				usleep(10000);
			} else if (result->type() == COMM_LOG_MSG) {
				LogMsgEvent *evt = (LogMsgEvent*) result->data();

				LOG_PRINT(evt->level(), ("%s", evt->msg()));
			} else if (result->type() == COMM_RESULT_ATTACH_VIEW) {
				ResultAttachViewEvent *evt = (ResultAttachViewEvent *)result->data();
				X3DDB *db = evt->getX3DDB();
				if (db) {
					X3DDB::Iterator *itr = db->getIterator();
					while (true) {
						const char *k = itr->nextKey();
						if (!k) { break; }
						const char *v = db->get(k);
						LOG_DEBUG1(("X3DDB : (%s -> %s)", k, v));
					}
					delete itr; 
				}

			}
			delete result;
				       
		}
		delete r;

	}
#endif
}

static void display()
{

	//	fprintf(stderr, "display\n");

	static GLfloat lightpos[] = {3.0, 4.0, 5.0, 1.0};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	if (s_world) {
		GLContext c;
		s_viewpoint->draw(c);
		//		fprintf(stderr, "draw\n");
		s_world->draw(c);
	}

	glutSwapBuffers();
}

static void keyboard(unsigned char key, int x, int y)
{
#ifdef IRWAS_TEST_CLIENT
	if (s_viewSource) {
		CommDataEncoder *enc = 0;
		switch(key) {
		case 'u':
			enc = new CommRequestSimCtrlEncoder(SIM_CTRL_COMMAND_START);
			break;
		case 'i':
			enc = new CommRequestSimCtrlEncoder(SIM_CTRL_COMMAND_STOP);
			break;
		case 'o':
			enc = new CommRequestSimCtrlEncoder(SIM_CTRL_COMMAND_RESTART);
			break;
		case 'q':
			quit(0);
			break;
		default:
			break;
		}
		if (enc) {
			enc->send(s_viewSource->socket());
			delete enc;
		}
	}
#endif

	if (s_viewpoint->setKey(key)) {
		glutPostRedisplay();
	}
}

int main(int argc, char **argv)
{
	char	server[128] = "localhost";
	int	port = 5678;
	int	verbose = 0;
	int 	servicePort = 6500;

#ifdef IRWAS_TEST_CLIENT
	{
		int ch;
		extern char *optarg;
		while ((ch = getopt(argc, argv, "s:p:v:c:")) != -1) {
			switch(ch) {
			case 's':
				strcpy(server, optarg); break;
			case 'p':
				port = atoi(optarg); break;
			case 'v':
				verbose = atoi(optarg);	break;
			case 'c':
				servicePort = atoi(optarg); break;
			default:
				break;
			}
		}
	}

#endif

	s_servicPID = startCaptureService(server, port, servicePort);
	
	int level = LOG_MSG;
	LOG_LEVEL_INCR(level, verbose);
	
	LOG_OUTPUT_LEVEL(level);
	LOG_STDOUT();
	
#ifdef EXEC_SIMULATION
	const char *worldfile = "MyWorld.xml";
	WorldXMLReader read("./conf");
	read(worldfile);
	s_world = read.release();
	s_world->start();
#endif
	s_viewpoint = new SimViewPoint(Position(0.0, -0.5, -15.0));
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	initGL();

	while (!initNET(server, port)) {
		sleep(5);
	}

	signal(SIGINT, quit);

	glutMainLoop();

#ifdef IRWAS_TEST_CLIENT
	FREE(s_viewSource);
#endif
	delete s_world;
	delete s_viewpoint;
	return 0;
}

#endif


