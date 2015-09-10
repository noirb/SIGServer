/* $Id: RequestCaptureViewImage.cpp,v 1.2 2011-08-26 05:24:08 okamoto Exp $ */
#include "RequestCaptureViewImage.h"
#include "binary.h"
#include "ViewImageInfo.h"
#include "Header.h"

#define FREE(P) if (P) { delete P; P = 0; }

void RequestCaptureViewImageEvent::free_()
{
	FREE(m_info);
}

bool RequestCaptureViewImageEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	char *name = BINARY_GET_STRING_INCR(p);
	m_agentName = name;
	BINARY_FREE_STRING(name);
	
	ImageDataType dataType = BINARY_GET_DATA_S_INCR(p, ImageDataType);
	ColorBitType cbType = BINARY_GET_DATA_S_INCR(p, ColorBitType);
	int w = BINARY_GET_DATA_S_INCR(p, unsigned short);
	int h = BINARY_GET_DATA_S_INCR(p, unsigned short);
	int id = BINARY_GET_DATA_S_INCR(p, short);
	m_id = id;

	bool b = CommData::isPacketEnd(p);
	if (b) {
		m_info = new ViewImageInfo(dataType, cbType, w, h);
	}
	return b;
}
