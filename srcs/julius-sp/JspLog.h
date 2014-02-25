#ifndef _JSP_LOG_H_
#define _JSP_LOG_H_

#define	LOG_FATAL 	1
#define LOG_ALWAYS	1
#define LOG_INFO 	2
#define LOG_DEBUG	3

namespace Jsp {
	// ----------------------------------------------------
	///
	// ----------------------------------------------------
	void setLogLevel(int lvl);

	// ----------------------------------------------------
	///
	// ----------------------------------------------------
	int getLogLevel();

	// ----------------------------------------------------
	///
	// ----------------------------------------------------
	void printLog(int lvl, const char *format, ...);
};

#endif

