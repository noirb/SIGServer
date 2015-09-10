/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Comm_Event_GetJointForce_h
#define Comm_Event_GetJointForce_h

#include "comm/data/ResultGetJointForceData.h"

class RequestGetJointForceEvent 
{
private:
	std::string m_agentName;
	std::string m_jointName;
public:
	bool set(int packetNum, int seq, char *data, int n);

	const char *getAgentName() { return m_agentName.length()>0? m_agentName.c_str(): NULL; }
	const char *getJointName() { return m_jointName.length()>0? m_jointName.c_str(): NULL; }
};

class ResultGetJointForceEvent
{
private:
	ResultGetJointForceData m_data;
public:
	bool set(int packetNum, int seq, char *data, int n);

	bool success() { return m_data.success(); }
	int getJointForceNum() { return ResultGetJointForceData::JOINT_FORCE_NUM; }

	JointForce * getJointForce(int i) {
		if (0 <= i && i < ResultGetJointForceData::JOINT_FORCE_NUM) {
			JointForce *jfs = m_data.getJointForces();
			return &jfs[i];
		} else {
			return NULL;
		}
	}
};


#endif // Comm_Event_GetJointForce_h
 

