/* $Id: JointAngle.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include "JointAngle.h"
#include "binary.h"

bool RequestSetJointAngleEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.agentName(name);
		}
		BINARY_FREE_STRING(name);
	}

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.jointName(name);
		}
		BINARY_FREE_STRING(name);
	}
	double angle = BINARY_GET_DOUBLE_INCR(p);
	m_data.angle(angle);
	return true;
}
