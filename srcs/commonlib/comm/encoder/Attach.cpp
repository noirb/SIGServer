/*
 * Created by okamoto on 2011-03-25
 */

#include "Attach.h"
#include "binary.h"
#include "Service.h"
#include "X3DDB.h"
#include <assert.h>

BEGIN_NS_COMMDATA();

char *AttachRequest::encode(int seq, int &sz)
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
		int r = moreData(p);
		p += r;
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

int AttachViewRequest::moreData(char *buf)
{
	char *p = buf;
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_polling?1:0);

	assert(p-buf == 2);
	return p-buf;
}

char *AttachViewResult::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	BINARY_SET_DATA_S_INCR(p, CommResultType, m_result);
	BINARY_SET_STRING_INCR(p, m_msg.c_str());

	if (m_result == COMM_RESULT_OK) {
		short num = m_db.size();
		BINARY_SET_DATA_S_INCR(p, short, num);
		X3DDB::Iterator *itr = m_db.getIterator();
		while (true) {
			const char *k = itr->nextKey();
			if (!k) { break; }
			const char *v = m_db.get(k);
			BINARY_SET_STRING_INCR(p, k);
			BINARY_SET_STRING_INCR(p, v);
		}
		delete itr;
	}

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}



char *ProvideServiceRequest::encode(int seq, int &sz)
{
	char *p = m_buf;
	p = setHeader(p, seq);

	typedef Service::Kind Kind;
	const char *servicename = m_service.name();
	Kind kind = m_service.kind();
	const char *hostname = m_service.hostname();
	int port = m_service.port();
	
	BINARY_SET_STRING_INCR(p, servicename);
	BINARY_SET_DATA_L_INCR(p, Kind, kind);
	BINARY_SET_STRING_INCR(p, hostname);
	BINARY_SET_DATA_L_INCR(p, int, port);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;

}

END_NS_COMMDATA();

