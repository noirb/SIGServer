/*
 * Modified by okamoto on 2011-03-31
 * Added comments by Tetsunari Inamura on 2014-02-28
 */

#ifndef __Source_h__
#define __Source_h__

#include "systemdef.h"

#include <string>
#include <map>

#include <string.h>

#ifndef WIN32
#include <sys/socket.h>
#endif

#include "pthread_dep.h"

class CommDataEncoder;

enum {
	SOURCE_TYPE_NOT_SET = -1,
	SOURCE_TYPE_SIMSERVER,
	SOURCE_TYPE_VIEW,
	SOURCE_TYPE_CONTROLLER_CMD,
	SOURCE_TYPE_CONTROLLER_DATA,
	SOURCE_TYPE_SERVICE_PROVIDER,
	SOURCE_TYPE_NUN,
	SOURCE_TYPE_NEW_VIEW, // new version
	SOURCE_TYPE_MESSAGE   // new version for Service Provider
};
typedef signed SourceType;

/**
 * @brief Class for source of data transfer connection
 */
class Source
{
private:
	class Locker
	{
	private:
		pthread_mutex_t m_mutex;
	public:
		Locker()  { pthread_mutex_init(&m_mutex, 0); }
		~Locker() { pthread_mutex_destroy(&m_mutex); }

		void lock()   { pthread_mutex_lock(&m_mutex); }
		void unlock() { pthread_mutex_unlock(&m_mutex); }
	};

private:
	SourceType    m_type;
	std::string   m_name;
	SOCKET        m_sock;
	std::string   m_hostname;
	bool          m_ignore;
	std::map<std::string, std::string> m_properties;
	Locker        m_locker;
	
public:
	/**
	 * @brief Constructor
	 *
	 * @param sock Socket of connection source
	 */
	Source(SOCKET sock)
		: m_type(SOURCE_TYPE_NOT_SET), m_sock(sock), m_ignore(false)
	{;}

	/**
	 * @brief Constructor
	 *
	 * @param sock     Socket of connection source
	 * @param hostname Hostname of connection source
	 */	
	Source(SOCKET sock, const char *hostname) 
		: m_type(SOURCE_TYPE_NOT_SET), m_sock(sock), m_hostname(hostname), m_ignore(false)
	{;}

	/**
	 * @brief Set type of connection source and connection name
	 */
	void set(SourceType type, const char *name)
	{
		m_type = type;
		if (name) {
			m_name = name;
		}
	}

	//! Return socket descripter
	SOCKET socket() { return m_sock; }

	int send(CommDataEncoder &);
	int send(char *, int);

	bool equals(SOCKET sock)
	{
		return m_sock == sock? true: false;
	}

	//! Refer connection name
	const char *name() const { return m_name.length() > 0? m_name.c_str(): ""; }
	//! Refer hostname of connection source
	const char *hostname() const { return m_hostname.c_str(); }

	//! Whether ignore the connection or not
	bool ignore() { return m_ignore; }
	//! Switch the ignore flag
	void ignore(bool b) { m_ignore = b; }
	//! Refer connection type
	SourceType type() { return m_type; }
	//! Whether the connection type is set or not
	bool noType() { return m_type == SOURCE_TYPE_NOT_SET? true: false; }
	//! Is the connection type 'View'?
	bool isView() { return m_type == SOURCE_TYPE_VIEW? true: false; }
	//! Is the connection type 'Controller Command'?
	bool isControllerCmd() { return m_type == SOURCE_TYPE_CONTROLLER_CMD? true: false; }
	//! Is the connection type 'Controller Data'?
	bool isControllerData() { return m_type == SOURCE_TYPE_CONTROLLER_DATA? true: false; }
	//! Is the connection type 'Service Provider'?
	bool isServiceProvider() { return m_type == SOURCE_TYPE_SERVICE_PROVIDER? true: false; }

	/**
	 * @brief Set connection property
	 * @param key   name of property
	 * @param value value of property
	 */
	void setProperty(const char *key, const char *val)
	{
		m_properties[std::string(key)] = std::string(val);
	}

	/**
	 * @brief Is connection property equal?
	 * @param key   name of property
	 * @param value value of property
	 */
	bool propertyEquals(const char *key,  const char *val)
	{
		std::map<std::string, std::string>::iterator i = m_properties.find(std::string(key));
		if (i == m_properties.end()) { return false; }
		return strcmp(i->second.c_str(), val) == 0? true: false;
	}

	//! Close the connection
	void close() {
#ifdef WIN32
		closesocket(m_sock);
		m_sock = SOCKET_ERROR;
#else
		// Use shutdown instead of close for socket for writing
		shutdown(m_sock, 1);
		m_sock = -1; 
#endif
	}
	//! Is the same connection source?
	bool equals(const Source &o)
	{
		return strcmp(o.name(), name()) == 0? true: false;
	}
};

#endif // __Source_h__
