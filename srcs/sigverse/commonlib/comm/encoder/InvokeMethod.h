/*
 * Created by okamoto on 2011-03-25
 */

#ifndef InvokeMethod_h
#define InvokeMethod_h

#include <string>
#include <vector>

#include <sigverse/commonlib/comm/encoder/Encoder.h>
#include <sigverse/commonlib/Encode.h>

namespace CommData {

class InvokeOnInit : public NoDataEncoder
{
public:
	InvokeOnInit() : NoDataEncoder(COMM_INVOKE_CONTROLLER_ON_INIT) {;}
};

class InvokeOnAction : public Encoder
{
private:
	enum { BUFSIZE = 256, }; //TODO: Magic number
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
	enum { BUFSIZE = 1024, }; //TODO: Magic number
private:
	double  m_time;
	std::string m_caller;
	std::string m_target;
	std::string m_text;
	Encode      m_encode;
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
	enum { BUFSIZE = 2048, }; //TODO: Magic number
private:
	std::string m_from;
	std::vector<std::string> m_msgs;
public:
	InvokeOnRecvMessage(const char *from, const char * target, int msgc, char **msgv)
		: Encoder(COMM_INVOKE_CONTROLLER_ON_RECV_MESSAGE, BUFSIZE)
	{
		setForwardTo(target, false);
		if (from) { m_from = from; }

		for (int i=0; i<msgc; i++) {
			m_msgs.push_back(std::string(msgv[i]));
		}
	}
	
	int packetNum() { return 1; }
	char *encode(int seq, int &sz);
};

class InvokeOnCollision : public Encoder
{
	enum { BUFSIZE = 4096, }; //TODO: Magic number
private:
	double m_time;
	const std::vector<std::string> &m_with;
public:
	InvokeOnCollision(double t, const std::vector<std::string> & with)
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
 

