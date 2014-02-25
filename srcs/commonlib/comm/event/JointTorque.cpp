/*
 * Written by kawamoto on 2011-04-08
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change RequestSetJointVelocityEvent  to RequestSetAngularVelocityToJointEvent
 *    Change RequestSetOjbectVelocityEvent to RequestSetAngularVelocityToPartsEvent
 */

#include "JointTorque.h"
#include "binary.h"

bool RequestAddJointTorqueEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.agentName(name);
		}
		BINARY_FREE_STRING(name);
	}

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.jointName(name);
		}
		BINARY_FREE_STRING(name);
	}
	double torque = BINARY_GET_DOUBLE_INCR(p);
	m_data.torque(torque);
	return true;
}


// modified by inamura on 2013-12-30
bool RequestSetAngularVelocityToJointEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.agentName(name);
		}
		BINARY_FREE_STRING(name);
	}
	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.jointName(name);
	}
	BINARY_FREE_STRING(name);
	}
	double velocity = BINARY_GET_DOUBLE_INCR(p);
	m_data.angular_velocity(velocity);

	double max_ = BINARY_GET_DOUBLE_INCR(p);
	m_data.maxValue(max_);
	return true;
}


// modified by inamura on 2013-12-30
bool RequestSetAngularVelocityToPartsEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.agentName(name);
		}
		BINARY_FREE_STRING(name);
	}
	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.objectName(name);
		}
		BINARY_FREE_STRING(name);
	}

	double velocity = BINARY_GET_DOUBLE_INCR(p);
	m_data.angular_velocity(velocity);

	double max_ = BINARY_GET_DOUBLE_INCR(p);
	m_data.maxValue(max_);

	//double xAx = BINARY_GET_DOUBLE_INCR(p);
	//double yAx = BINARY_GET_DOUBLE_INCR(p);
	//double zAx = BINARY_GET_DOUBLE_INCR(p);

	return true;
}


bool RequestGetJointAngleEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.agentName(name);
		}
		BINARY_FREE_STRING(name);
	}

	{
		char *name = BINARY_GET_STRING_INCR(p);
		if (name != NULL) {
			m_data.jointName(name);
		}
		BINARY_FREE_STRING(name);
	}
	return true;
}


bool ResultGetJointAngleEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	double angle = BINARY_GET_DOUBLE_INCR(p);
	m_data.angle(angle);
	return true;
}
