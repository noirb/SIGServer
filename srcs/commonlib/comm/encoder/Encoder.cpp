/* $Id: Encoder.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "Encoder.h"
#include "CommDataDef.h"
#include "binary.h"
#include "SimWorld.h"
#include "SimObj.h"
#include "Source.h"
#include "CommUtil.h"
#include "Logger.h"

#include <string.h>

CommDataEncoder::SendProc * CommDataEncoder::s_sendProc = &CommUtil::sendData;

int CommDataEncoder::send(SOCKET sock)
{
	assert(s_sendProc);

	int sum = 0;
	const int N = packetNum();
	for (int i=0; i<N; i++) {
		int n;
		char *data = encode(i, n);
		int r = (*s_sendProc)(sock, data, n);
		if (r < 0) { return -1; }
		sum += r;
	}
	return sum;
}


char *CommDataEncoder::getHeader(int seq, int &n)
{
	static char buf[COMM_DATA_HEADER_MAX_SIZE];
	char *p = buf;
	BINARY_SET_DATA_S_INCR(p, unsigned short, COMM_DATA_PACKET_START_TOKEN);
	BINARY_SET_DATA_S_INCR(p, unsigned short, 0); // packet size
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_type);
	BINARY_SET_DATA_S_INCR(p, unsigned short, packetNum());
	BINARY_SET_DATA_S_INCR(p, unsigned short, seq);
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_forwardFlags);
	BINARY_SET_STRING_INCR(p, m_forwardTo.c_str());
	BINARY_SET_DOUBLE_INCR(p, m_forwardReachRadius);
		
	n = p - buf;
	return buf;
}

char *CommDataEncoder::getFooter(int &n)
{
	static char buf[COMM_DATA_FOOTER_SIZE];
	char *p = buf;
	BINARY_SET_DATA_S_INCR(p, unsigned short, COMM_DATA_PACKET_END_TOKEN);
	n = COMM_DATA_FOOTER_SIZE;
	return buf;
}

void CommDataEncoder::setPacketSize(char *packet, unsigned short n)
{
	char *p = packet + COMM_DATA_PACKET_SIZE_OFFSET;
	BINARY_SET_DATA_S_INCR(p, unsigned short, n);
}

void CommDataEncoder::setForwardTo(const char *to, bool returnImmediate, double reachRadius)
{
	m_forwardFlags = COMM_DATA_FORWARD;
	if (returnImmediate) {
		m_forwardFlags |= COMM_DATA_FORWARD_RETURN_IMMEDIATE;
	}
	if (to) {
		m_forwardTo = to;
	}
	if (reachRadius >= 0) {
		m_forwardFlags |= COMM_DATA_FORWARD_REACH_RADIUS;
		m_forwardReachRadius = reachRadius;
	}
}

char *  CommDataEncoder::setHeader(char *p, int seq)
{
	int n;
	char *h = getHeader(seq, n);
	memcpy(p, h, n);
	return p + n;
}

char *  CommDataEncoder::setFooter(char *p)
{
	int n;
	char *f = getFooter(n);
	memcpy(p, f, n);
	return p + n;
}



BEGIN_NS_COMMDATA();


char *NoDataEncoder::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;

	char *h = getHeader(seq, n);
	memcpy(p, h, n);
	p += n;

	char *f = getFooter(n);
	memcpy(p, f, n);
	p += n;

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

int RawDataEncoder::packetNum() {
	if (m_packetNum < 0) {
		int datasize = getDataLen();
		m_packetNum = 1 + datasize/UNIT_DATA_SIZE + ((datasize%UNIT_DATA_SIZE)? 1: 0);
	} 
	return m_packetNum;	
}

char *RawDataEncoder::encode(int seq, int &sz)
{
	char *p = m_buf;
	{
		int n;
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	if (seq == 0) {
		int n;
		char *pp = getDataHeader(n);
		memcpy(p, pp, n);
		p += n;
	} else {
		// Image data
		int N = packetNum();
		int datalen = 0;
		if (seq + 1 < N) { 
			datalen = UNIT_DATA_SIZE;
		} else { // last packet
			int datasize = getDataLen();
			int left = datasize % UNIT_DATA_SIZE;
			datalen = left != 0? left: UNIT_DATA_SIZE;
		}
		BINARY_SET_DATA_S_INCR(p, unsigned short, datalen);

		char *data = getData();
		char *pp = data + UNIT_DATA_SIZE*(seq-1);
		memcpy(p, pp, datalen);
		p += datalen;
	}

	{
		int n;
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	setPacketSize(m_buf, sz);

	return m_buf;
}


END_NS_COMMDATA();
