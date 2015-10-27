/*
 * Modified by okamoto on 2012-03-27
 */

#include <sigverse/commonlib/comm/encoder/DetectEntities.h>
#include <sigverse/commonlib/comm/Header.h>
#include <sigverse/commonlib/binary.h>
#include <assert.h>

BEGIN_NS_COMMDATA();

char * DetectEntitiesRequest::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_STRING_INCR(p, m_name.c_str());
	BINARY_SET_DATA_S_INCR(p, short, m_id);
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

char * DetectEntitiesResult::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	char *pCnt = p;
	BINARY_SET_DATA_S_INCR(p, short, 0);

	short cnt = 0;
	// modified by noma@tome (2012/02/20)
	for (std::vector<std::string>::iterator i=m_detected.begin(); i!=m_detected.end(); i++) {
		std::string name = *i;
		
		int left = BUFSIZE - (p-m_buf + name.length()+2);
		if (left <= COMM_DATA_FOOTER_SIZE) {
			break;
		}
		BINARY_SET_STRING_INCR(p, name.c_str());
		cnt++;
	}
/*Original*/
//	for (C::iterator i=m_detected.begin(); i!=m_detected.end(); i++) {
//		S name = *i;
//		BINARY_SET_STRING_INCR(p, name.c_str());
//
//		int left = BUFSIZE - (p-m_buf);
//		if (left <= COMM_DATA_FOOTER_SIZE) {
//			break;
//		}
//		cnt++;
//	}

	BINARY_SET_DATA_S(pCnt, short, cnt);
	
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

