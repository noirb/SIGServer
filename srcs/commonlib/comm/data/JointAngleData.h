/* $Id: JointAngleData.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef JointAngleData_h
#define JointAngleData_h

#include <string>

class JointAngleData
{
private:
	typedef std::string S;
private:
	S	m_agentName;
	S	m_jointName;
	double	m_angle;
public:
	JointAngleData(const char *agentName_, const char *jointName_, double angle_) : m_agentName(agentName_), m_jointName(jointName_), m_angle(angle_) {}
	JointAngleData() : m_angle(0.0) {}

	void		agentName(const char *n) { m_agentName = n; }
	const char *	agentName() { return m_agentName.c_str(); }

	void		jointName(const char *n) { m_jointName = n; }
	const char *	jointName() { return m_jointName.c_str(); }
	

	void	angle(double v) { m_angle = v; }
	double	angle() { return m_angle; }
};


#endif // JointAngleData_h
 
