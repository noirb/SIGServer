/*
 * Modified by okamoto on 2011-08-10
 */

#ifndef DisplayText_h
#define DisplayText_h

#include "Encoder.h"

namespace CommData {

class DisplayText : public Encoder
{
	enum { BUFSIZE = 1024, }; //TODO: Magic number
private:
	short m_fs;
	short m_color;
	std::string m_msg;
public:
 DisplayText(int fs, int color, const char *msg)
   : Encoder(COMM_DISPLAY_TEXT, BUFSIZE), m_fs(fs), m_color(color), m_msg(msg) {;}
	
	int packetNum() { return 1;}
	char *encode(int seq, int &sz);
};

} // namespace

typedef CommData::DisplayText CommDisplayTextEncoder;

#endif // LogMsg_h
 

