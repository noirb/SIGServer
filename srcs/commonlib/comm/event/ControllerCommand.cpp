/* $Id: ControllerCommand.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#ifdef DEPRECATED

#include "ControllerCommand.h"
#include "Command.h"
#include "binary.h"

#define FREE(P) if (P) { delete P; P = 0; }

void ControllerCommandEvent::free_()
{
	FREE(m_cmd);
}

bool ControllerCommandEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	unsigned short len = BINARY_GET_DATA_S_INCR(p, unsigned short);
	n -= p - data;
	if (len > n) { return false;}
	
	m_cmd = Command::decode(p, len);
	return m_cmd? true: false;
}

#endif
