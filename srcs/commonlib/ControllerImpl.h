/*
 * Written by noma on 2012-03-27
 * Modified by Yoshiaki Mizuchi on 2014-01-16
 *    Modify indent
 * Modified by Yoshiaki Mizuchi on 2014-01-21
 *    Add English comment
 */

#ifndef ControllerImpl_h
#define ControllerImpl_h

#include "ControllerInf.h"
#include "RawSound.h"
#include "systemdef.h"
#include "Logger.h"
#include "ViewImage.h"
#include <vector>
#include <map>
#include <string>


class Command;
class CTSimObj;
class ControllerImpl;

namespace SocketUtil {
	bool sendData(SOCKET sock, const char* msg, int size);
	bool recvData(SOCKET sock, char* msg, int size);
}

//! Class for sending and receiving data between a controller and a service provider
class BaseService 
{
public:
	BaseService(){;} 

	BaseService(std::string name, unsigned short port, SOCKET sock) 
	  : m_name(name), m_port(port), m_sock(sock), m_getData(false) {;}
	~BaseService(){
	/*
	if (m_image != NULL) {
		delete m_image;
		m_image = NULL;
	}
	*/
	}

	//! Start data receiving loop
	bool startServiceLoop(ControllerImpl *con);

	//! End data receiving loop
	void endServiceLoop();

	//! Get service provider name
	std::string getName() { return m_name; }

	//! Get a socket for receiving data from a service provider
	SOCKET getSocket() { return m_sock; }

	//! Get a socket for connecting to a service provider
	SOCKET getClientSocket() { return m_clientSock; }

	//! Set a socket for connecting to a service provider
	void setClientSocket(SOCKET sock) { m_clientSock = sock; }

	//! Send message
	bool sendMsgToSrv(std::string msg);

	//! Set that data was received
	void setGetData(bool get){ m_getData = get; }

	//! Set detected object names
	void setDetectedNames(std::vector<std::string> names) { m_detectedNames = names; }
	/*
	ViewImage* getImage() {
		return m_image;
	}
	*/

	//! Get the name of an entity that use service
	std::string getEntityName() { 
		return m_entname; 
	}

	//! Set the name of an entity that use service
	void setEntityName(std::string name) {
		m_entname = name;
}

protected:
	// Service name
	std::string m_name;

	// Port number (Controller)
	unsigned short m_port;

	// Socket
	SOCKET m_sock;

	// Socket (Client side)
	SOCKET m_clientSock;

	//! Whether data was received
	bool m_getData;

	// Name of entity that use service
	std::string m_entname;

	// Image data
	std::vector<ViewImage*> m_images;

	// Distance data
	unsigned char m_distance;

	// Result of detectEntities
	std::vector<std::string> m_detectedNames;
};

class ViewService : public BaseService
{
public:
	ViewService(){;} 
	
	ViewService(std::string name, unsigned short port, SOCKET sock) 
		: BaseService(name, port, sock) {;}

	/**
	 * @brief Detects entities in view (Using the functions of service provider)
	 *
	 * @param v Container of entity names
	 * @param id Camera ID
	 * @retval true Success
	 * @retval false Failure
	 */
	bool detectEntities(std::vector<std::string> &v, int id = 1);

	/**
	 * @brief Gets the viewpoint image of an agent
	 * @param camID Camera ID
	 * @param ctype Image type
	 * @param size  Image sze
	 * @retval != NULL Image Data
	 * @retval NULL    Failure
	 */
	ViewImage *captureView(int camID = 1, ColorBitType ctype = COLORBIT_24, ImageDataSize size = IMAGE_320X240);

	/**
	 * @brief Gets distance to the object in the camera direction
	 * @param start Minimum distance (returns as 0)
	 * @param end   Maximum distance (returns as 255)
	 * @param ctype Bit depth
	 * @param camID Camera ID
	 * @retval 0~255 Distance data
	 */
	unsigned char distanceSensor(double start = 0.0, double end = 255.0, int camID =1, ColorBitType ctype = DEPTHBIT_8);

	/**
	 * @brief Gets 1D distance to objects in the camera view
	 * @param start Minimum distance (returns as 0)
	 * @param end   Maximum distance (returns as 255)
	 * @param camID Camera ID
	 * @param ctype Image data type
	 * @param size  Distance data size
	 */
	ViewImage *distanceSensor1D(double start = 0.0, double end = 255.0, int camID = 1, ColorBitType ctype = DEPTHBIT_8, ImageDataSize size = IMAGE_320X1);

	/**
	 * @brief Gets 2D distance to objects in the camera view
	 * @param start Minimum distance (returns as 0)
	 * @param end   Maximum distance (returns as 255)
	 * @param camID Camera ID
	 * @param ctype Image data type
	 * @param size  Distance data size
	 */
	ViewImage *distanceSensor2D(double start = 0.0, double end = 255.0, int camID = 1, ColorBitType ctype = DEPTHBIT_8, ImageDataSize size = IMAGE_320X240);

private:
	/**
	 * @brief send distanceSensor request to service provider
	 * @param type  Request type (Dimension of data to receive)
	 * @param start Minimum distance (returns as 0)
	 * @param end   Maximum distance (returns as 255)
	 * @param camID Camera ID
	 * @param ctype Image type
	 * @param size  Image size
	 */
	bool sendDSRequest(int type, double start = 0.0, double end = 255.0, int camID = 1, ColorBitType ctype = DEPTHBIT_8, ImageDataSize size = IMAGE_320X240);

};

/**
 * @brief Controller class for network communication
 *
 * communicates with the simulation server
 */
class ControllerImpl : public ControllerInf
{
private:
	typedef ControllerInf Super;

private:
	std::string m_server;
	int         m_port;
	std::string m_myname;

	bool      m_isAttached;
	CTSimObj *m_ctSimObj;

protected:
	SOCKET          m_cmdSock;
	SOCKET          m_dataSock;
	SOCKET          m_srvSock; // for service provider
	unsigned short  m_srvPort; // port number for service provider
	SOCKET          m_tmpSock;

	// Save sockets of each service provider
	std::map<std::string, SOCKET> m_srvSocks; 

	// Whether service provider connection is successed
	bool m_connected;

	// Whether connection to service provider is successed
	bool m_connectService;

	// Socket for service provider
	std::vector<BaseService*> m_services;

protected:
	CTSimObj &getCTSimObj();

	/**
	 * @brief Connects service providers (Port number is set as main port number +1)
	 *
	 * @param name          Service provider name
	 * @retval BaseService  Service
	 * @retval NULL         Failure
	 */
	BaseService* connectToService(std::string name);

	/**
	 * @brief Connects service providers (Designated port number)
	 *
	 * @param name          Service provider name
	 * @param port          Port number that is used in server
	 * @retval BaseService  Service
	 * @retval NULL         Failure
	 */
	BaseService* connectToService(std::string name, unsigned short port);

	/**
	 * @brief Disconnects from service provider
	 *        
	 * @param name   Service provider name
	 */
	void disconnectToService(std::string name);

	/**
	 * @brief  Check to the server about whether service provider is usable
	 * @param  name  Service name for check
	 * @retval true  Service is usable
	 * @retval false Service is unusable
	 */
	bool checkService(std::string name);

private:
	// bool recvData(SOCKET sock, char *msg, int size);

public:
	static void *serviceThread(void *pParam);

protected:
	void close_();

protected:
	//! Constructor
	ControllerImpl() : Super(),
		m_port(-1),
		m_isAttached(false),
		m_ctSimObj(NULL),
		m_cmdSock(-1),
		m_dataSock(-1),
		m_srvSock(-1),
		m_tmpSock(-1),
		m_connected(false),
		m_connectService(false)
	{;}

	//! Destructor
	~ControllerImpl() {
		close_();
	}

public:
	// Check whether controller is connected to service provider
	bool connected() { return m_connected;}

	// Set temporary socket
	void setTmpSock(SOCKET sock) { m_tmpSock = sock;}

	//! Get the map of connected service and its socket
	std::map<std::string, SOCKET> getSrvSocks() { return m_srvSocks; }

	// Set connecting condition of service provider
	void setConnected(bool connected) { m_connected = connected;}

	//! Get a server name
	const char * server() { return m_server.c_str(); }

	//! Get server port number
	int port() { return m_port; }

	//! Get an agent name to attach
	const char * myname() { return m_myname.c_str(); }

	//! Return whether controller is attached to an agent in the server
	bool isAttached() { return m_isAttached; } // added by sekikawa(2007/10/15)

	/** Attachs controller to the designated agent in the server
	 *
	 * @param server Simulation server name
	 * @param port   Port number of the simulation server
	 * @param myname Agent name
	 */
	bool attach(const char *server, int port, const char *myname);

	/**
	 * @brief Detachs controller from an agent
	 *
	 * Disconnect from server after attached from an agent
	 */
	void detach() { close_(); }

	/**
	 * @brief Gets a socket for communication of entity data
	 *
	 */
	SOCKET getDataSock() { return m_dataSock; }

	/**
	 * @brief Gets a socket for receiving from service provider
	 *
	 */
	SOCKET getSrvSock() { return m_srvSock; }

	// old
	void sendText(double t, const char *to, const char *text, double reachRadius);

	// old
	void sendText(double t, const char *to, const char *text);

	void sendDisplayText(double t, const char *to, const char *text, int fs, const char *color, double reachRadius = -1.0);

	void displayText(const char *text, int fs, const char *color, double dummy = -1.0);

	void sendMessage(const char *to, int argc, char **argv);

	/**
	 * @brief Sends message (onRecvMsg is called in the entity after receiving meassage)
	 *
	 * @param to         Destination agent name
	 * @param msg        Message
	 * @param distance   Distance of being able to recieve massege (Certainly message is reached when distance is not set)
	 */
	bool sendMsg(std::string to, std::string msg, double distance = -1.0);

	/**
	 * @brief Sends message (onRecvMsg is called in the entity after receiving meassage)
	 *
	 * @param to         Destination agent name
	 * @param msg        Message
	 * @param distance   Distance of being able to recieve massege (Message is certainly reached when distance is not set)
	 */
	bool sendMsg(std::vector<std::string> to, std::string msg, double distance = -1.0);

	void sendDisplayMessage(const char *to, int argc, char **argv, int fs, const char *color);

	/**
	 * @brief Send a message to all entities which have a connected controller
	 * 
	 * @param msg        Message
	 * @param distance   Distance of being able to recieve massege (Message is certainly reached when distance is not set)
	 */
	bool broadcastMsgToSrv(std::string msg);

	/**
	 * @brief Broadcasts a message
	 *
	 * @param msg        Message
	 * @param distance   Distance of being able to recieve massege (Message is sent to all users when distance is not set)
	 */
	bool broadcastMsgToCtl(std::string msg, double distance = -1.0);

	// old
	void broadcastMessage(int argc, char **argv);

	bool broadcastMsg(std::string msg, double distance = -1.0);

	// old
	void broadcastDisplayMessage(int argc, char **argv, int fs, const char *color);

	/**
	 * @brief Sends voice data to other agent
	 *
	 * @param t      Current time
	 * @param to     Destination of data (Send data to all agents when NULL is set)
	 * @param text   Voice data
	 */
	void sendSound(double t, const char *to, RawSound &sound);

	//! Send data
	bool sendData(SOCKET sock, const char *msg, int size);

	/**
	 * @brief Boadcasts a message
	 *
	 * @param msg        Message
	 * @param distance   Distance of being able to recieve massege (Message is certainly reached when -1.0 is set)
	 * @param to         Destination (-1: all / -2: service provider / -3: controller)
	 */
	bool broadcast(std::string msg, double distance, int to);

	// Delete the last-connected service
	void deleteLastService()
	{
		BaseService* srv = m_services.back();
		delete srv;
		m_services.pop_back();
	}

	// Delete designated service which is connected
	void deleteService(std::string sname)
	{
		std::vector<BaseService*>::iterator it;
		it = m_services.begin();
		while (it != m_services.end()) {

			std::string name = (*it)->getName();
			if (name == sname) {
				delete *it;
				m_services.erase(it);
				break;
			}
			it++;
		}

		// delete the service from map of a service name and a socket
		std::map<std::string, SOCKET>::iterator mit;
		mit = m_srvSocks.begin();
		while (mit != m_srvSocks.end()) {

			if ((*mit).first == sname) {
				m_srvSocks.erase(mit);
				return;
			}
			mit++;
		}
		LOG_ERR(("deleteService: cannot find %s", sname.c_str()));
	}

	//! Get the last-connected service
	BaseService* getLastService()
	{
		return m_services.back();
	}

	//! Gets designated service which is connected
	BaseService* getService(std::string sname)
	{
		std::vector<BaseService*>::iterator it;
		it = m_services.begin();

		while (it != m_services.end()) {

			std::string name = (*it)->getName();
			if (name == sname) {
				return (*it);
			}
			it++;
		}
		LOG_ERR(("getService: cannot find %s", sname.c_str()));
		return NULL;
	}
};

#endif // ControllerImpl_h



