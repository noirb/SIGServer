/*
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2014-01-08
 *    Delete major magic numbers by enum for new communication protocal
 *    Delete memory leak bugs
 * Modified by Yoshiaki Mizuchi on 2014-01-17
 *    Add English comment, modify indent
 *
 * TODO: Too many magic numbers are still remained
 */

#include "ControllerImpl.h"
#include "Controller.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"
#include "Logger.h"
#include "ct/CTSimObj.h"
#include "binary.h"
#include "Thread.h"
#include "comm/controller/RecvMessage.h"

#include <sys/types.h>
#include <errno.h>
#ifndef WIN32
#include <sys/time.h>
#include <sys/socket.h>
#endif
#include <time.h>
//#include <pthread.h>
#include <stdio.h>

#include "CommandImpl.h"

#ifdef WIN32
#ifdef SIGVERSE_OGRE_CLIENT
#include "SgvLog.h"
#endif	// SIGVERSE_OGRE_CLIENT

#ifdef IRWAS_OGRE_CLIENT
#include "IrcApp.h"
#endif	// IRWAS_OGRE_CLIENT
#endif	// WIN32

#ifdef WIN32


#define close closesocket

#ifdef SIGVERSE_OGRE_CLIENT
#define LOG(MSG) { Sgv::Log::println(MSG); }
#else
#ifdef IRWAS_OGRE_CLIENT
#define LOG(MSG) \
	{ IrcApp *app = getApp();\
	  if (app) { app->printLog(MSG);} \
	 }
#else
#define LOG(MSG)
#endif	// IRWAS_OGRE_CLIENT
#endif	// SIGVERSE_OGRE_CLIENT

#else
#define LOG(MSG)
#endif	// WIN32

#include <assert.h>

using namespace SocketUtil;

bool SocketUtil::sendData(SOCKET sock, const char* msg, int size)
{
	int sended = 0;

	// Deal with a case when all the data cannot be sent at once
	while (1) {
		// Send
		int r = send(sock, msg + sended, size - sended, 0);
#ifndef WIN32
		if (r < 0) {
		// Fail to send
			if ( errno == EAGAIN ||
				 errno == EWOULDBLOCK ) {

				usleep(100);

				continue;
			}
			LOG_ERR(("Failed to send data. erro[%d] [%s:%d]", r,  __FILE__, __LINE__ ));
			return false;
		}
#else
		if (r == SOCKET_ERROR) {
		// Fail to send
			int err = WSAGetLastError();
			if ( err == WSAEINTR ||
				 err == WSAEINPROGRESS ||
				 err == WSAEWOULDBLOCK ) {

				Sleep(1);

				continue;
			}
			LOG_ERR(("Failed to send data. erro[%d] [%s:%d]", r,  __FILE__, __LINE__ ));
			return false;
		}
#endif
		// Number of sent bytes
		sended += r;

		// Check whether all of the data is sent or not
		if (size == sended) break;
	}
	return true;
}


bool SocketUtil::recvData(SOCKET sock, char* msg, int ln)
{
	int recieved = 0;
	int r;

	// Deal with a case when all the data cannot be sent at once
	while (1) {


#ifndef WIN32
		// Receive
		r = recv(sock, msg + recieved, ln - recieved, 0);
		// Failed to Receive
		if (r < 0) {
			//LOG_SYS(("stop recieve data from Service. erro[%d]",r));
			return false;
		}
		// Number of received bytes
		recieved += r;
#else
		// Receive
		r = recv(sock, msg + recieved, ln - recieved, 0);
		if (r == SOCKET_ERROR) {
			fprintf(stderr, "====== ERROR in SocketUtil::recvData ======\n");
			int err = WSAGetLastError();
			if ( err == WSAEINTR ||
				 err == WSAEINPROGRESS ||
				 err == WSAEWOULDBLOCK ) {
				 Sleep(100);
				 continue;
			}
			//LOG_SYS(("stop recieve data from Service. erro[%d]",r));
			return false;
		}

		if (r > 0){
			// Number of received bytes
			recieved += r;
		}
#endif

		// Check whether all of the data is received or not
		if (ln == recieved) break;
	}
	
	return true;
}


std::string DoubleToString(double x)
{
	// Maximum data size is 32 bytes
	char tmp[32];
	sprintf(tmp,"%.3f",x);
	std::string str = std::string(tmp);
	str += ",";
	return str;
}

std::string IntToString(int x)
{
	char tmp[32];
	sprintf(tmp,"%d",x);
	std::string str = std::string(tmp);
	str += ",";
	return str;
}

void ControllerImpl::close_()
{
	delete m_ctSimObj;
	m_ctSimObj = NULL;

	if (m_cmdSock >= 0)
		{
			close(m_cmdSock);
			m_cmdSock = -1;
		}

	if (m_dataSock >= 0)
		{
			close(m_dataSock);
			m_dataSock = -1;
		}
}

#ifndef WIN32_ORG	// just for debug (substitute attach() and sendText() is at IrcViewController)

//! Start the loop of a service provider
bool BaseService::startServiceLoop(ControllerImpl *con)
{
	return false;
}

//! End the loop of a service provider
void BaseService::endServiceLoop()
{
	// Send message with an entity name
	std::string msg_tmp = m_entname + ",";
	int sendSize = msg_tmp.size() + sizeof(unsigned short) * 2;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0004); //TODO: Magic number
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg_tmp.c_str(), msg_tmp.size());

	// Request to finish the service
	if (!sendData(m_clientSock, sendBuff, sendSize)) {
		LOG_ERR(("endServiceLoop: cannot end service %s [%s, %s]" , m_name.c_str(),  __FILE__, __LINE__));
		delete [] sendBuff;
	}
	delete [] sendBuff;
}

bool BaseService::sendMsgToSrv(std::string msg)
{
	// Preparation of send data
	int msgSize = msg.size();
	std::string ssize = IntToString(msgSize);

	//LOG_MSG(("m_entname = %s", m_entname.c_str()));
	std::string ename = getEntityName();

	std::string sendMsg = ename + "," + ssize + msg;

	// Size of send data
	int tmpsize = sizeof(unsigned short) * 2;
	int sendSize = sendMsg.size() + tmpsize;

	// Buffer for sending message
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
	BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0001); //TODO: Magic number
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, sendMsg.c_str(), sendMsg.size());
	if (!sendData(m_clientSock, sendBuff, sendSize)) {
		LOG_ERR(("sendMsgToSrv: cannot send message [%s, %s].", __FILE__, __LINE__));
		delete [] sendBuff;
		return false;
	}

	delete [] sendBuff;
	return true;
}


ViewImage* ViewService::captureView(int camID, ColorBitType ctype, ImageDataSize size) 
{
	std::string camid = IntToString(camID);
	std::string sendMsg = m_entname + "," + camid;

	// Size of send data
	int tmpsize = sizeof(unsigned short) * 2;
	int sendSize = sendMsg.size() + tmpsize;

	// Buffer for sending message
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Header infomation
	BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0005); //TODO: Magic number
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	m_getData = false;
	memcpy(p, sendMsg.c_str(), sendMsg.size());
	if (!sendData(m_clientSock, sendBuff, sendSize)) {
		LOG_ERR(("captureView: cannot send request [%s, %d].", __FILE__, __LINE__));
		delete [] sendBuff;
		return NULL;
	}

	delete [] sendBuff;

	char tmp[4];
	p = tmp;

	if (!recvData(m_clientSock, tmp, 4)) {
		LOG_ERR(("captureView: cannot get capture data [%s, %d].", __FILE__, __LINE__));
		return false;
	}

	// Header information
	unsigned short head  = BINARY_GET_DATA_S_INCR(p, unsigned short); 
	int ssize = BINARY_GET_DATA_S_INCR(p, unsigned short);      

	if (head != 3)
		LOG_ERR(("captureView: cannot get capture data [%s, %d].", __FILE__, __LINE__));

	// Receive image data
	// Size is defined by the data type
	switch(ssize) {
	case 1: 
		{
			ssize = 230400; // 320*240*3; TODO: Magic number
			break;
		}
	}

	// Receive data
	char *recvBuff = new char[ssize];
	if (!recvData(m_clientSock, recvBuff, ssize)) {
		//LOG_ERR(("disconnected service [%s]", m_name.c_str()));
		delete [] recvBuff;
		return false;
	}

	ColorBitType cb;
	ImageDataSize ims;
	// Get image information from the size
	if (ssize == 230400) { //TODO: Magic number
		cb  = COLORBIT_24;
		ims = IMAGE_320X240; 
	}

	// Image information
	ViewImageInfo viewinfo(IMAGE_DATA_WINDOWS_BMP, cb, ims);

	// Create instance of the image class
	ViewImage* tmp_img = new ViewImage(viewinfo);

	tmp_img->setBuffer(recvBuff); //TODO: Danger: buffer created in this function is given to others
	return tmp_img;
	// CAUTION: Delete of the object should be done by users
}


bool  ViewService::detectEntities(std::vector<std::string> &v, int camID)
{
	std::string camid = IntToString(camID);
	std::string sendMsg = m_entname + "," + camid;

	// Size of send data
	int tmpsize = sizeof(unsigned short) * 2;
	int sendSize = sendMsg.size() + tmpsize;

	// Buffer for sending message
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Header information
	BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0007); //TODO: Magic number
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	memcpy(p, sendMsg.c_str(), sendMsg.size());
	if (!sendData(m_clientSock, sendBuff, sendSize)) {
		LOG_ERR(("detectEntities: cannot send request [%s, %d].", __FILE__, __LINE__));
		delete [] sendBuff;
		return false;
	}

	delete [] sendBuff;

	char tmp[4];
	p = tmp;

	if (!recvData(m_clientSock, tmp, 4)) {
		LOG_ERR(("detectEntities: cannot get entity data [%s, %d].", __FILE__, __LINE__));
		return false;
	}

	// Header information
	unsigned short head  = BINARY_GET_DATA_S_INCR(p, unsigned short); 
	unsigned short ssize = BINARY_GET_DATA_S_INCR(p, unsigned short);      
	ssize -= 4;

	// Header should be 8
	if (head != 8) { //TODO: Magic number
		LOG_ERR(("detectEntities: cannot get entity data [%s, %d].", __FILE__, __LINE__));
		return false;
	}

	char *recvBuff = new char[ssize];
	if (!recvData(m_clientSock, recvBuff, ssize)) {
		LOG_ERR(("detectEntities: cannot get entity data [%s, %d].", __FILE__, __LINE__));
		delete [] recvBuff;
		return false;
	}
	p = recvBuff;
	int entSize = atoi(strtok(p,","));

	// Receive found entity names
	for (int i = 0; i < entSize; i++) {
		std::string name = strtok(NULL,",");
		v.push_back(name);
	}
	delete [] recvBuff;
	return true;  
}


// TODO: return value of this API is strange, should be changed
// TODO: sensor value should be returned by variable, not as return value
unsigned char ViewService::distanceSensor(double start, double end, int camID, ColorBitType ctype) 
{
	// Send request to the viewer
	if (!sendDSRequest(0, start, end, camID, ctype))
		return 0;

	char tmp[4];
	char *p = tmp;

	if (!recvData(m_clientSock, tmp, 4)) {
		LOG_ERR(("distanceSensor: cannot get distance data [%s, %d].", __FILE__, __LINE__));
		return 0;
	}

	// Header infomation
	unsigned short head  = BINARY_GET_DATA_S_INCR(p, unsigned short); 
	int ssize            = BINARY_GET_DATA_S_INCR(p, unsigned short);      

	if (head != 3) //TODO: Magic number
		LOG_ERR(("distanceSensor: cannot get distance data [%s, %d].", __FILE__, __LINE__));

	// Receive image data
	// Size is defined by the received data type
	switch(ssize) {
	case 2: 
		{
			ssize = 1;
			break;
		}
	}
	// Receive data
	char *recvBuff = new char[ssize];
	if (!recvData(m_clientSock, recvBuff, ssize)) {
		delete [] recvBuff;
		return 0;
	} 
	// Receive distance data
	unsigned char distance = recvBuff[0];
	delete [] recvBuff;

	// Return the distance data
	return distance;
}


ViewImage* ViewService::distanceSensor1D(double start, double end, int camID, ColorBitType ctype, ImageDataSize size) 
{
	// Send request to the viewer
	if (!sendDSRequest(1, start, end, camID, ctype, size)) return NULL;

	char tmp[4];
	char *p = tmp;

	if (!recvData(m_clientSock, tmp, 4)) { //TODO: Magic number
		LOG_ERR(("distanceSensor1D: cannot get distance data [%s, %d].", __FILE__, __LINE__));
		return false;
	}

	// Header information
	unsigned short head  = BINARY_GET_DATA_S_INCR(p, unsigned short); 
	int ssize = BINARY_GET_DATA_S_INCR(p, unsigned short);      

	if (head != 3)
		LOG_ERR(("distanceSensor1D: cannot get distance data [%s, %d].", __FILE__, __LINE__));

	// Receive image data
	// Size is defined by the received data type
	switch(ssize) {
	case 3: 
		{
			ssize = 320;
			break;
		}
	}
	// Receive data
	char *recvBuff = new char[ssize];
	if (!recvData(m_clientSock, recvBuff, ssize)) {
		delete [] recvBuff;
		return NULL;
	} 

	ColorBitType cb;
	ImageDataSize ims;

	// Distance data size
	if (ssize == 320) {
		cb  = DEPTHBIT_8;
		ims = IMAGE_320X1; 
	}

	// Image information
	ViewImageInfo viewinfo(IMAGE_DATA_WINDOWS_BMP, cb, ims);

	// Create instance of the image class
	ViewImage* tmp_img = new ViewImage(viewinfo);

	tmp_img->setBuffer(recvBuff); //TODO: Danger: buffer created in this function is given to others
	return tmp_img;
}


ViewImage* ViewService::distanceSensor2D(double start, double end, int camID, ColorBitType ctype, ImageDataSize size) 
{
	// Send request to the viewer
	if (!sendDSRequest(2, start, end, camID, ctype, size)) //TODO: Magic number
		return NULL;

	char tmp[4];
	char *p = tmp;

	if (!recvData(m_clientSock, tmp, 4)) {
		LOG_ERR(("distanceSensor2D: cannot get distance data [%s, %d].", __FILE__, __LINE__));
		return false;
	}

	// Header information
	unsigned short head  = BINARY_GET_DATA_S_INCR(p, unsigned short); 
	int ssize = BINARY_GET_DATA_S_INCR(p, unsigned short);      

	if (head != 3) //TODO: Magic number
		LOG_ERR(("distanceSensor2D: cannot get distance data [%s, %d].", __FILE__, __LINE__));

	// Receive image data
	// Size is defined by the received data type
	switch(ssize) {
	case 4: 
		{
			ssize = 76800; // 320*240; TODO: Magic number
			break;
		}
	}

	// Recieve data
	char *recvBuff = new char[ssize];
	if (!recvData(m_clientSock, recvBuff, ssize)) {
		delete [] recvBuff;
		return NULL;
	} 

	ColorBitType cb;
	ImageDataSize ims;

	// Size of distance data
	if (ssize == 76800) { // TODO: Magic number
		cb  = DEPTHBIT_8;
		ims = IMAGE_320X240; 
	}

	// Image information
	ViewImageInfo viewinfo(IMAGE_DATA_WINDOWS_BMP, cb, ims);

	// Create instance of the image class
	ViewImage* tmp_img = new ViewImage(viewinfo);

	tmp_img->setBuffer(recvBuff); //TODO: Danger: buffer created in this function is given to others
	return tmp_img;
	// Delete of the object should be done by users
}


bool ViewService::sendDSRequest(int type, double start, double end, int camID, ColorBitType ctype, ImageDataSize size) 
{
	// Create message to send data
	std::string stype  = IntToString(type); 
	std::string camid  = IntToString(camID);
	std::string dstart = DoubleToString(start);
	std::string dend   = DoubleToString(end);
	std::string sendMsg = m_entname + "," + stype + camid + dstart + dend;

	// Size of send data
	int tmpsize = sizeof(unsigned short) * 2;
	int sendSize = sendMsg.size() + tmpsize;

	// Buffer for sending message
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Header information
	BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0006); //TODO: Magic number
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);      

	m_getData = false;
	memcpy(p, sendMsg.c_str(), sendMsg.size());
	if (!sendData(m_clientSock, sendBuff, sendSize)) {
		LOG_ERR(("distanceSensor: cannot send request [%s, %d].", __FILE__, __LINE__));
		delete [] sendBuff;
		return false;
	}

	delete [] sendBuff;
	return true;
}


THREAD_RET_VAL ControllerImpl::serviceThread(void *pParam)
{
	ControllerImpl *con = (ControllerImpl*)pParam;
	SOCKET sock = con->getSrvSock();
	struct sockaddr_in client;
#ifndef WIN32
	socklen_t len;
#else
	int len;
#endif
	// Client socket
	SOCKET s;

	listen(sock, 5);
	len = sizeof(client);
	while (1) {
		// Wait connection from the service
		s = accept(sock, (struct sockaddr *)&client, &len);

		// Receive first 5 bytes of the data, and get packet size
		char tmp[4]; 
		char *p = tmp;

		if (!recvData(s, tmp, 4)) {
			LOG_SYS(("failed to connect service."));
			goto error;
		}

		// Receive the data type
		unsigned short n = BINARY_GET_DATA_S_INCR(p, unsigned short);

		// Receive the data size
		int size = BINARY_GET_DATA_S_INCR(p, unsigned short);

		//////////////Data containing only header//////////////
		switch (n) {
			// Success in connection
		case 1:
			{
				// Set the client socket
				con->setConnected(true);
				con->setTmpSock(s);

				// continue;
				break;
			}

		case 4:
			{
				LOG_SYS(("failed to connect service"));
				// Failure or request of disconnection
				goto error;
			}
		}
	}

// Close socket
error:
	// Server socket
#ifndef WIN32
	if (sock != -1) { //Changed from NULL to -1 by inamura on 2014-02-28

		close(sock);
		sock = -1;  //Changed from NULL to -1 by inamura on 2014-02-28
	}
#else
	if (sock != INVALID_SOCKET) { // Changed by I.Hara.
		closesocket(sock);
		sock = INVALID_SOCKET;
	}
#endif
	/*
	// Client socket
	if(m_clientSock != NULL) {
		close(m_clientSock);
		m_clientSock = NULL;
	}
	*/
	return THREAD_RET_VAL_NULL;
}

#ifdef WIN32
int Pthread_Create(HANDLE *hT, void *attr, void (*func)(void *), void *arg)
{
  uintptr_t tp;
  tp = _beginthread(func, NULL, arg);
  if(tp == -1L){
     *hT = (HANDLE)NULL;
     return -1;
  }
  *hT = (HANDLE)tp;
  return 0;
}
#endif

BaseService* ControllerImpl::connectToService(std::string name)
{
	static bool first = false;
	SOCKET sock0;
	if (!first) {

		struct sockaddr_in addr;
		sock0 = socket(AF_INET, SOCK_STREAM, 0);
		if (sock0 < 0) {
			perror("socket");
			return NULL;
		}

		// Select port number at random (25000~30000)
		/*
		  struct timeval tv;
		  gettimeofday(&tv, NULL);
		  srand(tv.tv_usec);
		  int tmp = rand() % 5000;
		  unsigned short portNum = 25000 + tmp;
		*/

		// Port number of server + 1
		unsigned short portNum = m_port + 1;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(portNum);
		addr.sin_addr.s_addr = INADDR_ANY;

		int count = 0;
		// Repeat until free port is found
		while (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
			// Break when free port could not be found 10 times
			if (count == 10) {
				LOG_ERR(("Controller could not get port for service [%s, %s].", __FILE__, __LINE__));
				return NULL;
			}
			// Try next port
			//int tmp = rand() % 5000;
			//portNum = 25000 + tmp;
			portNum += 1;
    
			addr.sin_port = htons(portNum);
			count++;
		}
		m_srvSock = sock0;
		m_srvPort = portNum;
	}

	//BaseService *service = new BaseService(name, m_srvPort, m_srvSock);
	ViewService *service = new ViewService(name, m_srvPort, m_srvSock);

	// Send own name and a destination service name
	const char* tmpname = myname();
	std::string myname = std::string(tmpname);
	std::string sendMsg = name + "," + myname + ",";

	// Set own (entity) name
	service->setEntityName(myname);

	// Push service information as a menber variable
	m_services.push_back(service);

	// Size of send message (including header)
	int sendSize = sendMsg.size() + sizeof(unsigned short) * 3;

	// Prepare send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Add the size of header and data
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_CONNECT_SERVICE);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);      

	// Add port number
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_srvPort);

	// Add service name
	memcpy(p, sendMsg.c_str(), sendMsg.size());

	//////////////////////////////////////////////
	///Create a new thread for receiving data before sending request
	//////////////////////////////////////////////
	if (!first) {
#ifndef WIN32
		pthread_t tid1;
		pthread_mutex_t mutex;

		pthread_mutex_init(&mutex, NULL);
		pthread_create(&tid1, NULL, ControllerImpl::serviceThread, reinterpret_cast<void*>(this));

#else

		HANDLE tid1;
		HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
		Pthread_Create(&tid1, NULL, ControllerImpl::serviceThread, reinterpret_cast<void*>(this));

#endif
		first = true;
	}

	//////////////////////////////////////////////
	// Send data
	if (!sendData(m_dataSock, sendBuff, sendSize)) {
		LOG_ERR(("sendMsg: Failed to send message"));
		delete [] sendBuff;
		return NULL;
	}
	delete [] sendBuff;

	int count = 0;
	// Wait until connection completion
	while (!m_connected) {
#ifndef WIN32
		usleep(1000); //TODO: Magic number
#else
		Sleep(1);
#endif
		count++;
		// Time out when the socket is closed or the count is over 1.5 sec.
		if (count > 2000 || service->getSocket() < 0) {
			goto error;
		}
	}

	m_connected = false;
	m_srvSocks.insert(std::map<std::string, SOCKET>::value_type(name, m_tmpSock));

	// Set client socket
	service->setClientSocket(m_tmpSock);

	LOG_SYS(("connected to \"%s\" port[%d]",name.c_str(), m_srvPort));

	return service;

 error:
	close(sock0);
	sock0 = -1;    // Changed from NULL to -1 by inamura
	first = false;
	LOG_ERR(("failed to connect \"%s\"",name.c_str()));
	deleteLastService();
	return NULL;
}

//!Port number is set by a user
BaseService* ControllerImpl::connectToService(std::string name, unsigned short port)
{
	static bool first = false;
	SOCKET sock0;
	if (!first) {
		struct sockaddr_in addr;
    
		sock0 = socket(AF_INET, SOCK_STREAM, 0);
		if (sock0 < 0) {
			perror("socket");
			return NULL;
		}
    
		unsigned short portNum = port;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(portNum);
		addr.sin_addr.s_addr = INADDR_ANY;

		// When the port cannot be used
		if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {

			LOG_ERR(("Controller could not get port for service [%s, %s].", __FILE__, __LINE__));
			return NULL;
		}
		m_srvSock = sock0;
		m_srvPort = portNum;
	}
	//BaseService *service = new BaseService(name, m_srvPort, m_srvSock);
	ViewService *service = new ViewService(name, m_srvPort, m_srvSock);

	// Send own name and destination service name
	const char* tmpname = myname();
	std::string myname = std::string(tmpname);
	std::string sendMsg = name + "," + myname + ",";

	// Set own (entity) name
	service->setEntityName(myname);

	// Push service information as a menber variable
	m_services.push_back(service);


	// Size of send message (including header)
	int sendSize = sendMsg.size() + sizeof(unsigned short) * 3;

	// Prepare send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Add size of header and data
	// Data type of connectService is 9
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_CONNECT_SERVICE); 
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	// Add port number
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_srvPort);

	// Add service name
	memcpy(p, sendMsg.c_str(), sendMsg.size());

	//////////////////////////////////////////////
	///Create a new thread for receiving data before sending request
	//////////////////////////////////////////////
	if (!first) {
#ifndef WIN32
		pthread_t tid1;
		pthread_mutex_t mutex;

		pthread_mutex_init(&mutex, NULL);
		pthread_create(&tid1, NULL, ControllerImpl::serviceThread, reinterpret_cast<void*>(this));
#else

		HANDLE tid1;
		HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
		Pthread_Create(&tid1, NULL, ControllerImpl::serviceThread, reinterpret_cast<void*>(this));


#endif
		first = true;
	}
	//////////////////////////////////////////////

	// Send data
	if (!sendData(m_dataSock, sendBuff, sendSize)) {
		LOG_ERR(("sendMsg: Failed to send message"));
		delete [] sendBuff;
		return NULL;
	}
	delete [] sendBuff;

	int count = 0;
	// Wait until connection completion
	while (!m_connected) {
#ifndef WIN32
		usleep(1000);
#else
		Sleep(1);
#endif
		count++;
		// Time out when the socket is closed or the count is over 1 sec.
		if (count > 1000 || service->getSocket() == -1) { //Changed from NULL to -1 by inamura
			goto error;
		}
	}
	m_connected = false;
	m_srvSocks.insert(std::map<std::string, SOCKET>::value_type(name, m_tmpSock));
	LOG_SYS(("connected to \"%s\" port[%d]",name.c_str(), m_srvPort));

	// Set client socket
	service->setClientSocket(m_tmpSock);

	return service;

 error:
	close(sock0);
	sock0 = -1;  // Changed from NULL to -1 by inamura
	LOG_ERR(("failed to connect \"%s\"",name.c_str()));
	deleteLastService();
	return NULL;
}

void ControllerImpl::disconnectToService(std::string name)
{
	BaseService *srv =  getService(name);
	if (srv != NULL) {
		// End service loop
		srv->endServiceLoop();
	}
}


bool ControllerImpl::checkService(std::string name)
{
	std::string msg = name + ",";
	int sendSize = msg.size() + sizeof(unsigned short) *2;
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Add size of header and data
	// Data type of connectService is 9
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_CHECK_SERVICE);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);      

	// Add service name
	memcpy(p, msg.c_str(), msg.size());

	//////////////////////////////////////////////
	// Send data
	if (!sendData(m_dataSock, sendBuff, sendSize)) {
		LOG_ERR(("checkService: failed to send request request",  __FILE__, __LINE__ ));
		delete [] sendBuff;
		return NULL;
	}

	delete [] sendBuff;

	char recvBuff[4];
  
	if (recv(m_dataSock, recvBuff, 4, 0) != 4) {
		LOG_ERR(("checkService: failed to recieve result",  __FILE__, __LINE__ ));
		return false;
	}
	p = recvBuff;
	unsigned short result  = BINARY_GET_DATA_S_INCR(p, unsigned short); 

	// bool result = true;
	return (result != FALSE);
}


bool ControllerImpl::attach(char const *server, int port, char const *myname)
{
	assert(m_port < 0);

	{
		SOCKET s = CommUtil::connectServer(server, port);
		if (s < 0 || s == INVALID_SOCKET) { return false; }

		CommData::AttachControllerRequest enc(myname);
		enc.send(s);

		m_cmdSock = s;

		LOG("*** CommData::AttachControllerRequest ... send ok ***");
	}

	{
		SOCKET s = CommUtil::connectServer(server, port);
		if (s < 0 || s == INVALID_SOCKET) {
			goto err;
		}

		CommData::ConnectDataPortRequest enc(myname);
		enc.send(s);

		m_dataSock = s;

		LOG("*** CommData::ConnectDataPortRequest ... send ok ***");
	}

	m_server = server;
	m_port = port;
	m_myname = myname;

	m_isAttached = true;

	return true;

 err:
	close_();
	return false;
}


#ifdef DEPRECATED
void ControllerImpl::moveTo(double x, double z, double velocity)
{
	MoveCommand cmd(myname(), Vector3d(x, 0, z), velocity);
	send(cmd);
}

void ControllerImpl::send(Command &cmd)
{
	ControllerCommandEncoder enc(cmd);
	enc.send(m_dataSock);
}
#endif


CTSimObj & ControllerImpl::getCTSimObj()
{
	if (!m_ctSimObj) {
		m_ctSimObj = new CTSimObj(m_dataSock, myname());
	}
	return *m_ctSimObj;
}


void ControllerImpl::sendText(double t, const char *to, const char *text)
{
	CTSimObj &obj = getCTSimObj();
	obj.sendText(t, to, text);
}


void ControllerImpl::sendText(double t, const char *to, const char *text, double reachRadius)
{
	CTSimObj &obj = getCTSimObj();
	obj.sendText(t, to, text, reachRadius);

	LOG("*** CommData::InvokeOnRecvText ... send ok ***");
}

//added by okamoto@tome (2011/8/3)
void ControllerImpl::sendDisplayText(double t, const char *to, const char *text, int fs, const char *color, double reachRadius)
{
	CTSimObj &obj = getCTSimObj();
	if (reachRadius == -1.0)  // TODO: this description is danger
		obj.sendText(t, to, text);
	else
		obj.sendText(t, to, text, reachRadius);

	// Send whole data to the server
	if (fs <= 0) fs = 64;
	//	if (color == NULL || color <= 0 ) color = 1;
	int col;
	if      (strcmp(color,"red"   )==0) col = 1;
	else if (strcmp(color,"green" )==0) col = 2;
	else if (strcmp(color,"blue"  )==0) col = 3;
	else if (strcmp(color,"purple")==0) col = 4;
	else if (strcmp(color,"yellow")==0) col = 5;
	else if (strcmp(color,"brown" )==0) col = 6;
	else if (strcmp(color,"gray"  )==0) col = 7;
	else                                col = 0;
	CommDisplayTextEncoder enc(fs, col, text);
	enc.send(m_cmdSock);

}



//added by noma@tome2012/03/09
void ControllerImpl::displayText(const char *text, int fs, const char *color, double dummy)
{
	//	CTSimObj &obj = getCTSimObj();
	//	if (reachRadius == -1.0)	obj.sendText(t, to, text);
	//	else  obj.sendText(t, to, text, reachRadius);

	// Send data to the server once
	if (fs <= 0) fs = 64;
	//	if (color == NULL || color <= 0 ) color = 1;
	int col;
	if      (strcmp(color,"red"   )==0) col = 1;
	else if (strcmp(color,"green" )==0) col = 2;
	else if (strcmp(color,"blue"  )==0) col = 3;
	else if (strcmp(color,"purple")==0) col = 4;
	else if (strcmp(color,"yellow")==0) col = 5;
	else if (strcmp(color,"brown" )==0) col = 6;
	else if (strcmp(color,"gray"  )==0) col = 7;
	else                                col = 0;
	CommDisplayTextEncoder enc( fs, col, text);
	enc.send(m_cmdSock);

}


void ControllerImpl::sendMessage(const char *to, int argc, char **argv)
{
	CTSimObj obj(m_dataSock, myname());
	obj.sendMessage(to, argc, argv);
}


bool ControllerImpl::sendMsg(std::string to, std::string msg, double distance)
{
	// Get the length of message data
	char bsize[5];
	int msgSize = (int)msg.size();
	sprintf(bsize, "%.5d", msgSize);
	std::string msg_size = std::string(bsize);

	std::string dist;
	if (distance > 0) 
		dist = DoubleToString(distance);
	else
		dist = "-1.0,";

	// Add message length, message, send distance of message, number of destination entities, and names
	std::string sendMsg = msg_size + "," + msg + "," + dist + "1," + to + ",";

	// Size of send message (including header)
	int sendSize = sendMsg.size() + sizeof(unsigned short) *2;

	// Prepare send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Add the size of header and data
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SENDMSG_FROM_CONTROLLER);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	// Add message
	memcpy(p, sendMsg.c_str(), sendMsg.size());

	// Send data
	if (!sendData(m_dataSock, sendBuff, sendSize)) {
		LOG_ERR(("sendMsg: Failed to send message"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
	return true;
}


bool ControllerImpl::sendMsg(std::vector<std::string> to, std::string msg, double distance)
{
	// Get the length of message data
	char bsize[5];
	int msgSize = (int)msg.size();
	sprintf(bsize, "%.5d", msgSize);
	std::string msg_size = std::string(bsize);

	std::string dist;
	if (distance > 0) 
		dist = DoubleToString(distance);
	else
		dist = "-1.0,";

	int size = to.size();
	char tmp[5];
	sprintf(tmp, "%d", size);
	std::string num = std::string(tmp);

	// Add message and number of destination entities
	std::string sendMsg = msg_size + "," + msg + "," + dist + num + "," ;

	// Add names of destination entities
	for (int i = 0; i < size; i++) {
		sendMsg += to[i] + ",";
	}

	// Size of send message (including header)
	int sendSize = sendMsg.size() + sizeof(unsigned short) *2;

	// Prepare send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Add the size of header and data
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SENDMSG_FROM_CONTROLLER);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	// Add message
	memcpy(p, sendMsg.c_str(), sendMsg.size());

	// Send data
	if (!sendData(m_dataSock, sendBuff, sendSize)) {
		LOG_ERR(("sendMsg: Failed to send message"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
	return true;
}


void ControllerImpl::sendDisplayMessage(const char *to, int argc, char **argv, int fs, const char *color)
{
	CTSimObj obj(m_dataSock, myname());
	obj.sendMessage(to, argc, argv);

	// Send data to the server once
	if (fs <= 0) fs = 64;
	//	if (color == NULL || color <= 0 ) color = 1;
	int col;
	if      (strcmp(color,"red")   ==0) col = 1;
	else if (strcmp(color,"green") ==0) col = 2;
	else if (strcmp(color,"blue")  ==0) col = 3;
	else if (strcmp(color,"purple")==0) col = 4;
	else if (strcmp(color,"yellow")==0) col = 5;
	else if (strcmp(color,"brown") ==0) col = 6;
	else if (strcmp(color,"gray")  ==0) col = 7;
	else                                col = 0;

	// Show the first string
	const char *text = argv[0];
	CommDisplayTextEncoder enc(fs, col, text);
	enc.send(m_cmdSock);

}

void ControllerImpl::broadcastMessage(int argc, char **argv)
{
	CTSimObj obj(m_dataSock, myname());
	obj.broadcastMessage(argc, argv);
}

bool ControllerImpl::broadcastMsg(std::string msg, double distance)
{
	if (!broadcast(msg, distance, -1)) {
		return false;
	}
	return true;
}


bool ControllerImpl::broadcastMsgToSrv(std::string msg)
{
	if (!broadcast(msg, -1.0, -2)) {
		return false;
	}
	return true;
}

bool ControllerImpl::broadcastMsgToCtl(std::string msg, double distance)
{
	if (!broadcast(msg, distance, -3)) {
		return false;
	}
	return true;
}

void ControllerImpl::broadcastDisplayMessage(int argc, char **argv, int fs, const char *color)
{
	CTSimObj obj(m_dataSock, myname());
	obj.broadcastMessage(argc, argv);

	// Send data to the server once
	if (fs <= 0) fs = 64;
	//	if (color == NULL || color <= 0 ) color = 1;
	int col;
	if      (strcmp(color,"red")   ==0) col = 1;
	else if (strcmp(color,"green") ==0) col = 2;
	else if (strcmp(color,"blue")  ==0) col = 3;
	else if (strcmp(color,"purple")==0) col = 4;
	else if (strcmp(color,"yellow")==0) col = 5;
	else if (strcmp(color,"brown") ==0) col = 6;
	else if (strcmp(color,"gray")  ==0) col = 7;
	else                                col = 0;
	const char *text = argv[0];
	CommDisplayTextEncoder enc(fs, col, text);
	enc.send(m_cmdSock);
}


bool ControllerImpl::sendData(SOCKET sock, const char* msg, int size)
{
	int sended = 0;
	// For when cannot send all data at once
	while (1) {

		// Sending
#ifndef WIN32
		int r = send(sock, msg + sended, size - sended, 0);

		// Sending failed
		if (r < 0) {
			if ( errno == EAGAIN ||
				 errno == EWOULDBLOCK ) {

				sleep(0.01);

				continue;
			}
			LOG_ERR(("Failed to send data. erro[%d]",errno));
			return false;
		}

#else
		int r = send(sock, msg + sended, size - sended, 0);

		// Sending failed
		if (r == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if ( err == WSAEINTR ||
				 err == WSAEINPROGRESS ||
				 err == WSAEWOULDBLOCK ) {

				Sleep(1);

				continue;
			}
			LOG_ERR(("Failed to send data. erro[%d]",errno));
			return false;
		}
#endif
		// Number of already sent bytes
		sended += r;

		// Check all data was sent
		if (size == sended) break;
	}
	return true;
}


void ControllerImpl::sendSound(double t, const char *to, RawSound &sound)
{
	CTSimObj &obj = getCTSimObj();
	obj.sendSound(t, to, sound);
}


bool ControllerImpl::broadcast(std::string msg, double distance, int to)
{
	// Refer the length of message data
	char bsize[5];
	int msgSize = (int)msg.size();
	sprintf(bsize, "%.5d", msgSize);
	std::string msg_size = std::string(bsize);

	std::string dist;
	if (distance > 0) 
		dist = DoubleToString(distance);
	else
		dist = "-1.0,";

	int size = to;
	char tmp[5];
	sprintf(tmp, "%d", size);
	std::string num = std::string(tmp);

	// Add message and the number of destination entities
	std::string sendMsg = msg_size + "," + msg + "," + dist + num + "," ;

	// Size of send message (including header)
	int sendSize = sendMsg.size() + sizeof(unsigned short) *2;

	// Prepare send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Add the size of header and send data
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SENDMSG_FROM_CONTROLLER);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	// Add message
	memcpy(p, sendMsg.c_str(), sendMsg.size());

	// Send message
	if (!sendData(m_dataSock, sendBuff, sendSize)) {
		LOG_ERR(("sendMsg: Failed to send message"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
	return true;
}

#endif	// WIN32


