/*
 * Modified by okamoto on 2011-09-09
 */

#ifndef Service_h
#define Service_h

#include <string>

class Service
{
public:
	enum {
		SERVICE_KIND_NOT_SET = 0x00, 
		CAPTURE_VIEW = 0x01,         
		DETECT_ENTITIES = 0x02,      
		SOUND_RECOG = 0x04,          
		DISTANCE_SENSOR = 0x05,     
	};
	typedef unsigned int Kind;
private:
	typedef std::string S;
	
private:
	S		m_name;
	Kind		m_kind;
	S		m_hostname;
	int	        m_port;
public:
	const char *	name() const { return m_name.length() > 0? m_name.c_str(): NULL; }
	Kind		kind() const { return m_kind; }

	void		hostname(const char *h) {
		m_hostname = h;
	}
	const char *	hostname() {
		return m_hostname.length() > 0? m_hostname.c_str(): NULL;
	}
	int	       	port() const { return m_port; }
public:
	Service(const char *name_, Kind k, int p) : m_kind(k), m_port(p)
	{
		if (name_) { m_name = name_; }
	}

	Service(const char *name_, Kind k, const char *h, int p) :  m_kind(k),m_port(p)
	{
		if (name_) { m_name = name_; }
		if (h) { m_hostname = h; }
	}
};



#endif // Service_h
 

