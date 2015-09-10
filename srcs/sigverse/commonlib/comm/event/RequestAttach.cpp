/* $Id: RequestAttach.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "RequestAttach.h"
#include "binary.h"
#include "Service.h"

bool RequestAttachEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	char *name = BINARY_GET_STRING_INCR(p);
	if (!name) { return false; }
	m_name = name;
	BINARY_FREE_STRING(name);
	return true;
}


bool RequestAttachViewEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	char *name = BINARY_GET_STRING_INCR(p);
	if (!name) {
		return false;
	}
	m_name = name;
	BINARY_FREE_STRING(name);

	unsigned short v = BINARY_GET_DATA_S_INCR(p, unsigned short);
	m_polling = !!v;
	return true;
}

#define FREE(P) if (P) { delete P; P = 0; }

RequestProvideServiceEvent::~RequestProvideServiceEvent()
{
	FREE(m_service);
}


bool RequestProvideServiceEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	char *servicename = BINARY_GET_STRING_INCR(p);

	typedef Service::Kind Kind;
	
	Kind kind = BINARY_GET_DATA_L_INCR(p, Kind);

	char *hostname = BINARY_GET_STRING_INCR(p);
	int port = BINARY_GET_DATA_L_INCR(p, int);
	
	m_service = new Service(servicename, kind, hostname, port);

	BINARY_FREE_STRING(servicename);
	BINARY_FREE_STRING(hostname);

	return true;
}
