/*
 * Created by okamoto on 2011-03-25
 */

#ifndef CommEvent_Result_h
#define CommEvent_Result_h

#include <string>

#include "CommDataType.h"

class ResultEvent
{
private:
	CommResultType m_result;
	std::string    m_errmsg;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	CommResultType result() { return m_result; }
	const char *errmsg() { return m_errmsg.length() > 0? m_errmsg.c_str(): 0; }
};



#endif // CommEvent_Result_h
 

