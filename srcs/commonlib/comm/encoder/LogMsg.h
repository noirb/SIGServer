/*
 * Created by okamoto on 2011-03-25
 */

#ifndef LogMsg_h
#define LogMsg_h

#include "Encoder.h"

namespace CommData {

class LogMsg : public Encoder
{
	enum { BUFSIZE = 1024, }; //TODO: Magic number
private:
	short	m_level;
	std::string m_msg;
public:
	LogMsg(int level, const char *msg)
		: Encoder(COMM_LOG_MSG, BUFSIZE),
		  m_level(level), m_msg(msg) {;}

	int packetNum() { return 1; }
	char *encode(int seq, int &sz);
};

} // namespace

typedef CommData::LogMsg CommLogMsgEncoder;

#endif // LogMsg_h
 

