/* $Id: ReleaseJoint.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/encoder/ReleaseJoint.h>
#include <sigverse/commonlib/binary.h>

BEGIN_NS_COMMDATA()

char * RequestReleaseJoint::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	BINARY_SET_STRING_INCR(p, m_agentName.c_str());
	BINARY_SET_STRING_INCR(p, m_jointName.c_str());

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();

