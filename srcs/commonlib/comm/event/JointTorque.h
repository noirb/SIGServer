/*
 * Written by okamoto on 2011-12-21
 * Modified by Tetsunari Inamura on 2013-12-30
 *    change getVelocity to getAngularVelocity
 *    change getMax      to getMaxTorque
 *    change RequestSetJointVelocityEvent  to RequestSetAngularVelocityToJointEvent 
 *    change RequestSetObjectVelocityEvent to RequestSetAngularVelocityToPartsEvent 
 */

#ifndef JointTorque_h
#define JointTorque_h

#include "comm/data/JointTorqueData.h"

class RequestAddJointTorqueEvent 
{
private:
	JointTorqueData m_data;
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	const char * getAgentName() { return m_data.agentName(); }
	const char * getJointName() { return m_data.jointName(); }
	double getTorque(){ return  m_data.torque();}
};


class RequestSetAngularVelocityToJointEvent 
{
private:
	AngularVelocityToJointData m_data;
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	const char * getAgentName() { return m_data.agentName(); }
	const char * getJointName() { return m_data.jointName(); }
	double getAngularVelocity() { return m_data.angular_velocity();}  //modified by inamura on 2013-12-30
	double getMaxTorque()       { return m_data.maxValue();}          //modified by inamura on 2013-12-30
};



class RequestSetAngularVelocityToPartsEvent 
{
private:
	AngularVelocityToPartsData m_data;
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	const char * getAgentName() { return m_data.agentName(); }
	const char * getPartsName() { return m_data.objectName(); }
	double getAngularVelocity() { return  m_data.angular_velocity();} //modified by inamura on 2013-12-30
	double getMaxTorque()       { return  m_data.maxValue();}         //modified by inamura on 2013-12-30
};


class RequestGetJointAngleEvent
{
 private:
  GetJointAngleData m_data;
 public:
  bool	set(int packetNum, int seq, char *data, int n);
 public:
  const char * getAgentName() { return m_data.agentName(); }
  const char * getJointName() { return m_data.jointName(); }
};

class ResultGetJointAngleEvent
{
 private:
  ResultGetJointAngleData m_data;
 public:
  bool	set(int packetNum, int seq, char *data, int n);
 public:
  double getAngle() { return m_data.angle(); }


};

#endif // JointTorque_h


