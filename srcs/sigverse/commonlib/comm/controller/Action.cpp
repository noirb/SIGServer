/* $Id: Action.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/comm/controller/Action.h>

bool ActionEvent::set(int packetNum, int seq, char *data, int n)
{
	if (n < (int)sizeof(m_currTime)) { return false; }
	char *p = data;
	m_currTime = BINARY_GET_DOUBLE_INCR(p);

	return true; 
}

