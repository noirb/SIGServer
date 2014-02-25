/* 
 * Written by okamoto on 2011-12-28
 * Modified by Tetsunari Inamura on 2013-12-30
 *    change GetVelocity to GetLinearVelocity
 *    no need to translate comments
 * Add SetLinearVelocity by Tetsunari Inamura on 2014-01-06
 */

#include "MoveControl.h"
#include "binary.h"
#include "Logger.h"

bool RequestAddForceEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	{
		char *name = BINARY_GET_STRING_INCR(p);
		m_name = name;
	}

	m_x    = BINARY_GET_DOUBLE_INCR(p);
	m_y    = BINARY_GET_DOUBLE_INCR(p);
	m_z    = BINARY_GET_DOUBLE_INCR(p);
	m_relf = BINARY_GET_BOOL_INCR(p);

	return true;
}


bool RequestAddForceAtPosEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	{
		char *name = BINARY_GET_STRING_INCR(p);
		m_name = name;
	}

	m_x    = BINARY_GET_DOUBLE_INCR(p);
	m_y    = BINARY_GET_DOUBLE_INCR(p);
	m_z    = BINARY_GET_DOUBLE_INCR(p);
	m_px   = BINARY_GET_DOUBLE_INCR(p);
	m_py   = BINARY_GET_DOUBLE_INCR(p);
	m_pz   = BINARY_GET_DOUBLE_INCR(p);
	m_rel  = BINARY_GET_BOOL_INCR(p);
	m_relf = BINARY_GET_BOOL_INCR(p);

	return true;
}

bool RequestSetMassEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_name = BINARY_GET_STRING_INCR(p);
	m_mass = BINARY_GET_DOUBLE_INCR(p);

	return true;
}

bool RequestGetAngularVelocityEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_name = BINARY_GET_STRING_INCR(p);

	return true;
}

bool ResultGetAngularVelocityEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_name = BINARY_GET_STRING_INCR(p);
	m_x    = BINARY_GET_DOUBLE_INCR(p);
	m_y    = BINARY_GET_DOUBLE_INCR(p);
	m_z    = BINARY_GET_DOUBLE_INCR(p);

	return true;
}


// Modified by Tetsunari Inamura: change GetVelocity to GetLinearVelocity
bool RequestGetLinearVelocityEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_name = BINARY_GET_STRING_INCR(p);

	return true;
}

// Modified by Tetsunari Inamura: change GetVelocity to GetLinearVelocity
bool ResultGetLinearVelocityEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_name = BINARY_GET_STRING_INCR(p);
	m_x    = BINARY_GET_DOUBLE_INCR(p);
	m_y    = BINARY_GET_DOUBLE_INCR(p);
	m_z    = BINARY_GET_DOUBLE_INCR(p);

	return true;
}


// Added by Tetsunari Inamura on 2014-01-06
bool RequestSetLinearVelocityEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	{
		char *name = BINARY_GET_STRING_INCR(p);
		m_name = name;
	}

	m_x    = BINARY_GET_DOUBLE_INCR(p);
	m_y    = BINARY_GET_DOUBLE_INCR(p);
	m_z    = BINARY_GET_DOUBLE_INCR(p);

	return true;
}




bool RequestAddForceToPartsEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	
	m_name  = BINARY_GET_STRING_INCR(p);
	m_parts = BINARY_GET_STRING_INCR(p);
	
	m_x     = BINARY_GET_DOUBLE_INCR(p);
	m_y     = BINARY_GET_DOUBLE_INCR(p);
	m_z     = BINARY_GET_DOUBLE_INCR(p);

	return true;
}

bool RequestSetGravityModeEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	
	m_name    = BINARY_GET_STRING_INCR(p);
	m_gravity = BINARY_GET_BOOL_INCR(p);

	return true;
}

bool RequestGetGravityModeEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	
	m_name = BINARY_GET_STRING_INCR(p);

	return true;
}

bool ResultGetGravityModeEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	
	m_name    = BINARY_GET_STRING_INCR(p);
	m_gravity = BINARY_GET_BOOL_INCR(p);

	return true;
}

bool RequestSetDynamicsModeEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	
	m_name     = BINARY_GET_STRING_INCR(p);
	m_dynamics = BINARY_GET_BOOL_INCR(p);

	return true;
}
