/*
 * Created by okamoto on 2011-03-25
 */

#ifndef LogTransfer_h
#define LogTransfer_h

#include "Logger.h"

#include <string>
#include <vector>

class Connection;
class ServerAcceptProc;

class LogTransfer : public Logger::Listener
{
	struct Entry
	{
		int level;
		std::string msg;
		Entry(int l, const char *msg_) : level(l), msg(msg_) {;}
	};
	typedef std::vector<Entry *> EntryC;
private:
	ServerAcceptProc &m_accept;
	EntryC            m_entries;
public:
	LogTransfer(ServerAcceptProc &a) : m_accept(a) {;}

	void flush(const std::vector<Connection *> &clients);
private:
	void print(int level, const char *msg)
	{
		m_entries.push_back(new Entry(level, msg));
	}
};

#endif // LogTransfer_h
