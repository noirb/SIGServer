/* $Id: LogMsg.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/event/LogMsg.h>
#include <sigverse/commonlib/binary.h>

bool LogMsgEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	m_level = BINARY_GET_DATA_S_INCR(p, short);
	char *msg = BINARY_GET_STRING_INCR(p);

	if (msg) {
		m_msg = msg;
	}
	BINARY_FREE_STRING(msg);
	return true;
}

