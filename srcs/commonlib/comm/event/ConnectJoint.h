/* $Id: ConnectJoint.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Event_ConnectJoint_h
#define Comm_Event_ConnectJoint_h

#include "comm/data/RequestConnectJointData.h"


class RequestConnectJointEvent
{
private:
	RequestConnectJointData m_data;
private:
	const char *getString(RequestConnectJointData::S s) {
		return s.length() > 0? s.c_str(): NULL;
	}
public:
	bool	set(int packetNum, int seq, char *data, int n);

public:
	const char * getJointName() { return getString(m_data.jointName); }
	const char * getAgentName() { return getString(m_data.agentName); }
	const char * getAgentPartsName() { return getString(m_data.agentParts); }
	const char * getTargetName() { return getString(m_data.targetName); }
	const char * getTargetPartsName() { return getString(m_data.targetParts); }
};


#endif // Comm_Event_ConnectJoint_h
 

