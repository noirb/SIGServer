/*
 * Modified by sekikawa on 2010-08-26
 * Modified by Okamoto on 2011-03-25
 */

#include "NSLookup.h"
#include "CommUtil.h"
#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "CTReader.h"
#include "Service.h"
#include "Logger.h"
#include <assert.h>
#include <unistd.h>

#define FREE(P) if (P) { delete P; P = NULL; }

NSLookup::Provider::~Provider()
{
	close();
	FREE(m_service);
}

void NSLookup::Provider::close()
{
	if (m_sock >= 0) {
#ifdef WIN32
		::closesocket(m_sock);
#else
		::close(m_sock);
#endif
		m_sock = -1;
	}
}


Service &NSLookup::Provider::service()
{
	assert(m_service != NULL);
	return *m_service;
}


SOCKET NSLookup::Provider::sock()
{
	if (m_sock < 0) {
		// modify(sekikawa)(FIX20100826)
		// m_sock = CommUtil::connectServer(m_service->hostname(), m_service->port());
		m_sock = CommUtil::connectServer(m_service->hostname(), m_service->port(), 10);

		// add(sekikawa)(FIX20100826)
		if (m_sock < 0) {
			LOG_ERR(("failed to connect to service provider. (hostname=%s, port=%d) [%s:%d]",
			         m_service->hostname(),
			         m_service->port(),
			         __FILE__, __LINE__));
		}
	}

	return m_sock;
}


NSLookup::~NSLookup()
{
	for (std::vector<Provider*>::iterator i=m_providers.begin(); i!=m_providers.end(); i++) {
		Provider *p = *i;
		delete p;
	}
	m_providers.clear();
}


NSLookup::Provider *NSLookup::find(Service::Kind kind)
{
	for (std::vector<Provider*>::iterator i=m_providers.begin(); i!=m_providers.end(); i++) {
		Provider *p = *i;
		if (p->service().kind() & kind) {
			if (p->active()) {
				return p;
			} else {
				delete p;
				m_providers.erase(i);
			}
		}
	}
	return NULL;
}


NSLookup::Provider* NSLookup::lookup(Service::Kind kind)
{
	Provider *p = find(kind);
	if (p) {
		// requested service was found in cash.
		return p;
	}

	CommRequestNSQueryEncoder enc(kind);
	enc.send(m_sock);

	typedef CTReader Reader;
	typedef CommDataDecoder::Result Result;
	CommDataDecoder d;
	Reader r(m_sock, d, 128);

// modify(sekikawa)(FIX20100826)
//	int retry = 5;
	int retry = 100;

	int timeout = 10000;
	while (retry > 0) {
		Result *result = r.readSync();
		if (!result) {
			retry--;

			// add(sekikawa)(FIX20100826)
			LOG_DEBUG1(("retrying (%d) [%s:%d]", retry, __FILE__, __LINE__));
#ifdef WIN32
			Sleep(timeout);
#else
			usleep(timeout);
#endif
			timeout *= 2;
			continue;
		}

		Provider *provider = NULL;
		if (result->type() == COMM_NS_QUERY_RESULT) {
			ResultNSQueryEvent *evt = (ResultNSQueryEvent*)result->data();
			Service *service = evt->releaseService();
			if (service == NULL) {
				fprintf(stderr, "service is NULL\n");
				return NULL;
			}

			LOG_DEBUG1(("ns : %s:%d kind = %#x\n",
					service->hostname(),
					service->port(),
					service->kind()));
			provider = new Provider(service);
			SOCKET sock = provider->sock();
			if (sock < 0) {
				FREE(provider);
				LOG_ERR(("cannot connect service provider\n"));
			} else {
				// connect to service provider succeeded.
				// register it to cash.
				m_providers.push_back(provider);
			}
		} else {
			LOG_ERR(("cannot get result\n"));
		}
		delete result;
		return provider;
	}
	return NULL;
}

