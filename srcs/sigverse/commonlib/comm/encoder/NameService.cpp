/* $Id: NameService.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <sigverse/commonlib/comm/encoder/NameService.h>
#include <sigverse/commonlib/binary.h>

BEGIN_NS_COMMDATA();


char *NSQueryRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	unsigned short k = m_kind;
	BINARY_SET_DATA_S_INCR(p, unsigned short, k);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


char * NSQueryResult::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	unsigned short found = m_service? 1: 0;
	BINARY_SET_DATA_S_INCR(p, unsigned short, found);
	if (found) {

		const char *n = m_service->name();
		BINARY_SET_STRING_INCR(p, n);
		
		unsigned short k = m_service->kind();
		BINARY_SET_DATA_S_INCR(p, unsigned short, k);

		const char *h = m_service->hostname();
		BINARY_SET_STRING_INCR(p, h);

		int port = m_service->port();
		BINARY_SET_DATA_L_INCR(p, int, port);
	}

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


char *NSPingerRequest::encode(int seq, int &sz)
{
	char *p = m_buf;
	p = setHeader(p, seq);

	BINARY_SET_DATA_L_INCR(p, Service::Kind, m_kind);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


char * NSPingerResult::encode(int seq, int &sz)
{
	char *p = m_buf;
	p = setHeader(p, seq);

	unsigned short v = m_active? 1: 0;
	BINARY_SET_DATA_S_INCR(p, unsigned short, v);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();

