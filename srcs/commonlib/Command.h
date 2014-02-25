/* $Id: Command.h,v 1.2 2011-03-31 08:15:56 okamoto Exp $ */ 
#ifdef DEPRECATED

#ifndef Command_h
#define Command_h


#include <string>

enum {
	COMMAND_TYPE_NOT_SET = -1,
	COMMAND_MOVE,
	COMMAND_JOINT_CONTROL,
	COMMAND_CANCEL_COMMAND,
	COMMAND_TYPE_NUM,
};

typedef short CommandType;

class SSimObj;

class Command
{
protected:
	CommandType	m_type;
	std::string	m_target;
	double		m_startTime;
protected:
	Command(CommandType t, const char *target_) : m_type(t), m_target(target_), m_startTime(-1.0) {;}
public:
	virtual ~Command() {;}
public:
	const char *target() const { return m_target.c_str(); }
	
	virtual char *toBinary(int &n) = 0;
#ifdef IRWAS_SIMSERVER
	virtual bool apply(double t, SSimObj &) = 0;
#endif

#ifdef _DEBUG
	virtual void dump() = 0;
#endif
	
public:
	static Command *decode(char *buf, int size);
};

#endif

#endif // DEPRECATED
 
