/*
 * Written by Okamoto on 2011-08-10
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-29
 *    Modify indent, add English comments
 */
#ifndef Logger_h
#define Logger_h

#include "systemdef.h"

#include <vector>

enum {
	LOG_ERRMSG = 0,
	LOG_SYSTEM,
	LOG_MSG,
	LOG_DEBUG1,
	LOG_DEBUG2,
	LOG_DEBUG3,
	LOG_DISPLAY,   //add by okamoto@tome (2011/8/3)
	LOG_ALL,
};

/**
 * @brief Logger class
 *
 * Implements logger that is sattable output level and multiple destinations, and switchable destinations
 * Sendable logs to the simulation server
 * Should not directly use this class, but use through macro
 */
class Logger
{
public:
	//! Class for extracting destination
	class OutStream
	{
	private:
		bool m_release;
	protected:
		OutStream() : m_release(true) {;}
		OutStream(bool release) : m_release(release) {;}
	public:
		virtual ~OutStream() {;}

		virtual bool noHeader() = 0;
		virtual void print(int level, const char *msg) = 0;

		bool release() { return m_release; }
	};

	//! Log listener class
	class Listener : public OutStream
	{
	protected:
		Listener() : OutStream(false) {;}
	private:
		bool noHeader() { return true; }
	};
	
private:
	std::vector<OutStream*> m_streams;
	int m_level;
	int m_outlevel;
private:
	void push(OutStream *o) { m_streams.push_back(o); }
	void free_();
	Logger() : m_level(LOG_MSG), m_outlevel(LOG_MSG) {;}
public:
	//! Destructor
	~Logger() { free_(); }

	//! Push standard output to destination
	void pushSTDOUT();
	//! Push a flie to destination
	void pushFile(const char *fname);
	//! Push a socket to destination
	void pushSocket(SOCKET);
	/**
	 * @brief Push listener to destination
	 *  Output to arbitrary destination is available by implementing of listener
	 */
	void pushListener(Listener *l);
	//! Set output level
	void setOutputLevel(int l) { m_outlevel = l; }
	//! Set log level
	void setLevel(int l) { m_level = l; }
	//! Output method using format
	void print(const char *fmt, ...);
public:
	//! Get instance of Logger class (singleton pattern)
	static Logger & get();
};


#define LOG_STDOUT()    Logger::get().pushSTDOUT()
#define LOG_FILE(FNAME) Logger::get().pushFile(FNAME)
#define LOG_SOCKET(S)   Logger::get().pushSocket(S)
#define LOG_LISTENER(S) Logger::get().pushListener(S)

#define LOG_OUTPUT_LEVEL(L) Logger::get().setOutputLevel(L)
#define LOG_LEVEL_MIN(A, B) ( (A) < (B)? (A): (B) )
#define LOG_LEVEL_INCR(L, INCR) L = LOG_LEVEL_MIN(L + INCR, LOG_ALL)


#define LOG_PRINT(L, MSG) { Logger &l_ = Logger::get(); l_.setLevel(L); l_.print MSG; }

#define LOG_ERR(MSG) LOG_PRINT(LOG_ERRMSG, MSG)
#define LOG_ERROR    LOG_ERR

#define LOG_MSG(MSG)    LOG_PRINT(LOG_MSG, MSG)
#define LOG_SYSTEM(MSG) LOG_PRINT(LOG_SYSTEM, MSG)
#define LOG_SYS         LOG_SYSTEM

#define LOG_DEBUG1(MSG) LOG_PRINT(LOG_DEBUG1, MSG)
#define LOG_DEBUG2(MSG) LOG_PRINT(LOG_DEBUG2, MSG)
#define LOG_DEBUG3(MSG) LOG_PRINT(LOG_DEBUG3, MSG)

#define LOG_DISPLAY(MSG) LOG_PRINT(LOG_DISPLAY, MSG)

#endif // Logger_h



