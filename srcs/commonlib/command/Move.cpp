/* $Id: Move.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */

#ifdef DEPRECATED

#include "Move.h"
#include "binary.h"

char *MoveCommand::toBinary(int &n)
{
	static char buf[1024];

	char *p = buf;

	BINARY_SET_DATA_S_INCR(p, CommandType, m_type);
	BINARY_SET_STRING_INCR(p, target());
	BINARY_SET_DOUBLE_INCR(p, m_dest.x());
	BINARY_SET_DOUBLE_INCR(p, m_dest.y());
	BINARY_SET_DOUBLE_INCR(p, m_dest.z());
	BINARY_SET_DOUBLE_INCR(p, m_vel);
	
	n = p - buf;
	return buf;
}


void MoveCommand::dump()
{
#ifdef _DEBUG
	printf("type : %d\n", m_type);
	printf("target agent name : \"%s\"\n", target());
	printf("dest = (%f, %f, %f)\n", m_dest.x(), m_dest.y(), m_dest.z());
	printf("velocity = %f\n", m_vel);
	printf("\n");
#endif
}

#endif
