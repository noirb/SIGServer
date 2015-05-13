/* $Id: ReleaseJoint.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Event_ReleaseJoint_h
#define Comm_Event_ReleaseJoint_h

#include <string>

class RequestReleaseJointEvent
{
private:
	std::string m_agentName;
	std::string m_jointName;
public:
	bool set(int packetNum, int seq, char *data, int n);

	const char *getAgentName() {
		return m_agentName.length() > 0? m_agentName.c_str(): NULL;
	}
	const char *getJointName() {
		return m_jointName.length() > 0? m_jointName.c_str(): NULL;
	}
};

#endif // Comm_Event_ReleaseJoint_h
 

