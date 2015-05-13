/* $Id: RequestGetEntity.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef CommEvent_RequestGetEntity_h
#define CommEvent_RequestGetEntity_h

#include <string>


class RequestGetEntityEvent
{
private:
	std::string m_name;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	const char *name() { return m_name.c_str(); }
};


#endif // CommEvent_RequestGetEntity_h
 

