/* $Id: Source.h,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */ 
#ifndef Source_h
#define Source_h

#include "systemdef.h"

#include <string>
#include <map>

#include <string.h>

#ifndef WIN32
#include <sys/socket.h>
#endif

#ifdef EXEC_SIMULATION
#include <pthread.h>

class CommDataEncoder;
#endif

enum {
	SOURCE_TYPE_NOT_SET = -1,
	SOURCE_TYPE_SIMSERVER,
	SOURCE_TYPE_VIEW,
	SOURCE_TYPE_CONTROLLER_CMD,
	SOURCE_TYPE_CONTROLLER_DATA,
	SOURCE_TYPE_SERVICE_PROVIDER,
	SOURCE_TYPE_NUN,
	SOURCE_TYPE_NEW_VIEW, // new version
	SOURCE_TYPE_MESSAGE   
};
typedef signed SourceType;

class Source
{
private:
#ifdef EXEC_SIMULATION
	class Locker
	{
	private:
		pthread_mutex_t m_mutex;
	public:
		Locker() { pthread_mutex_init(&m_mutex, 0); }
		~Locker() { pthread_mutex_destroy(&m_mutex); }
		void	lock() { pthread_mutex_lock(&m_mutex); }
		void	unlock() { pthread_mutex_unlock(&m_mutex); }
			
	};
#endif
	typedef std::string S;
	typedef std::map<S, S> M;
private:
	SourceType 	m_type;
	S		m_name;
	SOCKET		m_sock;
	S		m_hostname;
	bool		m_ignore;
	M		m_properties;
#ifdef EXEC_SIMULATION
	Locker		m_locker;
#endif
	
public:
	Source(SOCKET sock)
		: m_type(SOURCE_TYPE_NOT_SET), m_sock(sock), m_ignore(false) {;}
	
	Source(SOCKET sock, const char *hostname) 
		: m_type(SOURCE_TYPE_NOT_SET), m_sock(sock), m_hostname(hostname), m_ignore(false) {;}

	void	set(SourceType type, const char *name) {
		m_type = type;
		if (name) {
			m_name = name;
		}
	}

	SOCKET	socket() { return m_sock; }
#ifdef EXEC_SIMULATION
	int	send(CommDataEncoder &);
	int	send(char *, int);
	bool  	equals(SOCKET sock) {
		return m_sock == sock? true: false;
	}
#endif

	const char *name() const { return m_name.length() > 0? m_name.c_str(): ""; }
	const char *hostname() const { return m_hostname.c_str(); }

	bool	ignore() { return m_ignore; }
	void	ignore(bool b) { m_ignore = b; }

	SourceType type() { return m_type; }
	bool	noType() { return m_type == SOURCE_TYPE_NOT_SET? true: false; }
	bool	isView() { return m_type == SOURCE_TYPE_VIEW? true: false; }
	bool	isControllerCmd() { return m_type == SOURCE_TYPE_CONTROLLER_CMD? true: false; }
	bool	isControllerData() { return m_type == SOURCE_TYPE_CONTROLLER_DATA? true: false; }
	bool	isServiceProvider() { return m_type == SOURCE_TYPE_SERVICE_PROVIDER? true: false; }


	void	setProperty(const char *key, const char *val)
	{
		m_properties[S(key)] = S(val);
	}

	bool	propertyEquals(const char *key,  const char *val)
	{
		M::iterator i = m_properties.find(S(key));
		if (i == m_properties.end()) { return false; }
		return strcmp(i->second.c_str(), val) == 0? true: false;
	}
	void	close() {
#ifdef WIN32
		closesocket(m_sock);
		m_sock = SOCKET_ERROR;
#else
		shutdown(m_sock, 1);
		m_sock = -1; 
#endif
	}
	bool 	equals(const Source &o)
	{
		return strcmp(o.name(), name()) == 0? true: false;
	}
};

#endif // Source_h
 

