/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Comm_Encode_GetJointForce_h
#define Comm_Encode_GetJointForce_h

#include <sigverse/commonlib/comm/encoder/Encoder.h>
#include <sigverse/commonlib/comm/data/ResultGetJointForceData.h>

namespace CommData {

class RequestGetJointForce : public Encoder
{
private:
	enum { BUFSIZE = 256, }; //TODO: Magic number
private:
	std::string m_agentName;
	std::string m_jointName;
public:
	RequestGetJointForce(const char *agentName, const char *jointName)
		: Encoder(COMM_REQUEST_GET_JOINT_FORCE, BUFSIZE),
		  m_agentName(agentName), m_jointName(jointName) {}

	int     packetNum() { return 1; }
	char *  encode(int seq, int &);
};

class ResultGetJointForce : public Encoder
{
	enum { BUFSIZE = 2048, };
private:
	ResultGetJointForceData m_data;
public:
	ResultGetJointForce() : Encoder(COMM_RESULT_GET_JOINT_FORCE, BUFSIZE) {}
	void  success(bool b) { m_data.success(b); }

	JointForce * getJointForces() {
		return m_data.getJointForces();
	}

	int     packetNum() { return 1; }
	char *  encode(int seq, int &);
};

} // namespace

typedef CommData::RequestGetJointForce CommRequestGetJointForceEncoder;
typedef CommData::ResultGetJointForce CommResultGetJointForceEncoder;

#endif // Comm_Encode_GetJointForce_h
 

