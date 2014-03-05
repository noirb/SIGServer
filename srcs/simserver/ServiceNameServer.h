/*
 * Created by okamoto on 2011-03-25
 */

#ifndef ServiceNameServer_h
#define ServiceNameServer_h

#include <string>
#include <vector>

#include "Service.h"
#include "Locker.h"

class ServiceNameServer
{
	class NSService {
	private:
		Service *m_service;
		int 	m_count;
		bool 	m_dead;
	public:
		NSService(Service *s) : m_service(s), m_count(0), m_dead(false) {;}
		~NSService() { delete m_service; m_service = NULL; }
		Service *service() { return m_service; }
		int	count() { return m_count; }
		int	incr() { m_count++;  return m_count;}
		bool	isDead() { return m_dead; }
		void	dead() { m_dead = true; }
	};
private:
	typedef std::vector<NSService*> C;
private:
	C	m_services;
	Locker	m_locker;
private:
	bool ping(Service &, Service::Kind kind);
public:
	~ServiceNameServer();
	bool	registerService(Service *s);

	Service * lookup(Service::Kind kind);
};

#endif // ServiceNameServer_h
 

