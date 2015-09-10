/*
 * Written by Kawamoto on 2011-04-05
 * Modified by Tetsunari Inamura on 2014-01-03
 *    English comment is added (translation is finished from v2.2.0)
 * Modified by Tetsunari Inamura on 2014-01-06
 *    enum for new communication protocal was moved to CommDataType.h
 */

#ifndef WorldSimulator_h
#define WorldSimulator_h

#include <sigverse/commonlib/CommDataDecoder.h>
#include <sigverse/commonlib/Controller.h>
#include <sigverse/simserver/SSimWorld.h>
#include <sigverse/simserver/DecoderListener.h>
//#include "DynamicsData.h"
//#include "./comm/controller/NoData.h"

#define SIGSERVER_DEFAULT_PORT_NUM  9000 // Default port num (first of all ports in the band)
#define SIGSERVER_DEFAULT_PORT_BAND 5    // Number of ports used by sole SIGVerse world

//#define CONTROLLER_LOCAL_TEST


class ServerAcceptProc;
class SimWorldProvider;
class LogTransfer;
class PacketForward;
class DecoderListener;
class ServiceNameServer;


class WorldSimulator
{
private:
	ServerAcceptProc   &m_accept;
	SimWorldProvider   &m_provider;
	ServiceNameServer  &m_ns;
	DecoderListener    *m_l;
	PacketForward      *m_forward;
	LogTransfer        *m_log;
	bool                m_loop;

	// Step counter of inner timer. It starts from when the simulation was started.
	unsigned int stepCount;

	// Time step width in micro second
	double m_stepTime;

	enum ControllerDataType
	{
		SEND_MESSAGE = 0x0001,    	// Send a message to controller
	};

	enum RequestMethodReturnType
	{
		NEXT_CLIENT,
		NEXT_MESSAGE,
		NEXT_STEP,
		RETURN_TRUE,
		RETURN_FALSE,
	};

public:
	//! Constructor
	WorldSimulator(ServerAcceptProc &accept, SimWorldProvider &provider, ServiceNameServer &ns);
	//! Destructor
	~WorldSimulator() { free_(); }
private:
	bool runStep();
	void free_();
	std::string DoubleToString(double x);
	std::string IntToString(int x);
public:
	/**
	 * @brief main loop function in the simulation
	 *
	 * @param End time of the simulation
	 * If the end time is negative, the simulation never ends.
	 */
	void loop(double endT = -1);

	//! Exit from the main loop of the simulation
	void terminate() { m_loop = false; }

	//! Send all of the entity data to SIGViewer
	bool sendAllEntities(SOCKET sock, const char* buf);

	//! Send entity data only which have moved, to SIGViewer
	bool sendMoveEntities(SOCKET sock, bool update = false);

	//! Send shape file
	bool sendShapeFile(SOCKET sock, std::string name);

	//! Send data
	bool sendData(SOCKET sock, const char *msg, int size);

	//! Receive data
	bool recvData(SOCKET sock, char *msg, int size);

	//! Forward data to entity
	//! data should be separated in parts, such as "[message],[destination 1],[destination 2],"
	bool sendOnMsg(SOCKET sock, std::string data, std::string from);

	// Start the simulation
	void startSimulation(SSimWorld *w);

	RequestMethodReturnType requestGetAllEntitiesfirst  (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *pbuf, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestStartSimulation      (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestStopSimulation       (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetMoveEntities      (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestDownloadShape        (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestDisconnect           (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestQuit                 (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSendmsgFromController(SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestConnectService       (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetJointPosition     (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetJointQuaternion   (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetPointingVector    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetPartsPosition     (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetPartsQuaternion   (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetEntityPosition    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestCheckService         (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetEntityPosition    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetEntityRotation    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetEntityRotation    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestCheckEntity          (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetCameraPosition    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetCameraPosition    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetCameraDirection   (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetCameraDirection   (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetCameraFov         (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetCameraAspectratio (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetWheel             (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetWheelVelocity     (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetJointAngle        (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetJointVelocity     (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGraspObject          (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestReleaseObject        (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetAllJointAngles    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestWorldStep            (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestWorldQuickStep       (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetIsGrasped         (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestSetCollisionable     (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetSimulationTime    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
	RequestMethodReturnType requestGetCollisionState    (SSimWorld *w, const std::vector<Connection*> &clients, Source *client, SOCKET &s, char *p, unsigned short &packetSize, int &i);
};

#endif // WorldSimulator_h
