#include <sigverse/simserver/LogTransfer.h>
#include <sigverse/simserver/ServerAcceptProc.h>
#include <sigverse/commonlib/CommDataEncoder.h>

void LogTransfer::flush(const std::vector<Connection*> &clients)
{
	m_accept.lock();

	for (EntryC::const_iterator i=m_entries.begin(); i!=m_entries.end(); i++) {

		Entry *e = *i;
		CommLogMsgEncoder enc(e->level, e->msg.c_str());

		for (ServerAcceptProc::ConC::const_iterator j=clients.begin(); j!=clients.end(); j++) {

			Connection *con = *j;
			Source *source = con->source;
			if (source->isView()) {
				source->send(enc);
			}
		}
		delete e;
	}
	m_entries.clear();
	m_accept.unlock();
}
