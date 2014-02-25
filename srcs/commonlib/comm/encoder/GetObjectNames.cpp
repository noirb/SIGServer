/* $Id: GetObjectNames.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "GetObjectNames.h"
#include "binary.h"

BEGIN_NS_COMMDATA();


char *	GetObjectNamesRequest::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_DATA_S_INCR(p, unsigned short, m_type);

	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

int 	GetObjectNamesResult::packetNum()
{
	return m_names.size()/50+1;
}

inline int MIN(int v1, int v2)
{
	return v1<v2? v1:v2;
}

char *	GetObjectNamesResult::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	{
		unsigned short N = m_names.size();
		int si = seq * UNIT;
		int ei = MIN((seq+1)*UNIT, N);
		int n_ = ei-si;
		BINARY_SET_DATA_S_INCR(p, unsigned short, n_);

		for (int i=si; i<ei; i++) {
			S s = m_names[i];
			BINARY_SET_STRING_INCR(p, s.c_str());
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

