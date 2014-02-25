/* $Id: GetJointForce.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Encode_GetJointForce_h
#define Comm_Encode_GetJointForce_h

#include "Encoder.h"
#include "comm/data/ResultGetJointForceData.h"

namespace CommData {

class RequestGetJointForce : public Encoder
{
private:
	enum { BUFSIZE = 256, };
private:
	std::string m_agentName;
	std::string m_jointName;
public:
	RequestGetJointForce(const char *agentName, const char *jointName)
		: Encoder(COMM_REQUEST_GET_JOINT_FORCE, BUFSIZE),
		  m_agentName(agentName), m_jointName(jointName) {}

	int 	packetNum() { return 1; }
	char *	encode(int seq, int &);
};

class ResultGetJointForce : public Encoder
{
	enum { BUFSIZE = 2048, };
private:
	ResultGetJointForceData m_data;
public:
	ResultGetJointForce() : Encoder(COMM_RESULT_GET_JOINT_FORCE, BUFSIZE) {}
	void	success(bool b) { m_data.success(b); }
	JointForce * getJointForces() {
		return m_data.getJointForces();
	}

	int 	packetNum() { return 1; }
	char *	encode(int seq, int &);
};

} // namespace

typedef CommData::RequestGetJointForce CommRequestGetJointForceEncoder;
typedef CommData::ResultGetJointForce CommResultGetJointForceEncoder;

#endif // Comm_Encode_GetJointForce_h
 

