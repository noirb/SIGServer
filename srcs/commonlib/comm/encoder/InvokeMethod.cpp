/*
 * Created by okamoto on 2011-03-25
 */

#include "InvokeMethod.h"
#include "binary.h"
#include "RawSound.h"
#include <assert.h>

BEGIN_NS_COMMDATA();

char *InvokeOnAction::encode(int seq, int &sz)
{
	int n;

	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_DOUBLE_INCR(p, m_time);

	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *InvokeOnRecvText::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}
	
	{
		BINARY_SET_DOUBLE_INCR(p, m_time);
		BINARY_SET_STRING_INCR(p, m_caller.c_str());
		BINARY_SET_STRING_INCR(p, m_target.c_str());
		BINARY_SET_STRING_INCR(p, m_text.c_str());
		BINARY_SET_DATA_S_INCR(p, Encode, m_encode);
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

char *InvokeOnRecvMessage::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	BINARY_SET_STRING_INCR(p, m_from.c_str());
	
	const unsigned int n = m_msgs.size();
	BINARY_SET_DATA_L_INCR(p, unsigned int, n);

	for (unsigned int i=0; i<n; i++) {
		const char *s = m_msgs[i].c_str();
		BINARY_SET_STRING_INCR(p, s);
	}
	
	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *InvokeOnCollision::encode(int seq, int &sz)
{
	int n;

	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_DOUBLE_INCR(p, m_time);
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_with.size());
	
	for (C::const_iterator i=m_with.begin(); i!=m_with.end(); i++) {
		BINARY_SET_STRING_INCR(p, i->c_str());
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
