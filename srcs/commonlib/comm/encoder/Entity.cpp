/* $Id: Entity.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "Entity.h"
#include "binary.h"
#include "SimObjBase.h"

BEGIN_NS_COMMDATA();

char *GetEntityRequest::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_STRING_INCR(p, m_name.c_str());

	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetEntityResult::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	{
		CommResultType result = m_obj? COMM_RESULT_OK: COMM_RESULT_ERROR;
		BINARY_SET_DATA_S_INCR(p, CommResultType, result);
		BINARY_SET_DOUBLE_INCR(p, m_time);
		unsigned short exist = m_obj != NULL? 1: 0;
		BINARY_SET_DATA_S_INCR(p, unsigned short, exist);
		
		if (m_obj) {
			char *pp = m_obj->toBinary(n);

			while (p - m_buf + n > m_bufsize) {
				p = bufresize(RESIZE, p);
			}
			memcpy(p, pp, n);
			p += n;
		}
	}

	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();
