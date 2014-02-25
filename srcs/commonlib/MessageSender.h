/*
 * Written on 2011-05-12
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-29
 *    Modify indent, add English comments
 */
#ifndef MessageSender_h
#define MessageSender_h

#include <string>

#include "RawSound.h"
#include "systemdef.h"

/**
 * @brief Class of sending message to agents
 *
 * Connects the simulation server, and sends message to a designated agent or all agents
 */
class MessageSender
{
private:
	typedef std::string	S;
private:
	S      m_name;
	SOCKET m_sock;
	bool   m_close;
public:
	/**
	 * @brief Constructor
	 * @param myname   sender name
	 * @param s        socket
	 */
	MessageSender(const char *myname, SOCKET s) : m_name(myname), m_sock(s), m_close(false) {}

	/**
	 * @brief Constructor
	 * @param myname   sender name
	 */
	MessageSender(const char *myname) : m_name(myname), m_sock(-1), m_close(true) {}

	//! Destructor
	~MessageSender();

	/**
	 * @brief Connects to designated server
	 * @param server simulation server name (IP address or host name)
	 * @param  port  simulation server port number
	 * @retval true  success
	 * @retval false failure
	 */
	bool connect(char *server, int port);

	/**
	 * @brief Sends message to a designated agent
	 * @param to     agent
	 * @param n      number of message string
	 * @param strs   message string
	 * @retval true  success
	 * @retval false failure
	 */
	bool sendto(const char *to, int n, char **strs);

	/**
	 * @brief Broadcasts message
	 * @param n      number of message string
	 * @param strs   message string
	 * @retval true  success
	 * @retval false failure
	 */
	bool broadcast(int n, char **strs);
};

// begin(FIX20110401)
/**
 * @brief Class of sending sound data to an agent
 */
class SoundSender
{
private:
	typedef std::string	S;

	S m_name;
	SOCKET m_sock;
	bool m_close;

public:
	/**
	 * @brief Constructor
	 * @param myname sender name
	 * @param s      socket
	 */
	SoundSender(const char *myname, SOCKET s) :
		m_name(myname), m_sock(s), m_close(false)
	{}

	/**
	 * @brief Constructor
	 * @param myname sender name
	 */
	SoundSender(const char *myname) :
		m_name(myname), m_sock(-1), m_close(true)
	{}

	//! Destructor
	~SoundSender();

	/**
	 * @brief Connects a designated server
	 * @param server simulation server name (IP address or host name)
	 * @param  port  simulation server port number
	 * @retval true  success
	 * @retval false failure
	 */
	bool connect(char *server, int port);

	/**
	 * @brief Sends sound data to a designated agent
	 * @param to     agent
	 * @param sound  sound data
	 * @retval true  success
	 * @retval false failure
	 */
	bool sendto(const char *to, RawSound &sound);
};
// end(FIX20110401)

#endif // MessageSender_h



