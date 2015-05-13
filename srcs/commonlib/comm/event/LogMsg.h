/* $Id: LogMsg.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 

#ifndef CommEvent_LogMsg_h
#define CommEvent_LogMsg_h

#include <string>

class LogMsgEvent
{
private:
	short m_level;
	std::string m_msg;
public:
	bool set(int packetNum, int seq, char *data, int n);

	int level() { return m_level; }
	const char *msg() { return m_msg.c_str(); }
};
	
#endif // CommEvent_LogMsg_h
 

