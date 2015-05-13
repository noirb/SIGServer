/* $Id: ServiceNameServer.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include "ServiceNameServer.h"
#include "Logger.h"
#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "ct/CTReader.h"
#include "CommUtil.h"
#include <unistd.h>

ServiceNameServer::~ServiceNameServer()
{
	for (std::vector<NSService*>::iterator i=m_services.begin(); i!=m_services.end(); i++) {
		NSService *s = *i;
		delete s;
	}
	m_services.clear();
}


bool ServiceNameServer::ping(Service &s, Service::Kind kind)
{
	typedef CommRequestNSPingerEncoder Enc;
	typedef CommDataDecoder::Result Result;

	SOCKET sock = CommUtil::connectServer(s.hostname(), s.port());
	if (sock < 0) { return false; }

	{
		Enc enc(kind);
		enc.send(sock);
	}

	CommDataDecoder d;

	// modify(sekikawa)(FIX20100826)
	// int retry = 5;
	int retry = 100;

	bool ret = false;
	CTReader *r = new CTReader(sock, d, 256);

	while (retry > 0) {
		Result *result = r->readSync();

		if (result != NULL) {
			if (result->type() == COMM_NS_PINGER_RESULT) {
				ResultNSPingerEvent *evt = (ResultNSPingerEvent*)result->data();
				ret = evt->active()? true: false;
			}
			break;
		}
		retry--;

		// add(sekikawa)(FIX20100826)
		usleep(10000);	// microsec
		LOG_DEBUG1(("ping retry (%d) [%s:%d]", retry, __FILE__, __LINE__));
	}
	close(sock);
	return ret;
}


bool ServiceNameServer::registerService(Service *s)
{
	if (!s) { return false; }
	m_locker.lock();
	m_services.push_back(new NSService(s));
	m_locker.unlock();
	return true;
}

Service * ServiceNameServer::lookup(Service::Kind kind)
{
	m_locker.lock();
	NSService *target = NULL;
	std::vector<NSService*>::iterator i;

	for (i=m_services.begin(); i!=m_services.end(); i++) {

		NSService *nss = *i;
		if (nss->isDead()) { continue; }
		Service *s = nss->service();
		LOG_DEBUG1(("service provider : %s:%d ", s->hostname(), s->port()));
		LOG_DEBUG1(("kind : %#x ", s->kind()));

		if (s->kind() & kind) {
			if (target != NULL) {
				if (nss->count() < target->count()) {
					target = nss;
				}
			} else {
				target = nss;
			}
		}
	}

	Service *service = NULL;
	if (!target) {
		goto ret;
	}
	target->incr();

	service = target->service();

	if (!ping(*service, kind)) {
		LOG_SYS(("%s(%s:%d) is not available", service->name(), service->hostname(), service->port()));
		target->dead();
		service = NULL;
		goto ret;
	} else {
		LOG_DEBUG1(("%s(%s:%d) is active", service->name(), service->hostname(), service->port()));
	}

ret:
	m_locker.unlock();
	return service;
}

