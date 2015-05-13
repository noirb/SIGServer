/*
 * Created by Okamoto on 2011-03-25
 */

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
	std::map<std::string, double> m_values;
public:
	JointControlCommand(const char *target_)
		: Command(COMMAND_JOINT_CONTROL, target_) {;}
		
public:
	void set(const char *name, double v)
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
 
