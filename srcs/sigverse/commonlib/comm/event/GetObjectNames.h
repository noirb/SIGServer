/* $Id: GetObjectNames.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Event_GetObjectNames_h
#define Comm_Event_GetObjectNames_h

#include <string>
#include <vector>

#include "comm/encoder/GetObjectNames.h"

class RequestGetObjectNamesEvent
{
private:
	CommData::GetObjectNamesRequest::Type m_type;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	bool getAgents() { return true; }
	bool getEntities() { return true; }
};

class ResultGetObjectNamesEvent
{
private:
	std::vector<std::string> m_names;
public:
	bool set(int packetNum, int seq, char *data, int n);
	int  copy(std::vector<std::string> &o) {
		o = m_names;
		return o.size();
	}
};

#endif // Comm_Event_GetObjectNames_h
 

