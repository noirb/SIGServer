/* $Id: Header.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "Header.h"
#include "binary.h"
#include "CommDataDef.h"

namespace CommData {

	int  getHeader(char *data, int n, CommDataHeader &h)
	{
		char *p = data;

		unsigned short token = BINARY_GET_DATA_S_INCR(p, unsigned short);
		if (token != COMM_DATA_PACKET_START_TOKEN) {
			return -1;
		}

		h.packetSize = BINARY_GET_DATA_S_INCR(p, unsigned short);
		if (h.packetSize < COMM_DATA_HEADER_MIN_SIZE) {
			return 0;
		}
	
		h.type = COMM_DATA_TYPE_NOT_SET;
		CommDataType t = BINARY_GET_DATA_S_INCR(p, CommDataType);
		if (COMM_DATA_TYPE_NOT_SET < t && t < COMM_DATA_TYPE_NUM) {
			h.type = t;
		} else {
			return 0;
		}
	
		h.packetNum = BINARY_GET_DATA_S_INCR(p, unsigned short);
		h.seq = BINARY_GET_DATA_S_INCR(p, unsigned short);
		h.forwardFlags = BINARY_GET_DATA_S_INCR(p, unsigned short);
		char *to = BINARY_GET_STRING_INCR(p);
		if (to) {
			h.forwardTo = to;
		}
		h.forwardReachRadius = BINARY_GET_DOUBLE_INCR(p);

		BINARY_FREE_STRING(to);

		return p - data;
	}

	bool isPacketEnd(char *data)
	{
		char *p = data;
		unsigned short token = BINARY_GET_DATA_S_INCR(p, unsigned short);
		return token == COMM_DATA_PACKET_END_TOKEN? true: false;
	}
}

