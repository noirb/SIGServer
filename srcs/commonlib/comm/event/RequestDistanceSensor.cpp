/* $Id: RequestDistanceSensor.cpp,v 1.3 2011-09-22 06:39:25 okamoto Exp $ */
#include "RequestDistanceSensor.h"
#include "binary.h"
#include "ViewImageInfo.h"
#include "Header.h"

#define FREE(P) if (P) { delete P; P = 0; }

bool RequestDistanceSensorEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	char *name = BINARY_GET_STRING_INCR(p);
	m_agentName = name;
	BINARY_FREE_STRING(name);

	double start = BINARY_GET_DOUBLE_INCR(p);	
	double end = BINARY_GET_DOUBLE_INCR(p);	
	int id = BINARY_GET_DATA_S_INCR(p, short);
	int dim = BINARY_GET_DATA_S_INCR(p, short);
	m_start = start;
	m_end = end;
	m_id = id;
	m_dim = dim;

	return true;
}
