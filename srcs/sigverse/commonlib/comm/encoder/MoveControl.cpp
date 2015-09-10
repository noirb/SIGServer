/*
 * Written by okamoto 2011-12-28
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Change GetVelocity to GetLinearVelocity
 * Modified by Tetsunari Inamura on 2014-01-06
 *    Bug fixed on 
 * Added addTorque by Tetsunari Inamura on 2014-02-26
 */

#include <sigverse/commonlib/comm/encoder/MoveControl.h>
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/Logger.h>

BEGIN_NS_COMMDATA();

char *AddForceRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	{
		BINARY_SET_STRING_INCR(p, m_name);
	}
	BINARY_SET_DOUBLE_INCR(p, m_x);
	BINARY_SET_DOUBLE_INCR(p, m_y);
	BINARY_SET_DOUBLE_INCR(p, m_z);
	BINARY_SET_BOOL_INCR(p, m_relf);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *AddForceAtPosRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	{
		BINARY_SET_STRING_INCR(p, m_name);
	}
	BINARY_SET_DOUBLE_INCR(p, m_x);
	BINARY_SET_DOUBLE_INCR(p, m_y);
	BINARY_SET_DOUBLE_INCR(p, m_z);
	BINARY_SET_DOUBLE_INCR(p, m_px);
	BINARY_SET_DOUBLE_INCR(p, m_py);
	BINARY_SET_DOUBLE_INCR(p, m_pz);
	BINARY_SET_BOOL_INCR(p, m_rel);
	BINARY_SET_BOOL_INCR(p, m_relf);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *AddTorqueRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	{
		BINARY_SET_STRING_INCR(p, m_name);
	}
	BINARY_SET_DOUBLE_INCR(p, m_x);
	BINARY_SET_DOUBLE_INCR(p, m_y);
	BINARY_SET_DOUBLE_INCR(p, m_z);
	BINARY_SET_BOOL_INCR(p, m_relf);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *SetMassRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	{
		BINARY_SET_STRING_INCR(p, m_name);
	}
	BINARY_SET_DOUBLE_INCR(p, m_mass);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetAngularVelocityRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	{
		BINARY_SET_STRING_INCR(p, m_name);
	}

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetAngularVelocityResult::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	BINARY_SET_STRING_INCR(p, m_name);
	BINARY_SET_DOUBLE_INCR(p, m_x);
	BINARY_SET_DOUBLE_INCR(p, m_y);
	BINARY_SET_DOUBLE_INCR(p, m_z);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetLinearVelocityRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	{
		BINARY_SET_STRING_INCR(p, m_name);
	}

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetLinearVelocityResult::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	BINARY_SET_STRING_INCR(p, m_name);
	BINARY_SET_DOUBLE_INCR(p, m_x);
	BINARY_SET_DOUBLE_INCR(p, m_y);
	BINARY_SET_DOUBLE_INCR(p, m_z);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


// Added by Tetsunari Inamura on 2014-01-06
char *SetLinearVelocityRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	BINARY_SET_STRING_INCR(p, m_name);
	BINARY_SET_DOUBLE_INCR(p, m_x);
	BINARY_SET_DOUBLE_INCR(p, m_y);
	BINARY_SET_DOUBLE_INCR(p, m_z);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


char *AddForceToPartsRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	BINARY_SET_STRING_INCR(p, m_name);
	BINARY_SET_STRING_INCR(p, m_parts);

	BINARY_SET_DOUBLE_INCR(p, m_x);
	BINARY_SET_DOUBLE_INCR(p, m_y);
	BINARY_SET_DOUBLE_INCR(p, m_z);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *SetGravityModeRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	BINARY_SET_STRING_INCR(p, m_name);
	BINARY_SET_BOOL_INCR(p, m_gravity);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetGravityModeRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	BINARY_SET_STRING_INCR(p, m_name);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *GetGravityModeResult::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	BINARY_SET_STRING_INCR(p, m_name);
	BINARY_SET_BOOL_INCR(p, m_gravity);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char *SetDynamicsModeRequest::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);
	BINARY_SET_STRING_INCR(p, m_name);
	BINARY_SET_BOOL_INCR(p, m_dynamics);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


END_NS_COMMDATA();
