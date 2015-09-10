/* $Id: ReleaseJoint.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/commevent/ReleaseJoint.h>
#include <sigverse/commonlib/binary.h>

bool RequestReleaseJointEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	char *str = BINARY_GET_STRING_INCR(p);
	if (str) {
		m_agentName = str;
	}
	BINARY_FREE_STRING(str);

	str = BINARY_GET_STRING_INCR(p);
	if (str) {
		m_jointName = str;
	}
	BINARY_FREE_STRING(str);
	return true;
}

