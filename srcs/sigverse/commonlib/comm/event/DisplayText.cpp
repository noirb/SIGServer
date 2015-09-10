/* $Id: DisplayText.cpp,v 1.2 2011-08-10 02:52:14 okamoto Exp $ */ 
#include "DisplayText.h"
#include "binary.h"

bool DisplayTextEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	m_fs = BINARY_GET_DATA_S_INCR(p, short);
	m_color = BINARY_GET_DATA_S_INCR(p, short);
	char *msg = BINARY_GET_STRING_INCR(p);
	if (msg) {
		m_msg = msg;
	}
	BINARY_FREE_STRING(msg);
	return true;
}

