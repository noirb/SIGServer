/*
 * Created by okamoto on 2011-03-25
 */

#include "JointAngle.h"
#include "binary.h"

BEGIN_NS_COMMDATA();

char *SetJointAngleRequest::encode(int seq, int &sz)
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

	double angle = m_data.angle();
	BINARY_SET_DOUBLE_INCR(p, angle);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();
