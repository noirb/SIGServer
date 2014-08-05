/* Written by okamoto on 2011-03-31
 * Modified by Tetsunari Inamura on 2013-12-29
 *   Add English comments (Translation from v2.2.0 is finished)
 */

#include "CParts.h"
#include "PartsCmpnt.h"
#include "binary.h"
#include "ControllerImpl.h"

const dReal *CParts::getPosition()
{

	return m_pos.values();
}

const dReal * CParts::getRotation()
{
#ifdef USE_ODE	// taku // TODO: what's this taku? by inamura
	return m_rot.matrix();
#else
	return NULL;
#endif
}

const dReal * CParts::getQuaternion()
{
	return m_rot.q();
}

#ifdef CONTROLLER


// TODO: Almost all the codes are identical to SimObj::getPartsPosition. They should be integrated
bool CParts::getPosition(Vector3d &v)
{
  /*
	v.set(m_pos.x(), m_pos.y(), m_pos.z());
	return v;
  */

	// Sending owner and parts name
	std::string msg;
	//const char *myName = name();
	msg += std::string(m_owner) + ",";
	msg += std::string(name()) + ",";
	unsigned char sendSize = msg.size();
	sendSize += sizeof(unsigned short) * 2;

	// Preparation of buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, 14); //TODO: Magic number should be removed, use REQUEST_GET_PARTS_POSITION
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg.c_str(), msg.size());

	if(!SocketUtil::sendData(m_sock, sendBuff, sendSize)) {
		LOG_ERR(("getPartsPosition: cannot get joint position SENDDATA"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	// Buffer for receive
	int recvSize = sizeof(double) * 3 + sizeof(bool);
	char *recvBuff = new char[recvSize];

	// Receive the result
	if(!SocketUtil::recvData(m_sock, recvBuff, recvSize)) {
		LOG_ERR(("getPartsPosition: cannot get joint position RECVDATA"));
		delete [] recvBuff;
		return false;
	}

	p = recvBuff;
	double x = BINARY_GET_DOUBLE_INCR(p);
	double y = BINARY_GET_DOUBLE_INCR(p);
	double z = BINARY_GET_DOUBLE_INCR(p);
	bool success = BINARY_GET_BOOL_INCR(p);

	v.set(x, y, z);

	if(!success) return false;

	return true;
}

bool CParts::graspObj(std::string objName)
{
	/*
	v.set(m_pos.x(), m_pos.y(), m_pos.z());
	return v;
	*/

	// Sending the owner name, parts name and object name which should be grasped
	std::string msg;
	//const char *myName = name();
	msg += std::string(m_owner) + ",";
	msg += std::string(name()) + ",";
	msg += objName + ",";
	unsigned char sendSize = msg.size();
	sendSize += sizeof(unsigned short) * 2;

	// Preparation of buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, 31);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg.c_str(), msg.size());

	if(!SocketUtil::sendData(m_sock, sendBuff, sendSize)) {
		LOG_ERR(("getPartsPosition: cannot get joint position"));
		delete [] sendBuff;
		return false;
	}
	delete [] sendBuff;

	// Buffer for Receiving
	int recvSize = sizeof(unsigned short);
	char *recvBuff = new char[recvSize];

	// Receive the result
	if(!SocketUtil::recvData(m_sock, recvBuff, recvSize)) {
		LOG_ERR(("getPartsPosition: cannot get joint position"));
		delete [] recvBuff;
		return false;
	}

	p = recvBuff;
	unsigned short result = BINARY_GET_DATA_S_INCR(p,unsigned short);

	if(result == 1) {
		LOG_ERR(("graspObj: cannot find object %s [%s, %d]",objName.c_str() , __FILE__, __LINE__));
		return false;
	}
	else if(result == 2) {
		LOG_ERR(("graspObj: %s is already grasping.", name()));
		return false;
	}
	else if(result == 3) {
		LOG_ERR(("graspObj: %s is already grasped.", objName.c_str()));
		return false;
	}
	else if(result == 4) {
		LOG_ERR(("graspObj: cannot grasp %s. ", objName.c_str()));
		return false;
	}
	else if(result == 5) {
		LOG_ERR(("graspObj: cannot grasp %s. out of reach", objName.c_str()));
		return false;
	}

	return true;
}


void CParts::releaseObj()
{
	std::string msg;

	// Sending owner and parts name
	//const char *myName = name();
	msg += std::string(m_owner) + ",";
	msg += std::string(name()) + ",";
	unsigned char sendSize = msg.size();
	sendSize += sizeof(unsigned short) * 2;

	// Preparation of buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, 32);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg.c_str(), msg.size());

	if(!SocketUtil::sendData(m_sock, sendBuff, sendSize)) {
		LOG_ERR(("getPartsPosition: cannot get joint position"));
	}

	delete [] sendBuff;
}


bool CParts::getCollisionState()
{
	std::string msg;

	// Sending owner and parts name
	//const char *myName = name();
	msg += std::string(m_owner) + ",";
	msg += std::string(name()) + ",";
	unsigned char sendSize = msg.size();
	sendSize += sizeof(unsigned short) * 2;

	// Preparation of buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	BINARY_SET_DATA_S_INCR(p, unsigned short, 39);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	memcpy(p, msg.c_str(), msg.size());

	if(!SocketUtil::sendData(m_sock, sendBuff, sendSize)) {
		LOG_ERR(("getCollisionState: faild to send request."));
	}
	// Buffer for receiving
	int recvSize = sizeof(bool);
	//char *recvBuff = new char[recvSize];
	char recvBuff[4];

	// Receive the result
	if(!SocketUtil::recvData(m_sock, recvBuff, recvSize)) {
		LOG_ERR(("getCollisionState: cannot get joint position"));
		return false;
	}

	p = recvBuff;
	bool result = BINARY_GET_BOOL_INCR(p);

	delete [] sendBuff;
	return result;
}


Rotation & CParts::getRotation(Rotation &r)
{
	r = m_rot;
	return r;
}

#endif

BoxParts::BoxParts(const char *name, const Position &pos, const Size &sz)
	: CParts(PARTS_TYPE_BOX, name, pos), m_cmpnt(NULL)
{
	m_cmpnt = new BoxPartsCmpnt(sz);
}

BoxParts::BoxParts(const BoxParts &o)
	: CParts(o), m_cmpnt(NULL)
{
	m_cmpnt = new BoxPartsCmpnt(*o.m_cmpnt);
}


BoxParts::~BoxParts()
{
	delete m_cmpnt;
	m_cmpnt = NULL;
}

void BoxParts::giveSize(double &x, double &y, double &z)
{
	Size &sz = m_cmpnt->size();
	x = (double)(sz.x());
	y = (double)(sz.y());
	z = (double)(sz.z());
}


PartsCmpnt * BoxParts::extdata()
{
	return m_cmpnt;
}

void BoxParts::dump()
{
#ifdef _DEBUG
	Size &sz = m_cmpnt->size();
	printf("\tparts(%d): %s(%s) pos(%f, %f, %f) size(%f, %f, %f)\n", id(), name(), "box",  m_pos.x(), m_pos.y(), m_pos.z(), sz.x(), sz.y(), sz.z());
#endif
}

Size BoxParts::getSize()
{
	return m_cmpnt->size();
}

CylinderParts::CylinderParts(const char *name,
	      const Position &pos, dReal rad, dReal len)
	: CParts(PARTS_TYPE_CYLINDER, name, pos), m_cmpnt(NULL)
{
	m_cmpnt = new CylinderPartsCmpnt(rad, len);
}

CylinderParts::CylinderParts(const CylinderParts &o) : CParts(o), m_cmpnt(NULL)
{
	m_cmpnt = new CylinderPartsCmpnt(*o.m_cmpnt);
}

CylinderParts::~CylinderParts()
{
	delete m_cmpnt;
	m_cmpnt = NULL;
}

void CylinderParts::giveSize(double &radius, double &length)
{
	radius = (double)m_cmpnt->radius();
	length = (double)m_cmpnt->length();
}


PartsCmpnt * CylinderParts::extdata()
{
	return m_cmpnt;
}

void CylinderParts::dump()
{
#ifdef _DEBUG
	printf("\tparts(%d): %s(%s) pos(%f, %f, %f) rad(%f) len(%f)\n", id(), name(), "cylinder",  m_pos.x(), m_pos.y(), m_pos.z(), m_cmpnt->radius(), m_cmpnt->length());
#endif
}

double CylinderParts::getLength(){
	return m_cmpnt->length();
}

double CylinderParts::getRad(){
	return m_cmpnt->radius();
}

SphereParts::SphereParts(const char *name, const Position &pos, double radius)
	: CParts(PARTS_TYPE_SPHERE, name, pos), m_cmpnt(NULL)
{
	m_cmpnt = new SpherePartsCmpnt(radius);
}

SphereParts::SphereParts(const SphereParts &o) : CParts(o), m_cmpnt(NULL)
{
	m_cmpnt = new SpherePartsCmpnt(*o.m_cmpnt);
}


SphereParts::~SphereParts()
{
	delete m_cmpnt;
	m_cmpnt = NULL;
}

void SphereParts::giveRadius(double &radius)
{
	radius = m_cmpnt->radius();
}


PartsCmpnt * SphereParts::extdata()
{
	return m_cmpnt;
}

void SphereParts::dump()
{
#ifdef _DEBUG
	printf("\tparts(%d): %s(%s) pos(%f, %f, %f) radius(%f)\n", id(), name(), "sphere",  m_pos.x(), m_pos.y(), m_pos.z(), m_cmpnt->radius());
#endif
}

double SphereParts::getRad(){
	return m_cmpnt->radius();
}




