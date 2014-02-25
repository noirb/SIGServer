/* $Id: InvokeMethod.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef InvokeMethod_h
#define InvokeMethod_h

#include <string>
#include <vector>

#include "Encoder.h"
#include "Encode.h"

namespace CommData {

class InvokeOnInit : public NoDataEncoder
{
public:
        InvokeOnInit() : NoDataEncoder(COMM_INVOKE_CONTROLLER_ON_INIT) {;}
};

class InvokeOnAction : public Encoder
{
private:
	enum { BUFSIZE = 256, };
private:
	double	m_time;
public:
	InvokeOnAction(double t)
		: Encoder(COMM_INVOKE_CONTROLLER_ON_ACTION, BUFSIZE) , m_time(t) {;}
	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};



class InvokeOnRecvText : public Encoder
{
private:
	enum { BUFSIZE = 1024, };
private:
	typedef std::string S;
private:
	double  m_time;
	S	m_caller;
	S	m_target;
	S	m_text;
	Encode	m_encode;
public:
	InvokeOnRecvText(double t, const char *caller, const char *target, const char *text, Encode e, double reachRadius)
		: Encoder(COMM_INVOKE_CONTROLLER_ON_RECV_TEXT, BUFSIZE),
		  m_time(t), m_caller(caller), m_encode(e)
	{
		setForwardTo(target, false, reachRadius);
		if (target) { m_target = target; }
		if (text) { m_text = text; }
	}
	int packetNum() { return 1; }
	char *encode(int seq, int &sz);
};

class InvokeOnRecvMessage : public Encoder
{
	enum { BUFSIZE = 2048, };
	typedef std::string S;
	typedef std::vector<S> C;
private:
	S	m_from;
	C 	m_msgs;
public:
	InvokeOnRecvMessage(const char *from, const char * target, int msgc, char **msgv)
		: Encoder(COMM_INVOKE_CONTROLLER_ON_RECV_MESSAGE, BUFSIZE)
	{
		setForwardTo(target, false);
		if (from) { m_from = from; }
		for (int i=0; i<msgc; i++) {
			m_msgs.push_back(S(msgv[i]));
		}
	}
	
	int packetNum() { return 1; }
	char *encode(int seq, int &sz);
};

class InvokeOnCollision : public Encoder
{
	typedef std::vector<std::string> C;
	enum { BUFSIZE = 4096, };
private:
	double	m_time;
	const C &m_with;
public:
	InvokeOnCollision(double t, const C & with)
		: Encoder(COMM_INVOKE_CONTROLLER_ON_COLLISION, BUFSIZE),
		  m_time(t), m_with(with) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


} // namespace

typedef CommData::InvokeOnInit CommInvokeMethodOnInitEncoder;
typedef CommData::InvokeOnAction CommInvokeMethodOnActionEncoder;
typedef CommData::InvokeOnRecvText CommInvokeMethodOnRecvTextEncoder;
typedef CommData::InvokeOnRecvMessage CommInvokeMethodOnRecvMessageEncoder;
typedef CommData::InvokeOnCollision CommInvokeMethodOnCollisionEncoder;

#endif // InvokeMethod_h
 

