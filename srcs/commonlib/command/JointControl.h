/* $Id: JointControl.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifdef DEPRECATED
#ifndef Command_JointControl_h
#define Command_JointControl_h

#include "Command.h"

#include <string>
#include <map>

#ifdef IRWAS_SIMSERVER
class SSimObj;
#endif

class JointControlCommand : public Command
{
private:
	typedef std::string S;
	typedef std::map<S, double> M;
private:
	M	m_values;
public:
	JointControlCommand(const char *target_)
		: Command(COMMAND_JOINT_CONTROL, target_) {;}
		
public:
	void	set(const char *name, double v)
	{
		m_values[name] = v;
	}
#ifdef IRWAS_SIMSERVER
	bool apply(double t, SSimObj &);
#endif
	char *toBinary(int &n);
	void dump();
};

#endif // Command_JointControl_h

#endif // DEPRECATED
 
