#include <sigverse/commonlib/comm/encoder/DistanceSensor.h>
#include <sigverse/commonlib/binary.h>
#include "assert.h"
#include <sigverse/commonlib/comm/Header.h>

#ifdef WIN32
#ifdef SIGVERSE_OGRE_CLIENT
#include "SgvLog.h"
#endif // SIGVERSE_OGRE_CLIENT

#ifdef IRWAS_OGRE_CLIENT
#include "IrcApp.h"
#endif // IRWAS_OGRE_CLIENT
#endif // WIN32

BEGIN_NS_COMMDATA();

char *DistanceSensorRequest::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_STRING_INCR(p, m_agentName.c_str());
	BINARY_SET_DOUBLE_INCR(p, m_start);
	BINARY_SET_DOUBLE_INCR(p, m_end);
	BINARY_SET_DATA_S_INCR(p, short, m_id);
	BINARY_SET_DATA_S_INCR(p, short, m_dim);
	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	assert(sz <= BUFSIZE);
	setPacketSize(m_buf, sz);
	return m_buf;
}

char * DistanceSensorResult::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;

	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}
	BINARY_SET_DATA_S_INCR(p, short, m_distance);

	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	assert(sz <= BUFSIZE);
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();

