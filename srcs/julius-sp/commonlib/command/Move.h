/*
 * Created by Okamoto on 2011-03-25
 */

#ifdef DEPRECATED

#ifndef Command_Move_h
#define Command_Move_h

#include "Command.h"
#include "Vector3d.h"

#ifdef IRWAS_SIMSERVER
class SSimObj;
#endif

class MoveCommand : public Command
{
private:
	Vector3d m_dest;
	double	m_vel;
public:
	MoveCommand(const char *target_, const Vector3d &dest, double vel)
		: Command(COMMAND_MOVE, target_), m_dest(dest), m_vel(vel)
	{
		
	}
public:
	const Vector3d & destination() { return m_dest; }
	double velocity() { return m_vel; }
private:
#ifdef IRWAS_SIMSERVER
	bool apply(double t, SSimObj &);
#endif
	char *toBinary(int &n);
	void dump();
};

#endif // Command_Move_h
 
#endif // DEPRECATED
