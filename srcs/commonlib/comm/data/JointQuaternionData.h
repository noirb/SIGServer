
#ifndef JointQuaternionData_h
#define JointQuaternionData_h


#include <string>
class JointQuaternionData
{
private:
	typedef std::string S;
private:
	S	m_agentName;
	S	m_jointName;
	double	m_qw;
	double	m_qx;
	double	m_qy;
	double	m_qz;
	bool    m_offset;
public:
 JointQuaternionData(const char *agentName_, const char *jointName_, double qw_, double qx_, double qy_, double qz_, bool b_) : m_agentName(agentName_), m_jointName(jointName_), m_qw(qw_), m_qx(qx_), m_qy(qy_), m_qz(qz_), m_offset(b_)  {}
 JointQuaternionData() : m_qw(0.0), m_qx(0.0), m_qy(0.0), m_qz(0.0) {}

	void		agentName(const char *n) { m_agentName = n; }
	const char *	agentName() { return m_agentName.c_str(); }

	void		jointName(const char *n) { m_jointName = n; }
	const char *	jointName() { return m_jointName.c_str(); }
	

	void	qw(double v) { m_qw = v; }
	double	qw() { return m_qw; }

	void	qx(double v) { m_qx = v; }
	double	qx() { return m_qx; }

	void	qy(double v) { m_qy = v; }
	double	qy() { return m_qy; }

	void	qz(double v) { m_qz = v; }
	double	qz() { return m_qz; }

	void            offset(bool b) { m_offset = b; }
	const bool      offset() { 
	  return m_offset; }
};

#endif
