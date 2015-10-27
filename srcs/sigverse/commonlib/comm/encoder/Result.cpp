/* $Id: Result.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/encoder/Result.h>
#include <sigverse/commonlib/binary.h>

BEGIN_NS_COMMDATA();

char *CommResultEncoder::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_DATA_S_INCR(p, CommResultType, m_result);
	BINARY_SET_STRING_INCR(p, m_msg.c_str());

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

