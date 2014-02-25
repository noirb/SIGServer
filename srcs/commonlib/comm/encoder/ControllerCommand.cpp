/* $Id: ControllerCommand.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */

#ifdef DEPRECATED

#include "ControllerCommand.h"
#include "Command.h"
#include "binary.h"
#include <assert.h>

BEGIN_NS_COMMDATA();

char *ControllerCommand::encode(int seq, int &sz)
{
	char *p = m_buf;
	int n;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}
	
	{
		char *data = m_cmd.toBinary(n);
		assert(data);
		BINARY_SET_DATA_S_INCR(p, unsigned short, n);
		memcpy(p, data, n);
		p += n;
	}
	
	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}
	sz = p - m_buf;
	assert(sz < BUFSIZE);
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();


#ifdef ControllerCommand_test

#include "../../command/Move.h"
#include "../../command/JointControl.h"
#include "Header.h"
#include "../event/ControllerCommand.h"


static Command * create()
{
	//return new MoveCommand("foo", Vector3d(1, 2, 3), 5);
	JointControlCommand *cmd = new JointControlCommand("foo");
	cmd->set("JOINT1", 1.0);
	cmd->set("JOINT2", -1.0);
	return cmd;
}

int main(int argc, char **argv)
{
	Command *cmd = create();
	ControllerCommandEncoder enc(*cmd);
	int n;
	char *p = enc.encode(0, n);
	if (p) {
		printf("size = %d\n", n);
 CommDataHeader h;
		int nn = CommData::getHeader(p, n, h);
		if (nn > 0) {
			char *pp = p+nn;
			ControllerCommandEvent evt;
			evt.set(h.packetNum, h.seq, pp, n-(pp-p));
			if (Command *cmd = evt.command()) {
				cmd->dump();
			}
			
		}
	}
	return 0;
}

#endif

#endif // DEPRECATED
