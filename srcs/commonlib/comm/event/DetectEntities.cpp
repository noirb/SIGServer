/* $Id: DetectEntities.cpp,v 1.2 2011-08-15 05:34:46 okamoto Exp $ */
#include "DetectEntities.h"
#include "binary.h"

bool RequestDetectEntitiesEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	char *name = BINARY_GET_STRING_INCR(p);
	if (!name) { return false; }
	m_agentName = name;
	int id = BINARY_GET_DATA_S_INCR(p, short);
	m_id = id;
	BINARY_FREE_STRING(name);

	return true;
}


bool ResultDetectEntitiesEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	const short N = BINARY_GET_DATA_S_INCR(p, short);
	
	for (int i=0; i<(int)N; i++) {
		int left = n - ( p - data);
		if (left <= 0) { return false; }
		char *s = BINARY_GET_STRING_INCR(p);
		if (s) {
			m_detected.push_back(s);
		}
		BINARY_FREE_STRING(s);
	}
	return true;
}

