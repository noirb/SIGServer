/* $Id: NameService.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <sigverse/commonlib/comm/event/NameService.h>
#include <sigverse/commonlib/binary.h>
#include <assert.h>

bool RequestNSQueryEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	unsigned short v = BINARY_GET_DATA_S_INCR(p, unsigned short);
	m_kind = (Service::Kind)v;
	return true;
}

bool ResultNSQueryEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	unsigned short found = BINARY_GET_DATA_S_INCR(p, unsigned short);

	if (found) {
		char *name = BINARY_GET_STRING_INCR(p);
		
		unsigned short v = BINARY_GET_DATA_S_INCR(p, unsigned short);
		Service::Kind kind = (Service::Kind)v;

		char *hostname = BINARY_GET_STRING_INCR(p);

		int port = BINARY_GET_DATA_L_INCR(p, int);

		m_service = new Service(name, kind, hostname, port);
		
		BINARY_FREE_STRING(name);
		BINARY_FREE_STRING(hostname);
	}

	return true;
}


bool RequestNSPingerEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	assert(sizeof(Service::Kind) == 4);
	m_kind = BINARY_GET_DATA_L_INCR(p, Service::Kind);
	return true;
}

#define FREE(P) if (P) { delete P; P = NULL; }

ResultNSQueryEvent::~ResultNSQueryEvent()
{
	FREE(m_service);
}

bool ResultNSPingerEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	unsigned short v = BINARY_GET_DATA_S_INCR(p, unsigned short);
	m_active = v != 0? true: false;

	return true;
}

