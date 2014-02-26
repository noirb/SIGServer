/*
 * Created by Okamoto on 2011-03-25
 */

#ifdef DEPRECATED

#include "Command.h"
#include "Move.h"
#include "JointControl.h"
#include "binary.h"
#include <assert.h>

Command * Command::decode(char *buf, int size)
{
	Command *cmd = NULL;
	
	char *p = buf;
	CommandType type = BINARY_GET_DATA_S_INCR(p, CommandType);
	std::string agentName = BINARY_GET_STRING_INCR(p);

	switch(type) {
	case COMMAND_MOVE:
		{
			double x = BINARY_GET_DOUBLE_INCR(p);
			double y = BINARY_GET_DOUBLE_INCR(p);
			double z = BINARY_GET_DOUBLE_INCR(p);
			double vel = BINARY_GET_DOUBLE_INCR(p);
			cmd = new MoveCommand(agentName.c_str(), Vector3d(x, y, z), vel);
		}
		break;
	case COMMAND_JOINT_CONTROL:
		{
			JointControlCommand *jcmd = new JointControlCommand(agentName.c_str());
			unsigned short n = BINARY_GET_DATA_S_INCR(p, unsigned short);
			for (int i=0; i<n; i++) {
				std::string name = BINARY_GET_STRING_INCR(p);
				double v = BINARY_GET_DOUBLE_INCR(p);
				jcmd->set(name.c_str(), v);
			}
			cmd = jcmd;

		}
		break;
	default:
		assert(0); break;
		
	}
	assert((p - buf) == size);

	return cmd;
}

#endif
