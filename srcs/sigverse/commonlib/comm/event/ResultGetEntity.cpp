/* $Id: ResultGetEntity.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/event/ResultGetEntity.h>
#include <sigverse/commonlib/CommDataType.h>
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/SimObj.h>

#define FREE(P) if (P) { delete P; P = 0; }

bool ResultGetEntityEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	CommResultType result = BINARY_GET_DATA_S_INCR(p, CommResultType);
	if (result != COMM_RESULT_OK) { return true; }
	m_time = BINARY_GET_DOUBLE_INCR(p);
	unsigned short exist = BINARY_GET_DATA_S_INCR(p, unsigned short);

	if (exist) {
		RobotObj *obj = new RobotObj();
		if (obj->setBinary(p, n) < 0) { return false; }
		m_obj = (SimObj*)obj;
	}
	return true;
}

void ResultGetEntityEvent::free_()
{
	FREE(m_obj);
}


