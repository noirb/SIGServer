/* $Id: NameService.h,v 1.2 2011-09-09 07:37:20 okamoto Exp $ */ 
#ifndef Comm_Event_NameService_h
#define Comm_Event_NameService_h

#include "Service.h"


class RequestNSQueryEvent
{
private:
	Service::Kind m_kind;

public:
	RequestNSQueryEvent() : m_kind(Service::SERVICE_KIND_NOT_SET) {;}

	Service::Kind kind() { return m_kind; }

	bool set(int, int, char *, int);
};


class ResultNSQueryEvent
{
private:
	Service *m_service;
public:
	ResultNSQueryEvent() : m_service(NULL) {}
	~ResultNSQueryEvent();

	Service *releaseService() {
		Service *tmp = m_service;
		m_service = NULL;
		return tmp;
	}
public:
	bool set(int, int, char *, int);
};


class RequestNSPingerEvent
{
private:
	Service::Kind m_kind;
public:
	RequestNSPingerEvent() : m_kind(Service::SERVICE_KIND_NOT_SET) {;}
	Service::Kind kind() { return m_kind; }
public:
	bool set(int, int, char *, int);
};

class ResultNSPingerEvent
{
private:
	bool m_active;
public:
	ResultNSPingerEvent() : m_active(false) {}
	bool active() { return m_active; }
public:
	bool set(int, int, char *, int);
};

#endif // Comm_Event_NameService_h
 

