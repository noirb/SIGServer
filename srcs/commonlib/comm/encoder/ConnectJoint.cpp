/* $Id: ConnectJoint.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "comm/encoder/ConnectJoint.h"
#include "binary.h"

BEGIN_NS_COMMDATA()

char *	RequestConnectJoint::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	BINARY_SET_STRING_INCR(p, m_data.jointName.c_str());
	BINARY_SET_STRING_INCR(p, m_data.agentName.c_str());
	BINARY_SET_STRING_INCR(p, m_data.agentParts.c_str());
	BINARY_SET_STRING_INCR(p, m_data.targetName.c_str());
	BINARY_SET_STRING_INCR(p, m_data.targetParts.c_str());

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;

}

END_NS_COMMDATA();

