/* $Id: RequestGetEntity.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/event/RequestGetEntity.h>
#include <sigverse/commonlib/binary.h>

bool RequestGetEntityEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	char *name = BINARY_GET_STRING_INCR(p);
	m_name = name;
	BINARY_FREE_STRING(name);
	return true;
}


