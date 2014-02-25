/* $Id: NSLookup.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef NSLookup_h
#define NSLookup_h

#include <vector>

#include "systemdef.h"
#include "Service.h"

class NSLookup
{
public:
	class Provider {
	private:
		Service *m_service;
		SOCKET m_sock;
	public:
		Provider(Service *service)
			: m_service(service), m_sock(-1) {}
		~Provider();
		Service &service();
		SOCKET sock();
		bool active() { return sock() >= 0? true: false; }
		void close();
	};
private:
	typedef std::vector<Provider*> C;
private:
	SOCKET		m_sock;
	C 		m_providers;
private:
	Provider * find(Service::Kind kind);
public:
	NSLookup(SOCKET sock) : m_sock(sock) {}
	~NSLookup();
	Provider * lookup(Service::Kind kind);
};


#endif // NSLookup_h
 

