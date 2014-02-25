/* $Id: Attrs.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "Attrs.h"
#include "binary.h"

bool RequestGetAttributesEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	unsigned short attrNum = BINARY_GET_DATA_S_INCR(p, unsigned short);
	for (int i=0; i<attrNum; i++) {
		char *str = BINARY_GET_STRING_INCR(p);
		if (str) {
			m_attrNames.push_back(str);
		}
		BINARY_FREE_STRING(str);
	}
	return true;
}
