/* $Id: ConnectJoint.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/event/ConnectJoint.h>
#include <sigverse/commonlib/binary.h>

#define ASSIGN_STR(P, MEMBER) { \
    char *str_ = BINARY_GET_STRING_INCR(P); \
    if (str_) { MEMBER = str_; } \
    BINARY_FREE_STRING(str_); \
}


bool RequestConnectJointEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	ASSIGN_STR(p, m_data.jointName);
	ASSIGN_STR(p, m_data.agentName);
	ASSIGN_STR(p, m_data.agentParts);
	ASSIGN_STR(p, m_data.targetName);
	ASSIGN_STR(p, m_data.targetParts);
	
	return true;
}

