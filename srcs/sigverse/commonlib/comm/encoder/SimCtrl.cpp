/*
 * Created by okamoto on 2011-03-25
 */

#include <sigverse/commonlib/comm/encoder/SimCtrl.h>
#include <sigverse/commonlib/binary.h>

BEGIN_NS_COMMDATA();

char *SimCtrlRequest::encode(int seq, int &sz) 
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}
	BINARY_SET_DATA_S_INCR(p, SimCtrlCmdType, m_command);

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
