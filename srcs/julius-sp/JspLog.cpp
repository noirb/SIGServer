#include "JspLog.h"
#include <stdio.h>
#include <stdarg.h>

namespace Jsp {
	int g_logLevel = 1;

	void setLogLevel(int lvl)
	{
		g_logLevel = lvl;
	}

	int getLogLevel()
	{
		return g_logLevel;
	}

	void printLog(int lvl, const char *format, ...)
	{
		if (lvl <= g_logLevel)
		{
			char buf[256];
			va_list arg;

			va_start(arg, format);
			vsprintf(buf, format, arg);

			fprintf(stderr, buf);

			va_end(arg);
		}
	}
};
