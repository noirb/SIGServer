/*
 * Written by kawamoto on 2011-04-08
 * Modified by Tetsunari Inamura on 2013-12-30
 *    change JointVelocity  to AngularVelocityToJoint
 *    change ObjectVelocity to AngularVelocityToParts
 */

#include "JointTorque.h"
#include "binary.h"

BEGIN_NS_COMMDATA();

char *AddJointTorqueRequest::encode(int seq, int &sz)
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

	double torque = m_data.torque();
	BINARY_SET_DOUBLE_INCR(p, torque);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

// modified by inamura on 2013-12-30
char *SetAngularVelocityToJointRequest::encode(int seq, int &sz)
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
	double velocity = m_data.angular_velocity();
	BINARY_SET_DOUBLE_INCR(p, velocity);
	
	double max_ = m_data.maxValue();
	BINARY_SET_DOUBLE_INCR(p, max_);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


// modified by inamura on 2013-12-30
char *SetAngularVelocityToPartsRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	{
		const char *name = m_data.agentName();
		BINARY_SET_STRING_INCR(p, name);
	}
	{
		const char *name = m_data.objectName();
		BINARY_SET_STRING_INCR(p, name);
	}
	double velocity = m_data.angular_velocity();
	BINARY_SET_DOUBLE_INCR(p, velocity);

	double max_ = m_data.maxValue();
	BINARY_SET_DOUBLE_INCR(p, max_);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


char *GetJointAngleRequest::encode(int seq, int &sz)
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

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetJointAngleResult::encode(int seq, int &sz)
{
	char *p = m_buf;
	p = setHeader(p, seq);

	double angle = m_data.angle();
	BINARY_SET_DOUBLE_INCR(p, angle);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();
