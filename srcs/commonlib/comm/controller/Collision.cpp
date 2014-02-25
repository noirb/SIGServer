/* $Id: Collision.cpp,v 1.2 2012-03-19 06:35:40 okamoto Exp $ */ 
#include "Collision.h"
#include "binary.h"
#include "Logger.h"

bool CollisionEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	
	m_currTime = BINARY_GET_DOUBLE_INCR(p);

	int wn = BINARY_GET_DATA_S_INCR(p, unsigned short);
	for (int i=0; i<wn; i++) {
		char *with = BINARY_GET_STRING_INCR(p);
		if (with || strlen(with) <= 0) {
		  char *name = strtok(with,":");
		  char *withParts = strtok(NULL,":");
		  char *myParts = strtok(NULL,":");
		  m_with.push_back(name);
		  m_withParts.push_back(withParts);
		  m_myParts.push_back(myParts);
		}
		BINARY_FREE_STRING(with);
	}
	return true;
}

