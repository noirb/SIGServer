/*
 * Written by kawamoto on 2011-04-18
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change ObjectVelocityData to AngularVelocityToPartsData
 *    Change JointVelocityData  to AngularVelocityToJointData
 */

#ifndef JointTorqueData_h
#define JointTorqueData_h

#ifndef JointVelocityData_h
#define JointVelocityData_h

#ifndef GetJointAngleData_h
#define GetJointAngleData_h

#ifndef ResultGetJointAngleData_h
#define ResultGetJointAngleData_h

#include <string>



class JointTorqueData
{
private:
	typedef std::string S;
private:
	S       m_agentName;
	S       m_jointName;
	double  m_torque;
public:
	JointTorqueData(const char *agentName_, const char *jointName_, double t_) : m_agentName(agentName_), m_jointName(jointName_), m_torque(t_) {}
	JointTorqueData() : m_torque(0.0) {}

	void          agentName(const char *n) { m_agentName = n; }
	const char *  agentName()              { return m_agentName.c_str(); }

	void          jointName(const char *n) { m_jointName = n; }
	const char *  jointName()              { return m_jointName.c_str(); }

	void          torque(double v)         { m_torque = v; }
	double        torque()                 { return m_torque; }
};


// Modified by inamura on 2013-12-30: changed from ObjectVelocityData
class AngularVelocityToPartsData
{
private:
	typedef std::string S;
private:
	S       m_agentName;
	S       m_objectName;
	double	m_velocity;
	double	m_max;
	double  x_Axis;
	double  y_Axis;
	double  z_Axis;
public:
	AngularVelocityToPartsData(
		const char *agentName_,
		const char *jointName_,
		double v_,
		double max_/*,
		double x_axis,
		double y_axis,
		double z_axis*/ ) : m_agentName(agentName_), m_objectName(jointName_), m_velocity(v_), m_max(max_) /*, x_Axis(x_axis) , y_Axis(y_axis) ,z_Axis(z_axis)*/ {}

	AngularVelocityToPartsData() : m_velocity(0.0), m_max(0.0) {}

	void          agentName(const char *n)  { m_agentName = n; }
	const char *  agentName()               { return m_agentName.c_str(); }

	void          objectName(const char *n) { m_objectName = n; }
	const char *  objectName()              { return m_objectName.c_str(); }

	void          angular_velocity(double v){ m_velocity = v; }
	double        angular_velocity()        { return m_velocity; }

	void          maxValue(double m)        { m_max = m; }
	double        maxValue()                { return m_max; }

/*
	void setXAxis(double x) { x_axis = x; }
	void setYAxis(double y) { y_axis = y; }
	void setZAxis(double z) { z_axis = z; }
*/
};


// Modified by inamura on 2013-12-30: changed from JointVelocityData
class AngularVelocityToJointData
{
private:
	typedef std::string S;
private:
	S       m_agentName;
	S       m_jointName;
	double  m_velocity;  // angular velocity of the joint
	double  m_max;       // be careful: it is max torque, but confusing with max angular velocity
public:
	AngularVelocityToJointData(const char *agentName_, const char *jointName_, double v_, double max_) : m_agentName(agentName_), m_jointName(jointName_), m_velocity(v_), m_max(max_) {}
	AngularVelocityToJointData() : m_velocity(0.0), m_max(0.0) {}

	void         agentName(const char *n)  { m_agentName = n; }
	const char * agentName()               { return m_agentName.c_str(); }

	void         jointName(const char *n)  { m_jointName = n; }
	const char * jointName()               { return m_jointName.c_str(); }
	

	void         angular_velocity(double v){ m_velocity = v; }
	double       angular_velocity()        { return m_velocity; }

	void         maxValue(double m)        { m_max = m; }
	double       maxValue()                { return m_max; }
};


class GetJointAngleData
{
 private:
  typedef std::string S;
 private:
  S	m_agentName;
  S	m_jointName;
 public:
  GetJointAngleData(const char *agentName_, const char *jointName_ ) : m_agentName(agentName_), m_jointName(jointName_) {}
  GetJointAngleData() {}
  void		agentName(const char *n) { m_agentName = n; }
  const char *	agentName() { return m_agentName.c_str(); }
  
  void		jointName(const char *n) { m_jointName = n; }
  const char *	jointName() { return m_jointName.c_str(); }
  
};

class ResultGetJointAngleData
{
 private:
  double      m_angle;
 public:
  ResultGetJointAngleData(double angle_) : m_angle(angle_) {}
  ResultGetJointAngleData() {}

  void		angle(double a) { m_angle = a; }
  double        angle() { return m_angle; }
  
};


#endif // JointTorqueData_h
#endif // JointVelocityData_h
#endif // GetJointAngleData_h
#endif // ResultGetJointAngleData_h
 
