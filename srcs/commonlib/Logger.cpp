/* $Id: Logger.cpp,v 1.3 2011-08-10 02:52:03 okamoto Exp $ */ 
#include "Logger.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"

#include <string>

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#define START_NS(NS) namespace NS {
#define END_NS(NS) }

START_NS(LoggerNS);

typedef Logger::OutStream Super;

class STDOUTStream : public Super
{
private:
	bool noHeader() { return false; }
	void print(int, const char *msg)
	{
		fputs(msg, stdout);
		fputs("\n", stdout);
	}
};

class FileStream : public Super
{
private:
	std::string m_fname;
public:
	FileStream(const char *fname) : Super(), m_fname(fname) {;}

private:
	bool noHeader() { return false; }
	void print(int, const char *msg)
	{
		FILE *fp = fopen(m_fname.c_str(), "aw");
		if (fp) {
			fputs(msg, fp);
			fputs("\n", fp);
			fclose(fp);
		}
	}
};

class SocketStream : public Super
{
private:
	SOCKET	m_sock;
public:
	SocketStream(SOCKET sock) : Super(), m_sock(sock) {;}
private:
	bool noHeader() { return true; }
	void print(int level, const char *msg)
	{
#ifndef COMM_NO_SEND
		CommLogMsgEncoder enc(level, msg);
		enc.send(m_sock);
#endif
	}

};

END_NS(LoggerNS);


void Logger::free_()
{
	for (C::iterator i=m_streams.begin(); i!=m_streams.end(); i++) {
		OutStream *p = *i;
		if (p->release()) {
			delete p;
		}
	}
	m_streams.clear();
}


void Logger::pushSTDOUT()
{
	push(new LoggerNS::STDOUTStream);
}

void Logger::pushFile(const char *fname)
{
	push(new LoggerNS::FileStream(fname));
}

void Logger::pushSocket(SOCKET s)
{
	push(new LoggerNS::SocketStream(s));
}

void Logger::pushListener(Listener *l)
{
	push(l);
}

#define ARRAY_SIZE(ARY) (int)(sizeof(ARY)/sizeof(ARY[0]))

void Logger::print(const char *fmt, ...)
{
	if (m_level > m_outlevel) { return; }

	static char buf[1024];

	static char *headers[] = {
		"[ERR]  ",
		"[SYS]  ",
		"[MSG]  ",
		"[DBG1] ",
		"[DBG2] ",
		"[DBG3] ", 
		"[DSP]  "  };

	assert(m_level < ARRAY_SIZE(headers));
	
	strcpy(buf, headers[m_level]);
	char *p = buf + strlen(buf);

	va_list args;
	va_start(args, fmt);
	vsprintf(p, fmt, args);
	va_end(args);

	for (C::iterator i=m_streams.begin(); i!=m_streams.end(); i++) {
		OutStream *out = *i;
		out->print(m_level, out->noHeader()? p: buf);
	}
}

Logger & Logger::get()
{
	static Logger l;
	return l;
}

#ifdef Logger_test
int main()
{
	LOG_STDOUT();
	LOG_FILE("foo.log");

	for (int i=0; i<5; i++) {
		LOG_OUTPUT_LEVEL(i);
		LOG_PRINT(LOG_ERRMSG, ("error[%d]", i));
		LOG_PRINT(LOG_MSG, ("msg"));
		LOG_PRINT(LOG_DEBUG1, ("debug1"));
		LOG_PRINT(LOG_DEBUG2, ("debug2"));
		LOG_PRINT(LOG_DEBUG3, ("debug3"));
		LOG_PRINT(LOG_DISPLAY, ("display"));
	}
	return 0;
}

#endif


