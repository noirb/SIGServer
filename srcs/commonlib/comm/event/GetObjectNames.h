/* $Id: GetObjectNames.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Event_GetObjectNames_h
#define Comm_Event_GetObjectNames_h

#include <string>
#include <vector>

#include "comm/encoder/GetObjectNames.h"

class RequestGetObjectNamesEvent
{
private:
	typedef CommData::GetObjectNamesRequest E;
private:
	E::Type m_type;
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	bool	getAgents() { return true; }
	bool	getEntities() { return true; }
};

class ResultGetObjectNamesEvent
{
private:
	typedef std::string S;
	typedef std::vector<S> C;
private:
	C	m_names;
public:
	bool	set(int packetNum, int seq, char *data, int n);
	int 	copy(C &o) {
		o = m_names;
		return o.size();
	}
};

#endif // Comm_Event_GetObjectNames_h
 

