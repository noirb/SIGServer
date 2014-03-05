/*
 * Created by okamoto on 2011-03-25
 */

#include "JointQuaternion.h"
#include "binary.h"

BEGIN_NS_COMMDATA();

char *SetJointQuaternionRequest::encode(int seq, int &sz)
{
	char *p = m_buf;
	p = setHeader(p, seq);
	{
		const char *name = m_data.agentName();
		BINARY_SET_STRING_INCR(p, name);
	}
	{
		const char *name = m_data.jointName();
		BINARY_SET_STRING_INCR(p, name);
	}
	double qw = m_data.qw();
	BINARY_SET_DOUBLE_INCR(p, qw);

	double qx = m_data.qx();
	BINARY_SET_DOUBLE_INCR(p, qx);

	double qy = m_data.qy();
	BINARY_SET_DOUBLE_INCR(p, qy);

	double qz = m_data.qz();
	BINARY_SET_DOUBLE_INCR(p, qz);

	bool offset = m_data.offset();
	BINARY_SET_BOOL_INCR(p, offset);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();
