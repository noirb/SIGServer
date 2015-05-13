/* $Id: ResultAttach.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Event_ResultAttach_h
#define Comm_Event_ResultAttach_h

#include <string>
#include "CommDataType.h"
#include "Result.h"

class X3DDB;

class ResultAttachViewEvent
{
private:
	CommResultType m_result;
	std::string    m_errmsg;
	X3DDB *m_db;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	ResultAttachViewEvent() : m_db(NULL) {}
	~ResultAttachViewEvent();

	CommResultType result() { return m_result; }
	const char *errmsg() { return m_errmsg.length() > 0? m_errmsg.c_str(): 0; }
	X3DDB *getX3DDB() { return m_db; }

	X3DDB *releaseX3DDb() {
		X3DDB *tmp = m_db;
		m_db = NULL;
		return tmp;
	}
};


class ResultAttachControllerEvent : public ResultEvent {};

#endif // Comm_Event_ResultAttach_h


 

