/* $Id: RequestAttach.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef CommEvent_RequestAttach_h
#define CommEvent_RequestAttach_h

#include <string>

class RequestAttachEvent
{
	typedef std::string S;
private:
	S	m_name;
public:
	bool	set(int packetNum, int seq, char *data, int n);

public:
	const char *getName()
	{
		return m_name.length() > 0? m_name.c_str(): NULL;
	}
};

class RequestAttachControllerEvent : public RequestAttachEvent
{
public:
	const char *getAgentName() { return getName(); }
};

class RequestAttachViewEvent
{
	bool 		m_polling;
	std::string 	m_name;
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	bool  isPolling() { return m_polling; }
	const char *getViewName() {
		return m_name.length() > 0? m_name.c_str(): NULL;
	}
};

class RequestConnectDataPortEvent : public RequestAttachEvent {};

class Service;

class RequestProvideServiceEvent 
{
private:
	typedef std::string	S;
	S 		m_name;
	Service	*	m_service;
public:
	RequestProvideServiceEvent() : m_service(NULL) {}
	~RequestProvideServiceEvent();

	const char *getProviderName() { return m_name.c_str(); }
	Service & getService() { return *m_service; }
	Service * releaseService() {
		Service *tmp = m_service;
		m_service = NULL;
		return tmp;
	}
public:
	bool	set(int packetNum, int seq, char *data, int n);
};

#endif // CommEvent_RequestAttach_h
 

