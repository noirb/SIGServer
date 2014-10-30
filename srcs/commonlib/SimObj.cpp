/*
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change get/setVelocity to get/setLinearVelocity
 *    change JointVelocity  to AngularVelocityToJoint
 *    change ObjectVelocity to AngularVelocityToParts
 *    (English comments is not checked)
 * Modified by Tetsunari Inamura on 2014-01-03
 *    fixed a bug of memory leak at getJointAngle, getCamPos, getCamDir, getPosition, getRotation, getJointPosition, getPartsPosition, getPointingVector
 * Modified by Tetsunari Inamura on 2014-01-08
 *    Delete major magic numbers
 */

#include "SimObj.h"
#include "Parts.h"
#include "CommDataEncoder.h"
#include "JointForce.h"
#include "Logger.h"
#include "CommDataResult.h"
#include "Controller.h"
#include "ControllerImpl.h"
#include "RecvEvent.h"
#include "binary.h"

#ifdef WIN32

#ifdef SIGVERSE_OGRE_CLIENT
#include "PartsCmpnt.h"
#endif	// SIGVERSE_OGRE_CLIENT

#ifdef IRWAS_OGRE_CLIENT
#include "PartsImpl.h"
#include "IrcApp.h"
#include "IrcWorld.h"
#endif	// IRWAS_OGRE_CLIENT

#endif	// WIN32

//DEBUG
#include <iostream>
//DEBUG


// TODO: dynamicsMode is trange naming, is it wheel mode?  : by inamura on 2013-12-31
SimObj::SimObj() : Super(), m_sender(NULL)
{
	dynamicsMode = MODE_NOT_USE_WHEEL;
}

SimObj::~SimObj()
{
	free_();
}

// added by kawamoto@tome (2011/04/15)
#ifndef WIN32
void SimObj::setRequestSener(RequestSender *s)
{
	m_sender = s;
}
#endif


void SimObj::free_()
{
	for (PartsM::iterator i=m_parts.begin(); i!=m_parts.end(); i++) {
		CParts *p = i->second;
		delete p;
	}
	// fix(sekikawa)(2007/11/30)
	m_parts.clear();
	//m_attrs.clear();
}


CParts *SimObj::getParts(const char *pname)
{
	if (m_parts.find(pname) == m_parts.end())
	{
		return NULL;
	}
	else
	{
		Controller *con = (Controller*)m_sender;
		ControllerImpl *conim = (ControllerImpl*)con;

		SOCKET sock = conim->getDataSock();
		m_parts[pname]->setSocket(sock);
		//const char *owner = name();
		m_parts[pname]->setOwner(name());
		return m_parts[pname];
	}
}


void SimObj::copy(const SimObj &o)
{
	SimObjBase::copy(o);
	
	for (PartsM::const_iterator i=o.m_parts.begin(); i!=o.m_parts.end(); i++) {
		CParts *p = i->second;
		CParts *newp = p->clone();

		this->push(newp);
		
	}
}



// added by sekikaw(2007/11/30)
std::map<std::string, CParts *>& SimObj::getPartsCollection()
{
	return m_parts;
}



void SimObj::push(CParts *p)
{
	// begin(add)(sekikawa)
	// [BUGFIX] Avoid memory leak. (delete parts if already exists)
	if (m_parts[p->name()])
	{
		delete m_parts[p->name()];
	}
	// end(add)

	m_parts[p->name()] = p;
}

int SimObj::setBinary(char *data, int size)
{
	char *p = data;
	DataLengthType datalen = BINARY_GET_DATA_S_INCR(p, DataLengthType);
	if (size < datalen) { return -1; }

	m_id = BINARY_GET_DATA_L_INCR(p, Id);
	BINARY_SKIP_STRING(p); // name
	BINARY_SKIP_STRING(p); // class
	short attached  = BINARY_GET_DATA_S_INCR(p, short);
	m_attached = (bool)(attached);
	m_ops = BINARY_GET_DATA_L_INCR(p, Operation);

	DataOffsetType offset;
	// attr offset value
	offset = BINARY_GET_DATA_S_INCR(p, DataOffsetType);
	char *attr_head = data + offset;
	// body offset value
	offset = BINARY_GET_DATA_S_INCR(p, DataOffsetType);
	char *body_head = data + offset;

	p = attr_head;
	DataLengthType attrslen = BINARY_GET_DATA_S_INCR(p, DataLengthType);

	while (1) {
		int left = attrslen - (p-attr_head);
		if (left <= 0) { break; }
		
		Attribute *attr = new Attribute();
		int n = attr->setBinary(p, left);
		if (n < 0) { return -1; }
		p += n;
		push(attr);
	}

	p = body_head;
	DataLengthType bodylen = BINARY_GET_DATA_S_INCR(p, DataLengthType);
	while (1) {
		int left = bodylen - (p-body_head);
		if (left <= 0) { break; }

		char *h = p;
		DataLengthType len = BINARY_GET_DATA_S_INCR(h, DataLengthType);
#ifdef SIGVERSE_OGRE_CLIENT
		CParts *parts = CParts::decode(h, m_id);
#else
#ifdef IRWAS_OGRE_CLIENT
		CParts *parts = CParts::decode(h, m_id);
#else
		CParts *parts = CParts::decode(h);
#endif	// IRWAS_OGRE_CLIENT
#endif	// SIGVERSE_OGRE_CLIENT
		if (!parts) { return -1; }
		p += len;
		push(parts);
	}

	//	dump();
	return datalen;
}

std::string SimObj::getCameraLinkName(int camID)
{
	if (m_parts.size() == 1) {
		if (camID == 1) {
			return "body";
		}
		else {
			LOG_ERR(("getCameraLinkName : cannot get Camera ID %d",camID));
			return NULL;
		}
	}
	else {
		if      (camID == 1) {return elnk1();}
		else if (camID == 2) {return elnk2();}
		/*
		else if (camID == 3) {return elnk3();}
		else if (camID == 4) {return elnk4();}
		else if (camID == 5) {return elnk5();}
		else if (camID == 6) {return elnk6();}
		else if (camID == 7) {return elnk7();}
		else if (camID == 8) {return elnk8();}
		else if (camID == 9) {return elnk9();}
		*/
		else {
			LOG_ERR(("getCameraLinkName : cannot get Camera ID %d",camID));
			return NULL;
		}
	}
}


// fixed a memory leak bug by Tetsunari Inamura on 2014-01-03
bool SimObj::getCamPos(Vector3d &pos, int camID, bool requestToServer)
{
	if (!requestToServer) {
    
		char tmpx[6];
		char tmpy[6];
		char tmpz[6];
    
		sprintf(tmpx,"epx%d",camID);
		sprintf(tmpy,"epy%d",camID);
		sprintf(tmpz,"epz%d",camID);
    
		if (!isAttr(tmpx) || !isAttr(tmpy) || !isAttr(tmpz)) {
			LOG_ERR(("getCamPos: Cannot find camera id [%d]", camID));
			return false;
		}
		else {
			double x = getAttr(tmpx).value().getDouble();
			double y = getAttr(tmpy).value().getDouble();
			double z = getAttr(tmpz).value().getDouble();
			pos.set(x, y, z);
		}
		return true;
	}
  
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 3;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_CAMERA_POSITION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DATA_S_INCR(p, unsigned short, camID);

	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getCamPos: cannot send request"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	int recvSize = sizeof(double) * 3 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getCamPos: failed to recieve position data"));
		delete [] recvBuff;
		return false;
	}
  
	p = recvBuff;
	bool success = BINARY_GET_BOOL_INCR(p);
	if (success) {
		double x = BINARY_GET_DOUBLE_INCR(p);
		double y = BINARY_GET_DOUBLE_INCR(p);
		double z = BINARY_GET_DOUBLE_INCR(p);
    
		pos.set(x, y, z);
	}
	delete [] recvBuff; // added by Tetsunari Inamura on 2014-01-03
	return true;
}


bool SimObj::setCamPos(Vector3d pos, int camID)
{
	char tmpx[6];
	char tmpy[6];
	char tmpz[6];

	sprintf(tmpx,"epx%d",camID);
	sprintf(tmpy,"epy%d",camID);
	sprintf(tmpz,"epz%d",camID);

	if (!isAttr(tmpx) || !isAttr(tmpy) || !isAttr(tmpz)) {
		LOG_ERR(("setCamPos: Cannot find camera id [%d]", camID));
		return false;
	}
	else {
		getAttr(tmpx).value().setDouble(pos.x());
		getAttr(tmpy).value().setDouble(pos.y());
		getAttr(tmpz).value().setDouble(pos.z());
	}

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();
  
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 3 + sizeof(double) * 3;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_CAMERA_POSITION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DATA_S_INCR(p, unsigned short, camID);

	BINARY_SET_DOUBLE_INCR(p, pos.x());
	BINARY_SET_DOUBLE_INCR(p, pos.y());
	BINARY_SET_DOUBLE_INCR(p, pos.z());

	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setCamPos: cannot send position data"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	return true;
}


// fixed a memory leak bug by Tetsunari Inamura on 2014-01-03
bool SimObj::getCamDir(Vector3d &vec,int camID, bool requestToServer)
{
	if (!requestToServer) {
		char tmpx[6];
		char tmpy[6];
		char tmpz[6];
    
		sprintf(tmpx,"evx%d",camID);
		sprintf(tmpy,"evy%d",camID);
		sprintf(tmpz,"evz%d",camID);

		if (!isAttr(tmpx) || !isAttr(tmpy) || !isAttr(tmpz)) {
			LOG_ERR(("getCamDir: Cannot find camera id [%d]", camID));
			return false;
		}
		else {
			double x = getAttr(tmpx).value().getDouble();
			double y = getAttr(tmpy).value().getDouble();
			double z = getAttr(tmpz).value().getDouble();
			vec.set(x, y, z);
		}
		return true;
	}

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 3;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_CAMERA_DIRECTION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DATA_S_INCR(p, unsigned short, camID);

	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getCamDir: cannot send request"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	int recvSize = sizeof(double) * 3 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getCamDir: failed to recieve position data"));
		delete [] recvBuff;
		return false;
	}
  
	p = recvBuff;
	bool success = BINARY_GET_BOOL_INCR(p);
	if (success) {
		double x = BINARY_GET_DOUBLE_INCR(p);
		double y = BINARY_GET_DOUBLE_INCR(p);
		double z = BINARY_GET_DOUBLE_INCR(p);
    
		vec.set(x, y, z);
	}
	delete [] recvBuff; // added by Tetsunari Inamura on 2014-01-03
	return true;
}


bool SimObj::getCamQuaternion(double &qw, double &qx, double &qy, double &qz, int camID)
{
	char tmpw[6];
	char tmpx[6];
	char tmpy[6];
	char tmpz[6];
    
	sprintf(tmpw,"quw%d",camID);
	sprintf(tmpx,"qux%d",camID);
	sprintf(tmpy,"quy%d",camID);
	sprintf(tmpz,"quz%d",camID);
    
	if (!isAttr(tmpw) || !isAttr(tmpx) || !isAttr(tmpy) || !isAttr(tmpz)) {
		LOG_ERR(("getCamQuaternion: Cannot find camera id [%d]", camID));
		return false;
	}
	else {
		qw = getAttr(tmpw).value().getDouble();
		qx = getAttr(tmpx).value().getDouble();
		qy = getAttr(tmpy).value().getDouble();
		qz = getAttr(tmpz).value().getDouble();
	}
	return true;
}

bool SimObj::setCamDir(Vector3d vec,int camID)
{
	char tmpx[6];
	char tmpy[6];
	char tmpz[6];

	sprintf(tmpx,"evx%d",camID);
	sprintf(tmpy,"evy%d",camID);
	sprintf(tmpz,"evz%d",camID);

	if (!isAttr(tmpx) || !isAttr(tmpy) || !isAttr(tmpz)) {
		LOG_ERR(("setCamDir: Cannot find camera id [%d]", camID));
		return false;
	}
	else {
		getAttr(tmpx).value().setDouble(vec.x());
		getAttr(tmpy).value().setDouble(vec.y());
		getAttr(tmpz).value().setDouble(vec.z());
	}

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();
  
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 3 + sizeof(double) * 3;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_CAMERA_DIRECTION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DATA_S_INCR(p, unsigned short, camID);

	BINARY_SET_DOUBLE_INCR(p, vec.x());
	BINARY_SET_DOUBLE_INCR(p, vec.y());
	BINARY_SET_DOUBLE_INCR(p, vec.z());

	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setCamDir: cannot send direction data"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
}


double SimObj::getCamFOV(int camID)
{
	char tmpx[6];

	sprintf(tmpx,"FOV%d",camID);

	if (!isAttr(tmpx)) {
		LOG_ERR(("getCamFOV: Cannot find camera id [%d]", camID));
		return 0.0;
	}

	double fov = getAttr(tmpx).value().getDouble();

	return fov;
}


bool SimObj::setCamFOV(double fov, int camID)
{
	char tmpx[6];
  
	sprintf(tmpx,"FOV%d",camID);
  
	if (!isAttr(tmpx)) {
		LOG_ERR(("setCamFOV: Cannot find camera id [%d]", camID));
		return false;
	}
  
	getAttr(tmpx).value().setDouble(fov);
  
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();
  
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 3 + sizeof(double);

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_CAMERA_FOV);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DATA_S_INCR(p, unsigned short, camID);

	BINARY_SET_DOUBLE_INCR(p, fov);
	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setCamFOV: cannot send fov data"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
  
	return true;
}


double SimObj::getCamAS(int camID)
{
	char tmpx[14];

	sprintf(tmpx,"aspectRatio%d",camID);

	if (!isAttr(tmpx)) {
		LOG_ERR(("getCamAS: Cannot find camera id [%d]", camID));
		return 0.0;
	}

	double as = getAttr(tmpx).value().getDouble();
	return as;
}


bool SimObj::setCamAS(double as, int camID)
{
	char tmpx[14];

	sprintf(tmpx,"aspectRatio%d",camID);

	if (!isAttr(tmpx)) {
		LOG_ERR(("setCamAS: Cannot find camera id [%d]", camID));
		return false;
	}

	getAttr(tmpx).value().setDouble(as);

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();
  
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 3 + sizeof(double);

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_CAMERA_ASPECTRATIO);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DATA_S_INCR(p, unsigned short, camID);

	BINARY_SET_DOUBLE_INCR(p, as);
	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setCamAS: cannot send fov data."));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
  
	return true;  
}


std::string SimObj::getCamLink(int camID)
{
	char tmpx[7];

	sprintf(tmpx,"elnk%d",camID);

	if (!isAttr(tmpx)) {
		LOG_ERR(("getCamLink: Cannot find camera ID [%d]", camID));
	}

	std::string link = getAttr(tmpx).value().getString();
	return link;
}


void SimObj::setCamLink(std::string link, int camID)
{
	char tmpx[7];
	sprintf(tmpx,"elnk%d",camID);

	if (!isAttr(tmpx)) {
		LOG_ERR(("setCamLink: Cannot set link for camera ID [%d]", camID));
	}

	getAttr(tmpx).value().setString(link.c_str());
}


#ifdef CONTROLLER

#include "CommandImpl.h"


// fixed a memory leak bug by Tetsunari Inamura on 2014-01-03
Vector3d & SimObj::getPosition(Vector3d &v)
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 2;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_ENTITY_POSITION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getPosition: cannot send request"));
		delete [] sendBuff;
		return v;
	}
	delete [] sendBuff;

	int recvSize = sizeof(double) * 3 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getPosition: failed to recieve position data"));
		delete [] recvBuff;
		return v;
	}
  
	p = recvBuff;
	bool success = BINARY_GET_BOOL_INCR(p);
	if (success) {
		double x = BINARY_GET_DOUBLE_INCR(p);
		double y = BINARY_GET_DOUBLE_INCR(p);
		double z = BINARY_GET_DOUBLE_INCR(p);
    
		v.set(x, y, z);
	}
	delete [] recvBuff; // added by Tetsunari Inamura on 2014-01-03
	return v;
}


// fixed a memory leak bug by Tetsunari Inamura on 2014-01-03
Rotation & SimObj::getRotation(Rotation &r)
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 2;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_ENTITY_ROTATION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getRotation: cannot send request."));
		delete [] sendBuff;
		return r;
	}
	delete [] sendBuff;

	int recvSize = sizeof(double) * 4 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getRotation: failed to recieve rotation data"));
		delete [] recvBuff;
		return r;
	}
  
	p = recvBuff;
	bool success = BINARY_GET_BOOL_INCR(p);
	if (success) {
		double qw = BINARY_GET_DOUBLE_INCR(p);
		double qx = BINARY_GET_DOUBLE_INCR(p);
		double qy = BINARY_GET_DOUBLE_INCR(p);
		double qz = BINARY_GET_DOUBLE_INCR(p);
    
		r.setQuaternion(qw, qx, qy, qz);
	}
	delete [] recvBuff; // added by Tetsunari Inamura on 2014-01-03
	//r.setQuaternion(qw(), qx(), qy(), qz());
	return r;
}


Vector3d & SimObj::getForce(Vector3d &v)
{
	v.set(fx(), fy(), fz());
	return v;
}

Vector3d & SimObj::getAccel(Vector3d &v)
{
	v.set(fx(), fy(), fz());
	v/=mass();
	return v;
}

Vector3d & SimObj::getTorque(Vector3d &v)
{
	v.set(tqx(), tqy(), tqz());
	return v;
}


void SimObj::setCameraViewPoint(Vector3d v,int camID)
{
	if (m_parts.size() == 1)
		{
			if (camID == 1)
				{
					vpx(v.x());
					vpy(v.y());
					vpz(v.z());
				}
			else
				{
					LOG_ERR(("setCameraViewPoint : cannot get Camera ID %d",camID));
				}
		}
	else
		{
			if      (camID == 1) {epx1(v.x()); epy1(v.y()); epz1(v.z());}
			else if (camID == 2) {epx2(v.x()); epy2(v.y()); epz2(v.z());}
			/*
			else if (camID == 3) {epx3(v.x()); epy3(v.y()); epz3(v.z());}
			else if (camID == 4) {epx4(v.x()); epy4(v.y()); epz4(v.z());}
			else if (camID == 5) {epx5(v.x()); epy5(v.y()); epz5(v.z());}
			else if (camID == 6) {epx6(v.x()); epy6(v.y()); epz6(v.z());}
			else if (camID == 7) {epx7(v.x()); epy7(v.y()); epz7(v.z());}
			else if (camID == 8) {epx8(v.x()); epy8(v.y()); epz8(v.z());}
			else if (camID == 9) {epx9(v.x()); epy9(v.y()); epz9(v.z());}
			*/
			else
				{
					LOG_ERR(("setCameraViewPoint : cannot get Camera ID %d",camID));
				}
		}
}


Vector3d & SimObj::getCameraViewPoint(Vector3d &v,int camID)
{
	if (m_parts.size() == 1)
		{
			if (camID == 1)
				v.set(vpx(), vpy(), vpz());
			else
				{
					LOG_ERR(("getCameraViewPoint : cannot get camera ID [%d]",camID));
				} 
		}
	else
		{
			if      (camID == 1) getCamera1ViewPoint(v);
			else if (camID == 2) getCamera2ViewPoint(v);
			/*
			else if (camID == 3)   getCamera3ViewPoint(v);
			else if (camID == 4)   getCamera4ViewPoint(v);
			else if (camID == 5)   getCamera5ViewPoint(v);
			else if (camID == 6)   getCamera6ViewPoint(v);
			else if (camID == 7)   getCamera7ViewPoint(v);
			else if (camID == 8)   getCamera8ViewPoint(v);
			else if (camID == 9)   getCamera9ViewPoint(v);
			*/
			else
				LOG_ERR(("getCameraViewPoint : cannot get Camera ID [%d]",camID));
		}
	return v;
}


void SimObj::setCameraViewVector(Vector3d v,int camID)
{
	if (m_parts.size() == 1) {
		if (camID == 1) {
			vvx(v.x()); 
			vvy(v.y()); 
			vvz(v.z());
		}
		else {
			LOG_ERR(("setCameraViewVector : cannot get Camera ID [%d]",camID));
		}
	}
	else {
		if      (camID == 1) {evx1(v.x()); evy1(v.y()); evz1(v.z());}
		else if (camID == 2) {evx2(v.x()); evy2(v.y()); evz2(v.z());}
		else {
			LOG_ERR(("setCameraViewVector : cannot get Camera ID [%d]",camID));
		}
	}
}


Vector3d & SimObj::getCamera1ViewPoint(Vector3d &v)
{
	v.set(epx1(), epy1(), epz1());
	return v;
}

Vector3d & SimObj::getCamera2ViewPoint(Vector3d &v)
{
	v.set(epx2(), epy2(), epz2());
	return v;
}

Vector3d & SimObj::getCamera1ViewVector(Vector3d &v)
{
	v.set(evx1(), evy1(), evz1());
	return v;
}

Vector3d & SimObj::getCamera2ViewVector(Vector3d &v)
{
	v.set(evx2(), evy2(), evz2());
	return v;
}

void SimObj::setPosition(double x, double y, double z)
{

	Super::setPosition(x, y, z);
	m_ops |= OP_SET_POSITION;

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 2 + sizeof(double) * 3;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_ENTITY_POSITION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	BINARY_SET_DOUBLE_INCR(p, x);
	BINARY_SET_DOUBLE_INCR(p, y);
	BINARY_SET_DOUBLE_INCR(p, z);

	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setPosition: cannot send position data"));
		delete [] sendBuff;
		return;
	}
	delete [] sendBuff;
	return;
}


bool SimObj::getIsGrasped()
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	// Get socket for sending/receiving entity data
	SOCKET sock = conim->getDataSock();

	// Sending self name, joint name and offset
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 2;

	// Get buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Prepare header
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_ISGRASPED);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
  
	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getIsGrasped: cannot send request"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	char recvBuff[8]; //TODO: Magic number should be removed
	// Receiving the result
	if (!SocketUtil::recvData(sock, recvBuff, sizeof(bool))) {
		LOG_ERR(("getIsGrasped: cannot receive result"));    
		//delete [] recvBuff; //comment out by Tetsunari Inamura on 2014-01-03
		return false;
	}
  
	p = recvBuff;
	bool result = BINARY_GET_BOOL_INCR(p);

	return result;
}


/**
 * Set position of entity
 * @param v position vector
 */
void SimObj::setPosition(const Vector3d &v)
{
	this->setPosition(v.x(), v.y(), v.z());
}


void SimObj::setAxisAndAngle(double ax, double ay, double az, double angle)
{
	Super::setAxisAndAngle(ax, ay, az, angle);
	m_ops |= OP_SET_ROTATION;

	Rotation r;
	r.setAxisAndAngle(ax, ay, az, angle);
	dReal *q = (dReal *)r.q();

	setEntityQuaternion(q, true);
}

void SimObj::setAxisAndAngle(double ax, double ay, double az, double angle, double direct)
{
	Super::setAxisAndAngle(ax, ay, az, angle, direct);
	m_ops |= OP_SET_ROTATION;
    
	Rotation r;
	r.setAxisAndAngle(ax, ay, az, angle);
	dReal *q = (dReal *)r.q();
  
	setEntityQuaternion(q, false);
}

void SimObj::setRotation(const Rotation &r)
{
	Super::setRotation(r);
	m_ops |= OP_SET_ROTATION;
  
	dReal *q = (dReal *)r.q();
	setEntityQuaternion(q, true);
}


// Renamed from sendEntityQuaternion by Tetsunari Inamura on 2014-03-04
/**
 * Set Quaternion of entity
 * @param qua array of real with 4 elements to represent quaternion
 * @param abs true: Absolute quaternion,  false: Relative rotation by quaternion
 */
bool SimObj::setEntityQuaternion(dReal *qua, bool abs)
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;
  
	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 3 + sizeof(double) * 4;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_ENTITY_ROTATION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	BINARY_SET_DATA_S_INCR(p, unsigned short, abs);
	BINARY_SET_DOUBLE_INCR(p, qua[0]);
	BINARY_SET_DOUBLE_INCR(p, qua[1]);
	BINARY_SET_DOUBLE_INCR(p, qua[2]);
	BINARY_SET_DOUBLE_INCR(p, qua[3]);

	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setEntityQuaternion: cannot send data"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
  
	return true;
}


void SimObj::setForce(double fx, double fy, double fz)
{
	Super::setForce(fx, fy, fz);
	m_ops |= OP_SET_FORCE;
}

void SimObj::addForce(double fx, double fy, double fz)
{
	assert(m_sender);
	CommRequestAddForceEncoder enc(name(), fx, fy, fz, false);
	m_sender->send(enc);
}


void SimObj::addForceAtPos(double fx, double fy, double fz, double px, double py, double pz)
{
	assert(m_sender);
	CommRequestAddForceAtPosEncoder enc(name(), fx, fy, fz, px, py, pz, false, false);
	m_sender->send(enc);
}

void SimObj::addForceAtRelPos(double fx, double fy, double fz, double px, double py, double pz)
{
	assert(m_sender);
	CommRequestAddForceAtPosEncoder enc(name(), fx, fy, fz, px, py, pz, true, false);
	m_sender->send(enc);
}

void SimObj::addRelForce(double fx, double fy, double fz)
{
	assert(m_sender);
	CommRequestAddForceEncoder enc(name(), fx, fy, fz, true);
	m_sender->send(enc);
}

void SimObj::addRelForceAtPos(double fx, double fy, double fz, double px, double py, double pz)
{
	assert(m_sender);
	CommRequestAddForceAtPosEncoder enc(name(), fx, fy, fz, px, py, pz, false, true);
	m_sender->send(enc);
}

void SimObj::addRelForceAtRelPos(double fx, double fy, double fz, double px, double py, double pz)
{
	assert(m_sender);
	CommRequestAddForceAtPosEncoder enc(name(), fx, fy, fz, px, py, pz, true, true);
	m_sender->send(enc);
}

void SimObj::addForceToParts(const char* parts, double fx, double fy, double fz)
{
	assert(m_sender);
	CommRequestAddForceToPartsEncoder enc(name(), parts, fx, fy, fz);
	m_sender->send(enc);
}

void SimObj::addTorque(double fx, double fy, double fz)
{
	assert(m_sender);
	CommRequestAddTorqueEncoder enc(name(), fx, fy, fz, false);
	m_sender->send(enc);
}

void SimObj::setGravityMode(bool gravity)
{
	assert(m_sender);
	CommRequestSetGravityModeEncoder enc(name(), gravity);
	m_sender->send(enc);
}

int SimObj::getGravityMode()
{
	assert(m_sender);
	CommRequestGetGravityModeEncoder enc(name());
	m_sender->send(enc);
	const int BUFSIZE = 128;
	CommDataResult *r = m_sender->recv(BUFSIZE);
	if (r == NULL || r->type() != COMM_RESULT_GET_GRAVITY_MODE) {
		return -1;
	}
	ResultGetGravityModeEvent *evt = (ResultGetGravityModeEvent *)r->data();
	if (strcmp(evt->getAgentName(),name()) != 0) {
		return -1;
	}
	int gravity = evt->grav();
	return gravity;
}

void SimObj::setDynamicsMode(bool dynamics)
{
	assert(m_sender);
	CommRequestSetDynamicsModeEncoder enc(name(), dynamics);
	m_sender->send(enc);
	Super::dynamics(dynamics);
}

bool SimObj::getDynamicsMode()
{
	bool dynamics = Super::dynamics();
	return  dynamics;
}

void SimObj::setMass(double mass)
{
	assert(m_sender);
	CommRequestSetMassEncoder enc(name(), mass);
	m_sender->send(enc);
	Super::mass(mass);
}

double SimObj::getMass()
{
	double mass = Super::mass();
	return  mass;
}

void SimObj::setAccel(double ax, double ay, double az)
{
	double m = mass();
	setForce(m*ax, m*ay, m*az);
}


void SimObj::setTorque(double x, double y, double z)
{
	Super::setTorque(x, y, z);
	m_ops |= OP_SET_TORQUE;
}


/**
 * @brief Set linear velocity of the entity
 * @param vx X element of velocity
 * @param vy Y element of velocity
 * @param vz Z element of velocity
 */
// Changed from setVelocity by Tetsunari Inamura on 2013-12-30
void SimObj::setLinearVelocity(double vx,double vy,double vz)
{
	Super::setLinearVelocity(vx,vy,vz);
	// m_ops |= OP_SET_LINEAR_VELOCITY;    // deleted by Tetsunari Inamura on 2014-01-06

	// Added by Tetsunari Inamura on 2014-01-06
	assert(m_sender);
	CommRequestSetLinearVelocityEncoder enc(name(), vx, vy, vz);
	m_sender->send(enc);
}


/**
 * @brief Refer linear velocity of the entity
 * @param vec velocity vector
 */
// Changed from setVelocity by Tetsunari Inamura on 2013-12-30
void SimObj::getLinearVelocity(Vector3d &vec)
{
	CommRequestGetLinearVelocityEncoder enc(name());
	m_sender->send(enc);
	const int BUFSIZE = 128; // TODO: Magic number should be removed
	CommDataResult *r = m_sender->recv(BUFSIZE);
	if (r == NULL || r->type() != COMM_RESULT_GET_LINEAR_VELOCITY) {
		return;
	}
	ResultGetLinearVelocityEvent *evt = (ResultGetLinearVelocityEvent *)r->data();
	if (strcmp(evt->getAgentName(),name()) != 0) {
		return;
	}
	vec.x(evt->x());
	vec.y(evt->y());
	vec.z(evt->z());
}


void SimObj::setAngularVelocity(double x,double y,double z)
{
	Super::setAngularVelocity(x,y,z);
	m_ops |= OP_SET_ANGULAR_VELOCITY;
}


void SimObj::getAngularVelocity(Vector3d &v)
{
	assert(m_sender);
	CommRequestGetAngularVelocityEncoder enc(name());
	m_sender->send(enc);
	const int BUFSIZE = 128; //TODO: Magic number
	CommDataResult *r = m_sender->recv(BUFSIZE);
	if (r == NULL || r->type() != COMM_RESULT_GET_ANGULAR_VELOCITY) {
		return;
	}
	ResultGetAngularVelocityEvent *evt = (ResultGetAngularVelocityEvent *)r->data();
	if (strcmp(evt->getAgentName(),name()) != 0) {
		return;
	}
	v.x(evt->x());
	v.y(evt->y());
	v.z(evt->z());
}


void SimObj::setJointAngle(const char *jointName, double angle)
{
	assert(m_sender);
	CommRequestSetJointAngleEncoder enc(name(), jointName, angle);
	m_sender->send(enc);
}

//added by okamoto@tome 2011/2/17
void SimObj::setJointQuaternion(const char *jointName, double qw, double qx, double qy, double qz, bool offset)
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
	msg += std::string(jointName) + ",";
	if (offset) 
		msg += "1,";
	else
		msg += "0,";

	int sendSize = msg.size() + sizeof(unsigned short) * 2 + sizeof(double) * 4;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_JOINT_QUATERNION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	BINARY_SET_DOUBLE_INCR(p, qw);
	BINARY_SET_DOUBLE_INCR(p, qx);
	BINARY_SET_DOUBLE_INCR(p, qy);
	BINARY_SET_DOUBLE_INCR(p, qz);

	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setJointQuaternion: cannot send request"));
		delete [] sendBuff;
		return;
	}
	delete [] sendBuff;
	return;
}


//added by okamoto@tome 2011/3/3
void SimObj::addJointTorque(const char *jointName, double t)
{
	if (dynamics()) {
		CommRequestAddJointTorqueEncoder enc(name(), jointName, t);
		m_sender->send(enc);
	}
	else {
		LOG_ERR(("addJointTorque : dynamics is off"));
	}
}


// Added by inamura on 2013-12-31
// To keep compatibility with versoin before v2.2.0
void SimObj::setJointVelocity(const char *jointName, double v, double max)
{
	setAngularVelocityToJoint(jointName, v, max);
}


//added by okamoto@tome 2011/3/9 (modify 2013/3/7)
//modiified by inamura on 2013-12-30
void SimObj::setAngularVelocityToJoint(const char *jointName, double v, double max)
{
	/*
	  assert(m_sender);
	  CommRequestSetAngularVelocityToJointEncoder enc(name(), jointName, v, max);
	  m_sender->send(enc);
	*/
  
	Controller *con       = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
	msg += std::string(jointName) + ",";

	int sendSize = msg.size() + sizeof(unsigned short) * 2 + sizeof(double) * 2;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_JOINT_VELOCITY);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	BINARY_SET_DOUBLE_INCR(p, v);
	BINARY_SET_DOUBLE_INCR(p, max);

	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setAngularVelocityToJoint: cannot send data"));
		delete [] sendBuff;
		return;
	}
	delete [] sendBuff;

}


//modiified by inamura on 2013-12-30
void SimObj::setAngularVelocityToParts(const char *partsName, double v, double max)
{
	assert(m_sender);
	CommRequestSetAngularVelocityToPartsEncoder enc(name(), partsName, v, max);
	m_sender->send(enc);
}



//added by okamoto@tome 2011/3/9(modify 2013/3/7)
double SimObj::getJointAngle(const char *jointName)
{
	// This is old version
	/*
	  assert(m_sender);
	  CommRequestGetJointAngleEncoder enc(name(), jointName);
	  m_sender->send(enc);
	  const int BUFSIZE = 128;
	  CommDataResult *r = m_sender->recv(BUFSIZE);
	  if (r == NULL || r->type() != COMM_RESULT_GET_JOINT_ANGLE) { return false; }
	  ResultGetJointAngleEvent *evt = (ResultGetJointAngleEvent *)r->data();
	  double angle = evt->getAngle();
	*/

	// Sending self name and joint name
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
	msg += std::string(jointName) + ",";

	if (!sendRequest(msg, REQUEST_GET_JOINT_ANGLE)) {
		LOG_ERR(("getJointAngle: cannot send request"));
	}
	// Prepare buffer
	int recvSize = sizeof(double) + sizeof(bool);
	char *recvBuff = new char[recvSize];

	// Get socket from sender
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;
	SOCKET sock = conim->getDataSock();

	// Receiving the result
	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getJointAngle: cannot get joint angle"));
		delete [] recvBuff;
		return false;
	}
  
	char *p = recvBuff;
	bool success = BINARY_GET_BOOL_INCR(p);
	double angle = BINARY_GET_DOUBLE_INCR(p);
  
	delete [] recvBuff; // add by Tetsunari Inamura on 2014-01-03
	return angle;
}


//added by okamoto@tome 2011/3/9(modify 2013/3/7)
std::map<std::string, double> SimObj::getAllJointAngles()
{
	std::map<std::string, double> alljoints;
  
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
	//msg += std::string(jointName) + ",";
  
	if (!sendRequest(msg, REQUEST_GET_ALL_JOINT_ANGLES)) {
		LOG_ERR(("getAllJointAngles: cannot send request"));
	}
  
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	char tmp[8];
	if (!SocketUtil::recvData(sock, tmp, 4)) { //TODO: magic number
		LOG_ERR(("getJointAngle: cannot get data 1/2"));
		return alljoints;
	}
	char *p = tmp;
	unsigned short recvSize  = BINARY_GET_DATA_S_INCR(p, unsigned short);
	unsigned short jointSize = BINARY_GET_DATA_S_INCR(p, unsigned short);
	recvSize -= 4;

	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getJointAngle: cannot get data 2/2"));
		delete [] recvBuff;
		return alljoints;
	}
  
	p = recvBuff;

	for (int i = 0; i < jointSize; i++) {
    
		std::string name;
		if (i == 0) {
			name = strtok(p,",");
		}
		else{
			name = strtok(NULL,",");
		}

		double angle = atof(strtok(NULL,","));
		alljoints.insert(std::map<std::string, double>::value_type(name, angle));
		//LOG_MSG(("ch(%s, %f)", name.c_str(), angle));
	}

	delete [] recvBuff;
	return alljoints;
}


// added by okamoto@tome 2012/11/13
// fixed a memory leak bug by Tetsunari Inamura on 2014-01-03
bool SimObj::getJointPosition( Vector3d &pos, const char *jointName)
{
	Controller *con       = (Controller *)m_sender;
	ControllerImpl *conim = (ControllerImpl *)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
	msg += std::string(jointName) + ",";
	unsigned char sendSize = msg.size();
	sendSize += sizeof(unsigned short) * 2;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_JOINT_POSITION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getJointPosition: cannot get joint position"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	int recvSize = sizeof(double) * 3 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getJointPosition: cannot get joint position"));
		delete [] recvBuff;
		return false;
	}
  
	p = recvBuff;
	double x = BINARY_GET_DOUBLE_INCR(p);
	double y = BINARY_GET_DOUBLE_INCR(p);
	double z = BINARY_GET_DOUBLE_INCR(p);
	bool success = BINARY_GET_BOOL_INCR(p);

	pos.set(x, y, z);
	delete [] recvBuff; // added by Tetsunari Inamura on 2014-01-03
	
	if (!success) return false;
	return true;
}


//added by okamoto@tome 2012/11/16
// fixed a memory leak bug by Tetsunari Inamura on 2014-01-03
bool SimObj::getPartsPosition(Vector3d &pos, const char *partsName)
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
	msg += std::string(partsName) + ",";
	unsigned char sendSize = msg.size();
	sendSize += sizeof(unsigned short) * 2;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_PARTS_POSITION);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getPartsPosition: cannot get Parts position 1/2"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	int recvSize = sizeof(double) * 3 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getPartsPosition: cannot get Parts position 2/2"));
		delete [] recvBuff;
		return false;
	}
  
	p = recvBuff;
	double x = BINARY_GET_DOUBLE_INCR(p);
	double y = BINARY_GET_DOUBLE_INCR(p);
	double z = BINARY_GET_DOUBLE_INCR(p);
	bool success = BINARY_GET_BOOL_INCR(p);

	pos.set(x, y, z);
	delete [] recvBuff; // added by Tetsunari Inamura on 2014-01-03

	if (!success) return false;
	return true;
}



bool SimObj::getJointForce(const char *jointName, JointForce &jf1, JointForce &jf2)
{
	if (!dynamics()) {
		LOG_ERR(("getJointForce : dynamics is off"));
		return false;
	}
	if (jointName == NULL || strlen(jointName) <= 0) {
		LOG_ERR(("getJointForce : no joint name"));
		return false;
	}

	CommRequestGetJointForceEncoder enc(name(), jointName);
	m_sender->send(enc);

	const int BUFSIZE = 1024; //TODO: Magic number
	CommDataResult *r = m_sender->recv(BUFSIZE);
	if (r == NULL || r->type() != COMM_RESULT_GET_JOINT_FORCE) {
		return false;
	}
	ResultGetJointForceEvent *evt = (ResultGetJointForceEvent *)r->data();
	if (!evt->success()) {
		return false;
	}
	JointForce *jf = evt->getJointForce(0);
	jf1 = *jf;
	jf = evt->getJointForce(1);
	jf2 = *jf;
	return true;
}


void SimObj::setCollisionEnable(bool flag)
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();

	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";

	int sendSize = sizeof(unsigned short) * 2 + sizeof(bool) + msg.size();

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_COLLISIONABLE);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_BOOL_INCR(p, flag );
	memcpy(p, msg.c_str(), msg.size());
	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setCollisionable: cannot send request to server"));
	}
	delete [] sendBuff;
}


// added by noma@tome (2012/02/20)
std::vector<const char*> SimObj::getPointedObject(const char* speakerName, int lrFlag, int lineID, int typicalType) {

	using namespace std;

	if (speakerName==NULL) {
		LOG_ERR(("getPointedObject : no speaker name"));
		std::vector<const char*> tmpRetVec;
		return tmpRetVec;
	}
	if (lrFlag<0 || lrFlag> 1) {
		LOG_ERR(("getPointedObject : invalid range of lrFlag"));
		std::vector<const char*> tmpRetVec;
		return tmpRetVec;
	}
	if (lineID<1 || lineID> 2) {
		LOG_ERR(("getPointedObject : invalid range of lineID"));
		std::vector<const char*> tmpRetVec;
		return tmpRetVec;
	}
	if (typicalType <0 || typicalType> 2) {
		LOG_ERR(("getPointedObject : invalid range of typicalType"));
		std::vector<const char*> tmpRetVec;
		return tmpRetVec;
	}

	char* partsName0;
	char* partsName1;
	if (lrFlag == 0) {
		if (lineID == 1) {
			partsName0 = (char *)"LEYE_LINK";
		}else if (lineID == 2) {
			partsName0 = (char *)"LARM_LINK4";
		}
		partsName1 = (char *)"LARM_LINK7";
	} else if (lrFlag == 1) {
		if (lineID == 1) {
			partsName0 = (char *)"REYE_LINK";
		}else if (lineID == 2) {
			partsName0 = (char *)"RARM_LINK4";
		}
		partsName1 = (char *)"RARM_LINK7";
	}

	assert(m_sender);
	CommRequestGetPointedObjectEncoder enc(speakerName, partsName0, partsName1, typicalType);
	m_sender->send(enc);
	const int BUFSIZE = 1024; // TODO: Magic number should be removed
	CommDataResult *r = m_sender->recv(BUFSIZE);

	if (r == NULL || r->type() != COMM_RESULT_GET_POINTED_OBJECT) {
		LOG_ERR(("getPointedObject : receive error"));
		std::vector<const char*> tmpRetVec;
		return tmpRetVec;
	}
	ResultGetPointedObjectEvent *evt = (ResultGetPointedObjectEvent *)r->data();

	std::vector<const char*> candidateVec;
	evt->copy(candidateVec);

	return candidateVec;
}

// added by okamoto@tome (2012/11/14)
// fixed a memory leak bug by Tetsunari Inamura on 2014-01-03
bool SimObj::getPointingVector(Vector3d &vec, const char *joint1, const char *joint2)
{
	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;
	
	SOCKET sock = conim->getDataSock();
	
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
	msg += std::string(joint1) + ",";
	msg += std::string(joint2) + ",";
	int sendSize = msg.size() + sizeof(unsigned short) * 2;
	
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_POINTING_VECTOR);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);      
	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("getPointingVector: cannot send request to server"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	int recvSize = sizeof(double) * 3 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	if (!SocketUtil::recvData(sock, recvBuff, recvSize)) {
		LOG_ERR(("getJointPosition: cannot get joint position"));
		delete [] recvBuff;
		return false;
	}
  
	p = recvBuff;
	double x = BINARY_GET_DOUBLE_INCR(p);
	double y = BINARY_GET_DOUBLE_INCR(p);
	double z = BINARY_GET_DOUBLE_INCR(p);
	bool success = BINARY_GET_BOOL_INCR(p);
	vec.set(x, y, z);

	delete [] recvBuff; // added by Tetsunari Inamura on 2014-01-03

	if (!success) return false;
	return true;
}


// added by okamoto@tome (2012/11/14)
bool SimObj::getPointingVector(Vector3d &vec, int lrFrag)
{
	if (lrFrag == 0)
		return getPointingVector(vec, "LARM_JOINT4", "LARM_JOINT7");
	else if (lrFrag == 1)
		return getPointingVector(vec, "RARM_JOINT4", "RARM_JOINT7");
}


inline bool NULL_STRING(const char *str)
{
	return (str == NULL || strlen(str) <= 0)? true: false;
}

void SimObj::connectJoint(const char *jointName, const char *myParts,
		     const char *targetName, const char *targetParts)
{
	if (NULL_STRING(jointName)) {
		LOG_ERR(("connectJoint : no joint name"));
		return;
	}
	if (NULL_STRING(targetName)) {
		LOG_ERR(("connectJoint : no target name"));
		return;
	}
	CommRequestConnectJointEncoder enc(jointName,
					   name(), myParts,
					   targetName, targetParts);
	m_sender->send(enc);

}
void SimObj::releaseJoint(const char *jointName)
{
	if (NULL_STRING(jointName)) {
		LOG_ERR(("releaseJoint : no target name"));
		return;
	}
	CommRequestReleaseJointEncoder enc(name(), jointName);
	m_sender->send(enc);
}

bool SimObj::sendRequest(std::string msg, int type)
{

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;

	SOCKET sock = conim->getDataSock();
	int sendSize = msg.size() + sizeof(unsigned short) * 2;

	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, type);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;
	return true;
}


#ifdef DEPRECATED
Command * SimObj::createJointControlCommand()
{
	if (m_jointValues.size() == 0) { return 0; }
	JointControlCommand *cmd = new JointControlCommand(name());

	typedef JointValueM M;
	M &m = m_jointValues;
	for (M::iterator i=m.begin(); i!=m.end(); i++) {
		S name = i->first;
		double v = i->second;
		cmd->set(name.c_str(), v);
	}
	return cmd;
}
#endif
#endif //	CONTROLLER



bool RobotObj::setWheel(double wheelRadius, double wheelDistance)
{
	m_wheelRadius   = wheelRadius;
	m_wheelDistance = wheelDistance;

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;
  
	SOCKET sock = conim->getDataSock();
  
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
  
	int sendSize = msg.size() + sizeof(unsigned short) * 2 + sizeof(double) * 2;
  
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_WHEEL);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	BINARY_SET_DOUBLE_INCR(p, wheelRadius);
	BINARY_SET_DOUBLE_INCR(p, wheelDistance);

	memcpy(p, msg.c_str(), msg.size());

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setWheel: cannot send request"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	return true;
}


bool RobotObj::setWheelVelocity(double left, double right)
{

	Controller *con = (Controller*)m_sender;
	ControllerImpl *conim = (ControllerImpl*)con;
  
	SOCKET sock = conim->getDataSock();
  
	std::string msg;
	const char *myName = name();
	msg += std::string(myName) + ",";
  
	int sendSize = msg.size() + sizeof(unsigned short) * 2 + sizeof(double) * 2;
  
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;
  
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_SET_WHEEL_VELOCITY);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	BINARY_SET_DOUBLE_INCR(p, left);
	BINARY_SET_DOUBLE_INCR(p, right);

	memcpy(p, msg.c_str(), msg.size());  

	if (!SocketUtil::sendData(sock, sendBuff, sendSize)) {
		LOG_ERR(("setWheelVelocity: cannot send request"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	return true;
}
