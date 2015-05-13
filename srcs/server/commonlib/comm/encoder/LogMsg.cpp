/* $Id: LogMsg.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "LogMsg.h"
#include "binary.h"

BEGIN_NS_COMMDATA();

char *CommLogMsgEncoder::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}
	
	{
		BINARY_SET_DATA_S_INCR(p, short, m_level);
		BINARY_SET_STRING_INCR(p, m_msg.c_str());
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
