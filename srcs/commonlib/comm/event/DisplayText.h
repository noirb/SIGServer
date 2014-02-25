/* $Id: DisplayText.h,v 1.3 2011-12-14 08:13:55 okamoto Exp $ */ 
#ifndef CommEvent_DisplayText_h
#define CommEvent_DisplayText_h

#include <string>

class DisplayTextEvent
{
	typedef std::string S;
private:
	short	m_fs;
	short	m_color;
	S 	m_msg;
public:
	bool 	set(int packetNum, int seq, char *data, int n);

	const char *msg() { return m_msg.c_str(); }
	int	getFontSize() { return m_fs; }
	int	getColor() { return m_color; }

};
	
#endif // CommEvent_DisplayText_h
 

