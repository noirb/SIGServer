/* $Id: Attrs.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "Attrs.h"
#include "binary.h"

BEGIN_NS_COMMDATA();

char *GetAttributesRequest::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	unsigned short attrNum = m_attrNames.size();
	BINARY_SET_DATA_S_INCR(p, unsigned short, attrNum);
	
	for(C::iterator i=m_attrNames.begin(); i!=m_attrNames.end(); i++) {
		const char *str = i->c_str();
		BINARY_SET_STRING_INCR(p, str);
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


/*
char * GetAttributesResult::encode(int seq, int &)
{

}
*/

END_NS_COMMDATA();
