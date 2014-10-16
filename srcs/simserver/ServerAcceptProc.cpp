/*
 * Written by okamoto on 2011-12-13
 * Modified by Tetsunari Inamura on 2013-12-31
 *    Add English comments
 */
#ifdef IRWAS_SIMSERVER

#include "ServerAcceptProc.h"
#include "CommDataEncoder.h"
#include "Logger.h"
#include "SimWorldProvider.h"
#include "SSimWorld.h"
#include "SSimObj.h"
#include "binary.h"

#ifdef DEPRECATED
#include "Command.h"
#endif

#include "ServiceNameServer.h"
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#endif

#ifndef MSG_DONTWAIT
#define	MSG_DONTWAIT	0
#endif

void ServerAcceptProc::run()
{
	for (;;) {
		struct sockaddr_in addr;
#ifndef WIN32
		socklen_t len;
#else
		int len = sizeof(struct sockaddr_in);
#endif
		// waiting for the connection request
		int s = accept(m_sock, (sockaddr*)&addr, &len);
		if (s < 0) {
			LOG_ERR(("cannot accept"));
			break;
		} 

		char *hostname = inet_ntoa(addr.sin_addr);
		char buf[1024]; // TODO: Magic number should be removed

		// Recieving message
#ifndef WIN32
		int r = read(s, buf, sizeof(buf));
#else
		int r = recv(s, buf, sizeof(buf), 0);
#endif
		if (r <= 0) { continue; }

		Source *src = new Source(s, hostname);

		if(strncmp(buf, "SIGViewer", 9) == 0) {
			char *p = buf;
			char *tmp = strtok(p,",");
			
			std::string sname = strtok(NULL,",");
			sname[sname.size() ] = '\0';
			
			//const char *name = sname.c_str();
			if(sname == "SIGSTART"){
				src->set(SOURCE_TYPE_NEW_VIEW, "SIGSTART");
			}
			else {
				LOG_SYS(("%s is connected", tmp));
				src->set(SOURCE_TYPE_NEW_VIEW, "SIGViewer");
			}
			Connection *con = new Connection(src, m_decoderL, m_sender);
			m_clients.push_back(con);

		} 
		else if(strncmp(buf, "SIGMESSAGE", 10) == 0) {

			char* p = buf;
			char *tmp = strtok(p,",");
			std::string sname = strtok(NULL,",");
#ifndef WIN32
			sname[sname.size()] = '\0';
#endif
			
			const char *name = sname.c_str();
			
			src->set(SOURCE_TYPE_MESSAGE, name);
			Connection *con = new Connection(src, m_decoderL, m_sender);
			
			bool same_name = false;
			
			int size = m_messages.size();
			
			std::vector<SOCKET> socks;
			if(size > 0) {
				for(int i = 0; i < size; i++) {
					
					Source *msg_src = m_messages[i]->source;
					if(strcmp(name, msg_src->name() ) == 0) {
						same_name = true;
						break;
					}
					
					int sock = msg_src->socket();
					socks.push_back(sock);
				}
				if(!same_name) {
					for(int i = 0; i < size; i++) {
						int sendSize = sname.size() + sizeof(unsigned short) * 2;
						char *sendBuff = new char[sendSize];
						char *p = sendBuff;
						
						BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0002); 
						BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize); 
						
						memcpy(p, sname.c_str(), sname.size()); 
						
						send(socks[i], sendBuff, sendSize, MSG_DONTWAIT);
						delete sendBuff;		     
					}
				}
			}

			//char result[1];	
			if (same_name) {
				LOG_ERR(("Service name \"%s\" already exist.", sname.c_str()));
				LOG_ERR(("Service: \"%s\" cannot connect", sname.c_str()));

				//BINARY_SET_DATA_S(result, unsigned char, 0); 
				//BINARY_SET_DATA_S_INCR(p, unsigned short, 4); 	  
				//sleep(1000);
				send(s, "FAIL", 4, MSG_DONTWAIT);
				delete con;
			}
			else {
				m_messages.push_back(con);
				if(sname != "SIGEND")
					LOG_SYS(("Service: \"%s\" is available", sname.c_str()));
				//BINARY_SET_DATA_S(result, unsigned char, 1);
				send(s, "SUCC", 4, MSG_DONTWAIT);
			}
		} 
		else {
			CommDataDecoder decoder;
			decoder.setPacketSender(m_sender);
			decoder.setListener(this);
			int decoded;
			typedef CommDataDecoder::Result Result;
			Result *res = decoder.push(*src, buf, r, decoded);
			if (decoded > 0) {
				if (res) {
					assert(res->forwarded());
					assert(res->type() == COMM_INVOKE_CONTROLLER_ON_RECV_MESSAGE);
	    
					//LOG_SYS(("data forwarded"));
					delete src;
#ifndef WIN32
					::close(s);
#else
					closesocket(s);
#endif
					continue;
				}
	  
				LOG_SYS(("%s connected", src->hostname()));
				Connection *con = new Connection(src, m_decoderL, m_sender);
				lock();
				m_clients.push_back(con);
				unlock();
				LOG_DEBUG1(("# of connection : %d", m_clients.size()));
			} else {
				LOG_SYS(("%s connection refused : no type", src->hostname()));
				delete src;
#ifndef WIN32
				::close(s);
#else
				closesocket(s);
#endif
			}
		}
	}
}


void ServerAcceptProc::free_()
{
#ifdef DEPRECATED
	typedef CommandHash H;
	for (H::iterator i=m_commandH.begin(); i!=m_commandH.end(); i++) {
		CommandQ *q = i->second;
		while (!q->empty()) {
			Command *c = q->front();
			q->pop();
			delete c;
		}
		delete q;
	}
	m_commandH.clear();
#endif // DEPRECATED
}

void ServerAcceptProc::close() {
	for (C::iterator i=m_clients.begin(); i!=m_clients.end(); i++) {
		Connection *con = *i;
		delete con;
	}
	m_clients.clear();
}

Source * ServerAcceptProc::get(const char *name, SourceType type)
{
	for (C::iterator i=m_clients.begin(); i!=m_clients.end(); i++) {
		Connection *con = *i;
		Source *client = con->source; 
		if (client->type() == type &&
			strcmp(client->name(), name) == 0) {
			return client;
		}
	}

	for (C::iterator i=m_messages.begin(); i!=m_messages.end(); i++) {
		Connection *con = *i;
		Source *client = con->source; 
		if (client->type() == type &&
			strcmp(client->name(), name) == 0) {
			return client;
		}
	}

	return 0;
}
std::vector<Source*> ServerAcceptProc::getAllClients()
{
	std::vector<Source*> srcs;

	for (C::iterator i=m_clients.begin(); i!=m_clients.end(); i++) {
		Connection *con = *i;
		Source *client = con->source; 
		if(client->type() == SOURCE_TYPE_CONTROLLER_CMD) {
			srcs.push_back(client);      
		}
	}

	for (C::iterator i=m_messages.begin(); i!=m_messages.end(); i++) {
		Connection *con = *i;
		srcs.push_back(con->source);
	}
	return srcs;
}

std::vector<Source*> ServerAcceptProc::getAllCtlClients()
{
	std::vector<Source*> srcs;
 
	for (C::iterator i=m_clients.begin(); i!=m_clients.end(); i++) {
		Connection *con = *i;
		Source *client = con->source; 
		if(client->type() == SOURCE_TYPE_CONTROLLER_CMD) {
			srcs.push_back(client);      
		}
	}

	return srcs;
}

std::vector<Source*> ServerAcceptProc::getAllSrvClients()
{
	std::vector<Source*> srcs;
 
	for (C::iterator i=m_messages.begin(); i!=m_messages.end(); i++) {
		Connection *con = *i;
		srcs.push_back(con->source);
	}
	return srcs;
}

Source * ServerAcceptProc::get(int sock)
{
	for (C::iterator i=m_clients.begin(); i!=m_clients.end(); i++) {
		Connection *con = *i;
		Source *client = con->source;
		if (client->equals(sock)) {
			return client;
		}
	}
	return 0;
}

Source * ServerAcceptProc::getByType(SourceType type)
{
	for (C::iterator i=m_clients.begin(); i!=m_clients.end(); i++) {
		Connection *con = *i;
		Source *client = con->source;
		if (client->type() == type) {
			return client;
		}
	}
	return 0;
}

void ServerAcceptProc::eraseNoNeeded()
{
	if (m_noneeded.size() <= 0) { return; }

	//LOG_SYSTEM(("erase %d socks", (int)m_noneeded.size()));

	for (SourceC::iterator i=m_noneeded.begin(); i!=m_noneeded.end(); i++) {
		Source *source = *i;
		for (ConC::iterator j=m_clients.begin();j!=m_clients.end(); j++) {
			Connection *con = *j;
			if (con->source == source) {
				/*
				  LOG_SYSTEM(("erase controller of %s",
				  source->name()));
				*/
				if (source->isControllerCmd()) {
					SSimWorld * w = m_wProvider.get();
					const char *name = source->name();
					SSimObj *obj = w->getSObj(name);
					if (obj) {
						obj->setAttached(false);
					}
					LOG_SYSTEM(("\"%s\" is detached\n", name));
				}
				m_clients.erase(j);
				delete con;
				break;
			}
		}

		for (ConC::iterator j=m_messages.begin();j!=m_messages.end(); j++) {
			Connection *con = *j;
			if (con->source == source) {
				LOG_SYSTEM(("erase service of %s",
							source->name()));

				if (source->isControllerCmd()) {
					SSimWorld * w = m_wProvider.get();
					const char *name = source->name();
					SSimObj *obj = w->getSObj(name);
					if (obj) {
						obj->setAttached(false);
					}
					LOG_SYSTEM(("\"%s\" is detached\n", name));
				}
				m_messages.erase(j);
				delete con;
				break;
			}
		}


	}


	//LOG_SYSTEM(("connection %d left", m_clients.size()));
	m_noneeded.clear();
}

#ifdef DEPRECATED
void ServerAcceptProc::applyCommands(SSimWorld &w)
{
	typedef CommandHash H;
	H &h = m_commandH;
	if (h.size() <= 0) { return; }
	double t = w.time();
	for (H::iterator i=h.begin(); i!=h.end(); i++) {
		S name = i->first;
		CommandQ *q = i->second;
		if (q->empty()) { continue; }
		Command *cmd = q->front();
		SSimObj *obj = w.getSObj(name.c_str());
		if (obj == 0) {
			LOG_ERR(("no agent : \"%s\"", name.c_str()));
			continue;
		}
		if (cmd->apply(t, *obj)) {
			//cmd->dump();
			q->pop();
			delete cmd;
		}

	}
}


ServerAcceptProc::CommandQ & ServerAcceptProc::getCommandQ(const char *name)
{
	typedef CommandHash H;
	H &h = m_commandH;

	CommandQ *q = h[S(name)];
	if (!q) {
		q = new CommandQ;
		h[S(name)] = q;
	}
	return *q;
}

void ServerAcceptProc::push(Command *cmd)
{
	CommandQ &q = getCommandQ(cmd->target());
	if (q.size() >= 100) {
		LOG_ERR(("Command to %s : Queue is full. Ignored", cmd->target()));
		delete cmd;
		return;
	}
	q.push(cmd);
}

#endif // DEPRECATED

void ServerAcceptProc::recvRequestAttachController(Source &from, RequestAttachControllerEvent &evt)
{
	//added by okamoto@tome (2011/12/13)
	if(m_startReq)
		{
#ifndef WIN32
			sleep(1);
#else
			Sleep(1000);
#endif
			m_startReq = false;
		}

	const char *agentName = evt.getAgentName();
	Source *client = get(agentName, SOURCE_TYPE_CONTROLLER_CMD);
	CommResultType r;
	if (client) {
		r = COMM_RESULT_ERROR;
		LOG_ERR(("\"%s\" : Controller is already attached", agentName));
	} else {

		SSimWorld * w = m_wProvider.get();
		SSimObj *obj = w->getSObj(agentName);
		if (obj) {
			from.set(SOURCE_TYPE_CONTROLLER_CMD, agentName);
			obj->setAttached(true);
			r = COMM_RESULT_OK;
			LOG_SYS(("Controller attached to \"%s\"", agentName));
		} else {
			r = COMM_RESULT_NO_AGENT;
			LOG_ERR(("\"%s\" DOES NOT exist in the world", agentName));
		}
	}

	CommResultEncoder enc(COMM_RESULT_ATTACH_CONTROLLER, r, 0);
	from.send(enc);
}

void ServerAcceptProc::recvRequestAttachView(Source &from, RequestAttachViewEvent &evt)
{
	const char *viewName = evt.getViewName();
	CommResultType r;

	LOG_SYS(("attach view : %s", evt.isPolling()?"polling":"not polling"));
	from.set(SOURCE_TYPE_VIEW, viewName);
	from.setProperty("polling", evt.isPolling()? "1":"0");
	r = COMM_RESULT_OK;

	X3DDB &db = m_wProvider.getX3DDB();
	const char *msg_ = getenv("SIGVERSE_DATADIR");
	std::string d;

	if (msg_ != NULL) {
		d = msg_;
		d += "/shape";
	}
	CommResultAttachViewEncoder enc(r,d.c_str(), db);

	from.send(enc);
}

void ServerAcceptProc::recvRequestProvideService(Source &from, RequestProvideServiceEvent &evt)
{
	LOG_DEBUG1(("service provider\n"))
		from.set(SOURCE_TYPE_SERVICE_PROVIDER, evt.getProviderName());
	CommResultEncoder enc(COMM_RESULT_PROVIDE_SERVICE, COMM_RESULT_OK, 0);
	from.send(enc);
	Service *service = evt.releaseService();
	if (service) {
		if (service->hostname() == NULL) {
			service->hostname(from.hostname());
		}
		m_ns.registerService(service);
		LOG_SYS(("%s:%d %s : service provider",
				 service->hostname(),
				 service->port(),
				 evt.getProviderName()));
	}

}

void ServerAcceptProc::recvRequestConnectDataPort(Source &from, RequestConnectDataPortEvent &evt)
{
	//sleep(1);
	from.set(SOURCE_TYPE_CONTROLLER_DATA, evt.getName());
	CommResultEncoder enc(COMM_RESULT_CONNECT_DATA_PORT, COMM_RESULT_OK, 0);
	from.send(enc);

	LOG_SYS(("%s : dataport", evt.getName()));
}

#endif

