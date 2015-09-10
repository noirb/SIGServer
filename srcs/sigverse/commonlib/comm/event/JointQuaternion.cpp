/* $Id: JointQuaternion.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <sigverse/commonlib/comm/event/JointQuaternion.h>
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/Logger.h>

bool RequestSetJointQuaternionEvent::set(int packetNum, int seq, char *data, int n)
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

	{
		double qw = BINARY_GET_DOUBLE_INCR(p);
		if (qw != 0 ) {
			m_data.qw(qw);
		}
	}

	{
		double qx = BINARY_GET_DOUBLE_INCR(p);
		if (qx != 0) {
			m_data.qx(qx);
		}
	}

	{
		double qy = BINARY_GET_DOUBLE_INCR(p);
		if (qy != 0) {
			m_data.qy(qy);
		}
	}

	{
		double qz = BINARY_GET_DOUBLE_INCR(p);
		if (qz != 0) {
			m_data.qz(qz);
		}
	}

	{
		bool offset = BINARY_GET_BOOL_INCR(p);
		m_data.offset(offset);
	}

	return true;
}
