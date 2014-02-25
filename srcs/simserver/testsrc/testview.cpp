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
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <getopt.h>

#include "CommDataType.h"
#include "CommDataDecoder.h"
#include "CommDataEncoder.h"
#include "SimGround.h"
#include "Source.h"
#include "CommUtil.h"
#include "Logger.h"
#include "CaptureServiceProvider.h"

#endif

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
static CommDataDecoder *s_decoder = 0;

class CaptureServiceProviderDataListener : public CommDataDecoder::Listener
{
	void recvRequestCaptureViewImage(Source &from, RequestCaptureViewImageEvent &evt)
	{
		printf("capture request from %s", evt.getAgentName());
		const ViewImageInfo &info = evt.getImageInfo();
		LOG_DEBUG1(("size = (%d, %d)", info.getWidth(), info.getHeight()));
		ViewImage *img = ViewImage::createSample();
		CommResultCaptureViewImageEncoder enc(evt.getAgentName(), *img);
		enc.send(from.socket());
		delete img;
	}
	void recvRequestDetectEntities(Source &from, RequestDetectEntitiesEvent &evt)
	{
		CommResultDetectEntitiesEncoder enc(evt.getAgentName());
		enc.pushDetectedEntity("Agent1");
		enc.pushDetectedEntity("Agent2");
		enc.pushDetectedEntity("Agent3");
		enc.send(from.socket());
	};
};


class DataListener : public CommDataDecoder::Listener
{
private:
	void recvRequestSimCtrl(Source &from, RequestSimCtrlEvent &evt) {;}

	void recvRequestGetAllEntities(Source &from, RequestGetAllEntitiesEvent &evt) {;}
	void recvResultGetAllEntities(Source &from, ResultGetAllEntitiesEvent &evt)
	{
		SimWorld *newWorld = evt.releaseWorld();
		newWorld->setGround(new SimGround());
		SimWorld *tmp = s_world;
		s_world = newWorld;
		delete tmp;
	}

	void recvRequestUpdateEntities(Source &from, RequestUpdateEntitiesEvent &evt) {;}
	void recvRequestGetEntity(Source &from, RequestGetEntityEvent &evt) {;}
	void recvResultGetEntity(Source &from, ResultGetEntityEvent &evt) {;}

	void recvRequestAttachController(Source &from, RequestAttachControllerEvent &evt) {;}
	void recvResultAttachController(Source &from, ResultAttachControllerEvent &evt) {;}

	void recvRequestCaptureViewImage(Source &from, RequestCaptureViewImageEvent &evt) {;}
	void recvResultCaptureViewImage(Source &from, RequestCaptureViewImageEvent &evt) {;}

	void recvRequestAttachView(Source &from, RequestAttachViewEvent &evt) {;}
	void recvResultAttachView(Source &from, ResultAttachViewEvent &evt) 
	{
		CommResultType r = evt.result();
		printf("%s\n", (r == COMM_RESULT_OK)? "connected": "failed");
		if (r != COMM_RESULT_OK) {
			//close(s_sock);
			FREE(s_viewSource);
			exit(1);
		}
	}

	void recvLogMsg(Source &from, LogMsgEvent &evt)
	{
		//printf("recvLog\n%s", evt.msg());
		LOG_PRINT(evt.level(), ("%s", evt.msg()));
	}
};

static CaptureServiceProviderDataListener s_spListener;
static CaptureServiceProviderThread *s_thread = 0;

static bool initNET(char *hostname, int port)
{
	CommDataEncoder::setSendProc(CommUtil::sendData);
	{

		int sock = CommUtil::connectServer(hostname, port);
		CommRequestAttachViewEncoder enc("testview");
		int r = enc.send(sock);
		if (r <= 0) {
			close(sock);
			return false;
		}
		s_viewSource = new Source(sock, hostname);
		s_viewSource->set(SOURCE_TYPE_SIMSERVER, "simserver");
	}

	{
		CaptureServiceProvider *provider = new CaptureServiceProvider(s_spListener);
		if (!provider->init(hostname, port)) {
			fprintf(stderr, "service provider : cannot connect to server");
			return false;
		}
		s_thread = new CaptureServiceProviderThread(provider);
		s_thread->run();
	}

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
	s_world->nextStep();
	glutPostRedisplay();
}

static void display()
{

#ifdef IRWAS_TEST_CLIENT
	if (s_viewSource > 0 && s_decoder) {
		int sock = s_viewSource->socket();

		fd_set rfds;
		struct timeval tv;
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100;

		int r = select(sock + 1, &rfds, NULL, NULL, &tv);
		//		fprintf(stderr, "select (%d)\n", r);
		if (r >= 0 && FD_ISSET(sock, &rfds)) {
			char buf[20000];
			int rbytes = read(sock, buf, sizeof(buf));
			//			fprintf(stderr, "%d bytes read\n", rbytes);
			if (rbytes <= 0) {
				perror("read");
			}
			if (!s_decoder->push(*s_viewSource, buf, rbytes)) {	
				fprintf(stderr, "data error\n");
			}
		}
	}
#endif

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

#ifdef IRWAS_TEST_CLIENT
	{
		int ch;
		extern char *optarg;
		while ((ch = getopt(argc, argv, "s:p:v:")) != -1) {
			switch(ch) {
			case 's':
				strcpy(server, optarg); break;
			case 'p':
				port = atoi(optarg); break;
			case 'v':
				verbose = atoi(optarg);	break;
			default:
				break;
			}
		}
	}

	DataListener *l = new DataListener();
	s_decoder = new CommDataDecoder();
	s_decoder->setListener(l);
#endif
	
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


	glutMainLoop();

#ifdef IRWAS_TEST_CLIENT
	if (s_thread) { s_thread->terminate(); }
	FREE(s_thread);
	FREE(s_viewSource);
	delete l;
	delete s_decoder;
#endif
	delete s_world;
	delete s_viewpoint;
	return 0;
}

#endif

