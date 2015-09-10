/*
 * Created by Okamoto on 2011-03-25
 */

#include <sigverse/commonlib/ct/CTReader.h>
#include <sigverse/commonlib/CommDataEncoder.h>
#include <sigverse/commonlib/Logger.h>
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/ControllerInf.h>
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/comm/controller/RecvMessage.h>
#include <sigverse/commonlib/comm/controller/NoData.h>
#include <sigverse/commonlib/comm/controller/Action.h>

#ifndef WIN32
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

#else
#include <sys/timeb.h>

void gettimeofday(struct timeval *tv, struct timezone *tz)
{
	_timeb tvb;
	_ftime_s(&tvb);

	tv->tv_sec = (long)tvb.time;
	tv->tv_usec = tvb.millitm * 1000;
	return;
}
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE 32
#endif

#define SIGHEADER_LEN       4
#define SIG_MSG_HEADER_LEN  4
#define USE_SIMULATION_TIME 0

class ControllerInf;
class RecvMsgEvent;
class InitEvent;
class ActionEvent;

CTReader::CTReader(SOCKET s, CommDataDecoder &d, int bufsize)  : m_sock(s), m_source(s), m_decoder(d), m_buf(NULL)
{
	m_start_sim=false;
	m_timewidth=1000;
	m_start.tv_sec=m_start.tv_usec = 0;
	m_eoa.tv_sec = m_eoa.tv_usec = 0;
	m_server_startTime = 0;
	m_eoa_time=0;
	m_flag=true;
	m_start_onInit=false;
	m_buf = new Buffer(bufsize);
	m_source.set(SOURCE_TYPE_SIMSERVER, "simserver");
}

CTReader::~CTReader()
{
	delete m_buf;
}

void CTReader::clearSockBuffer()
{
	fd_set rfds;
	struct timeval tv = {0,1};
	int ret;

	while(1){
		FD_ZERO(&rfds);
		FD_SET(m_sock, &rfds);

		ret = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
		if(ret > 0){
			char tmpbuf[1024];
			recv(m_sock, tmpbuf, 1024, 0);
		}else{
			return;
		}
	}
	return;
}

bool CTReader::read()
{
	Controller *con = (Controller *)m_decoder.getController();
	struct timeval now;

	if (m_start_sim && m_flag) {

#if USE_SIMULATION_TIME
		//// We should clear socket buffer before calling getSimulationTime().
		clearSockBuffer();
		double current_sim_time = con->getSimulationTime();

		//now = clock();
		if (current_sim_time > 1000000) { /// Something wrong might be occured..
			fprintf(stderr, "ERROR in getSimulationTime(); %lf\n", current_sim_time);
		}
#endif


#if USE_SIMULATION_TIME
		double tmp_time = current_sim_time - m_eoa_time;
#else
		gettimeofday(&now, NULL);
		double tmp_time = (double)(now.tv_sec - m_eoa.tv_sec) + (double)(now.tv_usec - m_eoa.tv_usec) * 0.000001;
#endif

		if (tmp_time >= m_timewidth)
		{
			ActionEvent aevt;
#if USE_SIMULATION_TIME
			aevt.setTime(current_sim_time);
#else
			double nowtime = (double)(now.tv_sec - m_start.tv_sec) + (double)(now.tv_usec - m_start.tv_usec) * 0.000001 + m_server_startTime;
			aevt.setTime(nowtime);
#endif
			m_timewidth = con->onAction(aevt);

#if USE_SIMULATION_TIME
			m_eoa_time = current_sim_time;
#else
			gettimeofday(&m_eoa, NULL);
#endif
		}
	}

	//// check comming data from other modules
	SOCKET s = m_sock;

	fd_set rfds;
	struct timeval tv = {0,1000};
	FD_ZERO(&rfds);
	FD_SET(m_sock, &rfds);

	ControllerImpl* coni = (ControllerImpl*)con;
	std::map<std::string, SOCKET> ssocks = coni->getSrvSocks();
	std::map<std::string, SOCKET>::iterator it = ssocks.begin();

	while (it != ssocks.end()) {
		FD_SET((SOCKET)((*it).second), &rfds);
		it++;
	}

	int ret = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);

	/////////////////////

	if (ret == -1) {        /// Error in select function.
		perror("select");
		return false;
	}
	else if (ret == 0) {  ///  No data comming...
		return true;
	}
	else if (ret > 0) { /// arrive data from other modules.
		//// First, process main data port connected with sigserver.
		///
		if (FD_ISSET(s, &rfds)) {
			int rbytes;

			if (m_buf->datasize() == 0) {
				rbytes = m_buf->read(s, SIGHEADER_LEN);

				if (rbytes < 0) {  /// Error occored in reading socket.
#ifndef WIN32
					if (errno == ECONNRESET) {
						LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
					} else {
						LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
					}
#endif
					throw ConnectionClosedException();
				}

				if (rbytes == SIGHEADER_LEN) { ////  

					char *data = m_buf->data();
					char *p = data;
					unsigned short token = BINARY_GET_DATA_S_INCR(p, unsigned short);

					if (token == COMM_DATA_PACKET_START_TOKEN) {  /// Accept COMM_DATA typed command packet, which start with '0xabcd'.
						unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);

						// sekikawa(FIX20100826)
						int rbytes2 = m_buf->read(s, size - SIGHEADER_LEN);  // read remains data

						if (rbytes2 < 0) {
#ifndef WIN32
							if (errno == ECONNRESET) {
								LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
							} else {
								LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
							}
#endif
							throw ConnectionClosedException();
						}
						rbytes += rbytes2;

					}
					else if (token == START_SIM) { /// Simulation of SigServer started.

						if (!m_start_sim) {
							m_start_sim = true;

							char tmpBuff[sizeof(double)];
							if (!recvData(s,  tmpBuff, sizeof(double))) {
								LOG_ERR(("Controller: failed to get world current time"));
							}

							m_server_startTime = BINARY_GET_DOUBLE(tmpBuff);
#if !USE_SIMULATION_TIME
							gettimeofday(&m_start, NULL);
#endif
							con->setSimState(true);

							if (!m_start_onInit) {
								InitEvent evt;
								con->onInit(evt);
								m_start_onInit = true;
							}
#if USE_SIMULATION_TIME
							double nowtime = con->getSimulationTime();
#else
							gettimeofday(&now, NULL);
							double nowtime = (double)(now.tv_sec - m_start.tv_sec) + (double)(now.tv_usec - m_start.tv_usec) * 0.000001;
							nowtime += m_server_startTime;
#endif
							ActionEvent aevt;

							aevt.setTime(nowtime);
							m_timewidth = con->onAction(aevt);
							//con->updateObjs();
							m_eoa_time = nowtime;
							// eoa(end of onAction)
#if !USE_SIMULATION_TIME
							gettimeofday(&m_eoa, NULL);
#endif
							m_buf->setDecodedByte(SIGHEADER_LEN);
							return true;
						}
					}

					else if (token == STOP_SIM) {   /// Stop simulation on the SIGServer.
						m_start_sim = false;

						con->setSimState(false);
						m_buf->setDecodedByte(SIGHEADER_LEN);
						return true;
					}

					else if (token == SEND_MESSAGE) {   /// Reccive message with the string based message commands from other agent.
						unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
						size -= SIGHEADER_LEN;

						char *tmpBuff = new char[size + 1];
						memset(tmpBuff, 0, sizeof(tmpBuff));

						if (!recvData(s,  tmpBuff, size)) {
							LOG_ERR(("Controller: failed to recv message data."));
						}

						tmpBuff[size] = '\0';

						RecvMsgEvent msg;
						msg.setData(tmpBuff, size);

						con->onRecvMsg(msg);

						//Controller *conn = (Controller*)con;
						//conn->updateObjs();

						m_buf->setDecodedByte(SIGHEADER_LEN);

						delete [] tmpBuff;
						return true;
					}
					else {   //// It might be error....
						LOG_ERR(("Could not find packet start token. [%d], %d", token, rbytes));
						return false;
					}
				}else{  ///  It seems an communication error....
					if (rbytes == 0) {  throw ConnectionClosedException(); }

					LOG_ERR(("Could not get enough header packet..... %d bytes read.",rbytes));
					return false;
				}
			}
			else {   /// Remains data when the previous message recieved...

				rbytes = m_buf->read(s);

				// sekikawa(FIX20100826)
				if (rbytes <= 0) {
#ifndef WIN32
					if (errno == ECONNRESET) {
						LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
					} else {
						LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
					}
#endif
					throw ConnectionClosedException();
				}
			}

			///////// Create and execute decoder from recieved packet.
			LOG_DEBUG1(("%d bytes read", rbytes));

			if (m_buf->datasize() > 0) {
				char *data = m_buf->data();
				int datasize = m_buf->datasize();
				int r = m_decoder.push(m_source, data, datasize);
				if (r >= 0) {
					m_buf->setDecodedByte(r);
					return true;
				}
			}

			ret--;
		} //   end of the procedure from SIGServe:'m_sock'

		/// Other messages from other services found.
		if(ret > 0) 
		{
			std::map<std::string, SOCKET>::iterator it = ssocks.begin();

			while (ret > 0 && it != ssocks.end()) {
				SOCKET sock = (*it).second;  
				std::string srv_name = (*it).first.c_str();

				if (FD_ISSET(sock, &rfds)) {
					char tmp[SIG_MSG_HEADER_LEN]; 
					char *p = tmp;

					if (!recvData(sock, tmp, SIG_MSG_HEADER_LEN)) {
						LOG_SYS(("disconnected from service [%s]", srv_name.c_str()));
						coni->deleteService(srv_name);
						it++;
						continue;
					}

					unsigned short n = BINARY_GET_DATA_S_INCR(p, unsigned short);

					int size = BINARY_GET_DATA_S_INCR(p, unsigned short);

					if (n == SIGHEADER_LEN) {
						LOG_SYS(("disconnected service \"%s\"", srv_name.c_str()));
						coni->deleteService(srv_name);
					} 

					size -= SIG_MSG_HEADER_LEN;

					if (size == 0){   /// No command body found.
						continue;
					}

					char *recvBuff = new char[size];
					if (!recvData(sock, recvBuff, size)) {
						coni->deleteService(srv_name);
						delete [] recvBuff;
					}

					switch(n) {
						case 2:  //// Recieve message command from other serivice
						{
							if (con->getSimState()) {
								RecvMsgEvent msg;
								msg.setData(recvBuff, size);
								con->onRecvMsg(msg);
							}
							break;
						} 
					} // switch(n) {
					ret--;

				} //if (FD_ISSET(sock, &rfds)) {
				it++;

			}// while (it != ssocks.end()) {
		} // if(ret > 0)
	} // else if (ret > 0)
	return true;
}


CommDataDecoder::Result * CTReader::readSync()
{
	SOCKET s = m_sock;

	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(s, &rfds);

	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	int ret = select(s+1, &rfds, NULL, NULL, &tv);
	if (ret == -1) {
		perror("select");
		return false;
	}
	if (ret > 0) {
		int rbytes;
		if (m_buf->datasize() == 0) {
			rbytes = m_buf->read(s, 4);
#if 1
			// sekikawa(FIX20100826)
			if (rbytes < 0) {
#ifndef WIN32
				if (errno == ECONNRESET) {
					LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
				} else {
					LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
				}
#endif
				throw ConnectionClosedException();
			}
#endif
			if (rbytes > 0) {
				char *data = m_buf->data();
				char *p = data;
				unsigned short token = BINARY_GET_DATA_S_INCR(p, unsigned short);
				if (token != COMM_DATA_PACKET_START_TOKEN) {
					return false;
				}

				unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);

#if 1
				// sekikawa(FIX20100826)
				int rbytes2 = m_buf->read(s, size-4);
				if (rbytes2 < 0) {
#ifndef WIN32
					if (errno == ECONNRESET) {
						LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
					} else {
						LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
					}
#endif
					throw ConnectionClosedException();
				}
				rbytes += rbytes2;
#else
				// orig
				rbytes += m_buf->read(s, size-4);
#endif
			}
		} else {
			rbytes = m_buf->read(s);

#if 1
			// sekikawa(FIX20100826)
			if (rbytes < 0) {
#ifndef WIN32
				if (errno == ECONNRESET) {
					LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
				} else {
					LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
				}
#endif
				throw ConnectionClosedException();
			}
#endif
		}
		LOG_DEBUG1(("%d bytes read", rbytes));
		if (m_buf->datasize() > 0) {

			char *data = m_buf->data();
			int datasize = m_buf->datasize();
			int r = -1;
			Result *result = m_decoder.push(m_source, data, datasize, r);
			if (r >= 0) {
				m_buf->setDecodedByte(r);
				return result;
			}
		}
#if 0
		// sekikawa(FIX20100826)
		else if (rbytes == 0) {
			// rbytes=0 is not error.
			// when connection is gracefully closed, rbytes becomes 0.
			return NULL;
		} else if (rbytes < 0) {
			// socket error occurred
			LOG_ERR(("socket error [%s:%d]", rbytes, __FILE__, __LINE__));
			throw ConnectionClosedException();
		}
#else
		// orig
		else if (rbytes == 0) {
			throw ConnectionClosedException();
		}
#endif
	}
	return NULL;
}


/*
 * CTReader::Buffer
 *
 */
CTReader::Buffer::Buffer(int size) : m_bufsize(size), m_readHead(NULL), m_curr(0) {
	int i = 0;
	m_buf[i] = new char[size]; i++;
	m_buf[i] = new char[size]; i++;
	m_readHead = m_buf[m_curr];
}

CTReader::Buffer::~Buffer() {
	int i=0;
	delete [] m_buf[i]; i++;
	delete [] m_buf[i]; i++;
}

int CTReader::Buffer::read(SOCKET s)
{
	char *currbuf = m_buf[m_curr];
	int n = m_bufsize - (m_readHead - currbuf);

#ifdef WIN32
	int rbytes = ::recv(s, m_readHead, n, 0);
	if (rbytes == SOCKET_ERROR){
		int err = WSAGetLastError();
		if ( err == WSAEINTR ||
			 err == WSAEINPROGRESS ||
			 err == WSAEWOULDBLOCK ) {
				 return 0;
		}
		return -1;
	}

#else
	int rbytes = ::read(s, m_readHead, n);
	if (rbytes <= 0) { return rbytes; }
#endif

	m_readHead += rbytes;
	return rbytes;
}

int CTReader::Buffer::read(SOCKET s, int bytes)
{
#ifdef WIN32
	int rbytes = ::recv(s, m_readHead, bytes, 0);
	if (rbytes == SOCKET_ERROR){
		int err = WSAGetLastError();
		if ( err == WSAEINTR ||
			 err == WSAEINPROGRESS ||
			 err == WSAEWOULDBLOCK ) {
				 return 0;
		}
		return -1;
	}
#else
	int rbytes = ::read(s, m_readHead, bytes);
	if (rbytes <= 0) { return rbytes; }
#endif

	m_readHead += rbytes;
	return rbytes;
}

void CTReader::Buffer::setDecodedByte(int decoded)
{
	int left = datasize() - decoded;

	if (left > 0) {
		LOG_DEBUG1(("%d bytes left", left));
		char *currbuf = m_buf[m_curr];
		int next = (m_curr+1) % 2;
		char *nextbuf = m_buf[next];
		memcpy(nextbuf, &currbuf[decoded], left);
		m_readHead = &nextbuf[left];
		m_curr = next;

	} else if (left == 0) {
		m_readHead = m_buf[m_curr];
	}
}

bool CTReader::recvData(SOCKET sock, char* msg, int size)
{
	int recieved = 0;
	while (1) {

		int r = recv(sock, msg + recieved, size - recieved, 0);
#ifndef WIN32
		if (r < 0) 
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) {

				sleep(0.1);
				continue;
			}
			LOG_ERR(("Controller: Failed to recieve data. erro[%d]",r));
			return false;
		}
#else
		if (r == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if ( err == WSAEINTR ||
				 err == WSAEINPROGRESS ||
				 err == WSAEWOULDBLOCK ) {
				 Sleep(100);
				 continue;
			}
			LOG_ERR(("Controller: Failed to recieve data. erro[%d]",r));
			return false;
		}
#endif

		recieved += r;

		if (size == recieved) break;
	}
	return true;
}

