/*
 * Modified by okamoto on 2011-08-10
 */

#include <sigverse/commonlib/comm/encoder/DisplayText.h>
#include <sigverse/commonlib/binary.h>

BEGIN_NS_COMMDATA();

char *DisplayText::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}
	{
		BINARY_SET_DATA_S_INCR(p, short, m_fs);
		BINARY_SET_DATA_S_INCR(p, short, m_color);
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
