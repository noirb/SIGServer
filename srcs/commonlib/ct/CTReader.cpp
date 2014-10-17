/*
 * Created by Okamoto on 2011-03-25
 */

#include "CTReader.h"
#include "CommDataEncoder.h"
#include "Logger.h"
#include "binary.h"
#include "ControllerInf.h"
#include "Controller.h"
#include "comm/controller/RecvMessage.h"
#include "comm/controller/NoData.h"
#include "comm/controller/Action.h"

#ifndef WIN32
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

#else
#include <sys/timeb.h>

void
gettimeofday(struct timeval *tv, struct timezone *tz)
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

class ControllerInf;
class RecvMsgEvent;
class InitEvent;
class ActionEvent;

CTReader::CTReader(SOCKET s, CommDataDecoder &d, int bufsize)  : m_sock(s), m_source(s), m_decoder(d), m_buf(NULL)
{
	m_buf = new Buffer(bufsize);
	m_source.set(SOURCE_TYPE_SIMSERVER, "simserver");
}

CTReader::~CTReader()
{
	delete m_buf;
}


//void gettimeofday(struct timeval *tv, struct timezone *tz);

bool CTReader::read()
{
	ControllerInf *con = m_decoder.getController();

	static bool start_sim;

	static double timewidth;

	static struct timeval start, eoa;
	struct timeval now; 

	static double server_startTime;

	if (start_sim) {

		//now = clock();
		gettimeofday(&now, NULL);

		double tmp_time = (double)(now.tv_sec - eoa.tv_sec) + (double)(now.tv_usec - eoa.tv_usec) * 0.001 * 0.001;

		if (tmp_time >= timewidth) {
			ActionEvent aevt;
      
			double nowtime = (double)(now.tv_sec - start.tv_sec) + (double)(now.tv_usec - start.tv_usec) * 0.001 * 0.001;

			nowtime += server_startTime;
			aevt.setTime(nowtime);
      
			timewidth = con->onAction(aevt);
//      fprintf(stderr, "=%lf,  %lf == ", tmp_time, timewidth);
			gettimeofday(&eoa, NULL);

			//Controller *conn = (Controller*)con;
			//conn->updateObjs();
		}
	}

	SOCKET s = m_sock;

	fd_set rfds;
	struct timeval tv = {0,1000};
	FD_ZERO(&rfds);
	FD_SET(s, &rfds);

	ControllerImpl* coni = (ControllerImpl*)con;
	std::map<std::string, SOCKET> ssocks = coni->getSrvSocks();
	std::map<std::string, SOCKET>::iterator it = ssocks.begin();
	while (it != ssocks.end()) {
		FD_SET((SOCKET)((*it).second), &rfds);
		it++;
	}
//	tv.tv_sec = 0;
	//tv.tv_usec = 100000;
//	tv.tv_usec = 1000; 
  
	int ret = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
	if (ret == -1) {
		perror("select");
		return false;

	} else if (ret == 0) {

		return true;
	}	else if (ret > 0) {

		if (FD_ISSET(s, &rfds)) {

			int rbytes;
			if (m_buf->datasize() == 0) {
				rbytes = m_buf->read(s, 4);
#if 1
				// sekikawa(FIX20100826)
				if (rbytes < 0) {
					if (errno == ECONNRESET) {
						LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
					} else {
						LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
					}
					throw ConnectionClosedException();
				}
#endif

				if (rbytes > 0) {

					char *data = m_buf->data();
					char *p = data;
					unsigned short token = BINARY_GET_DATA_S_INCR(p, unsigned short);

					if (token == COMM_DATA_PACKET_START_TOKEN) {
						unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
#if 1
						// sekikawa(FIX20100826)
						int rbytes2 = m_buf->read(s, size-4);
						if (rbytes2 < 0) {
							if (errno == ECONNRESET) {
								LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
							} else {
								LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
							}
	    
							throw ConnectionClosedException();
						}
						rbytes += rbytes2;
						/*
						  #else
						  // orig
						  rbytes += m_buf->read(s, size-4);
						  }
						*/
#endif
					}
				
					else if (token == START_SIM) {

						if (!start_sim) {

							start_sim = true;
							static bool start_onInit = false;

							char tmpBuff[sizeof(double) + 1];
							if (!recvData(s,  tmpBuff, sizeof(double))) {
								LOG_ERR(("Controller: failed to get world current time"));
							}

							server_startTime = BINARY_GET_DOUBLE(tmpBuff);

							gettimeofday(&start, NULL);
	    
							ControllerInf *con = m_decoder.getController();

							con->setSimState(true);
	    
							if (!start_onInit) {
								InitEvent evt;
								con->onInit(evt);
								start_onInit = true;
							}
							Controller *conn = (Controller*)con;

							gettimeofday(&now, NULL);

							double nowtime = (double)(now.tv_sec - start.tv_sec) + (double)(now.tv_usec - start.tv_usec) * 0.001 * 0.001;
							nowtime += server_startTime;

							ActionEvent aevt;
							aevt.setTime(nowtime);
							timewidth = con->onAction(aevt);
							conn->updateObjs();

							// eoa(end of onAction)
							gettimeofday(&eoa, NULL);

							m_buf->setDecodedByte(4);
						}
					}

					else if (token == STOP_SIM) {
						start_sim = false;

						ControllerInf *con = m_decoder.getController();

						con->setSimState(false);

						m_buf->setDecodedByte(4);
					}

					else if (token == SEND_MESSAGE) {

						unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
						size -= 4;

						char *tmpBuff = new char[size + 1];
						memset(tmpBuff, 0, sizeof(tmpBuff));

						if (!recvData(s,  tmpBuff, size)) {
							LOG_ERR(("Controller: failed to recv message data."));
						}
	  
						tmpBuff[size] = '\0';
	  
						ControllerInf *con = m_decoder.getController();

						RecvMsgEvent msg;

						msg.setData(tmpBuff, size);
	  
						con->onRecvMsg(msg);

						Controller *conn = (Controller*)con;
						//conn->updateObjs();

						m_buf->setDecodedByte(4);
	  
						delete tmpBuff;
						return true;
					}
					else {
						LOG_ERR(("Could not find packet start token. [%d]", token));
						return false;
					}
				}
			}
			else {
				rbytes = m_buf->read(s);
      
#if 1
				// sekikawa(FIX20100826)
				if (rbytes < 0) {
					if (errno == ECONNRESET) {
						LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
					} else {
						LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
					}
					throw ConnectionClosedException();
				}
#endif
			}

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
#if 0
			// sekikawa(FIX20100826)
			else if (rbytes == 0) {
				// rbytes=0 is not error.
				// when connection is gracefully closed, rbytes becomes 0.
				return false;
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

		} //if (FD_ISSET(s, &rfds)) {
		else{
			std::map<std::string, SOCKET>::iterator it = ssocks.begin();
			while (it != ssocks.end()) {
				SOCKET sock = (*it).second;  
				std::string srv_name = (*it).first.c_str(); 
				if (FD_ISSET(sock, &rfds)) {
					char tmp[4]; 
					char *p = tmp;
	  
					if (!recvData(sock, tmp, 4)) {
						LOG_SYS(("disconnected from service [%s]", srv_name.c_str()));
						coni->deleteService(srv_name);
						it++;
						continue;
					} 

					unsigned short n = BINARY_GET_DATA_S_INCR(p, unsigned short);
	  
					int size = BINARY_GET_DATA_S_INCR(p, unsigned short);

					if (n == 4) {
						LOG_SYS(("disconnected service \"%s\"", srv_name.c_str()));
						coni->deleteService(srv_name);
						//m_connected = false;
						//goto error;
					} // switch(n) {
      
					size -= 4;      

					if (size == 0) 
						continue;
	  
					char *recvBuff = new char[size];
					if (!recvData(sock, recvBuff, size)) {
						coni->deleteService(srv_name);
						delete [] recvBuff;
					}    

					switch(n) {

					case 2:
						{
							if (con->getSimState()) {
								RecvMsgEvent msg;
								msg.setData(recvBuff, size);
		
								con->onRecvMsg(msg);
		
								Controller* cont = (Controller*)con;
								//cont->updateObjs();
							}
							break;
						} 
					} // switch(n) {
				} //if (FD_ISSET(sock, &rfds)) {
				it++;
			}// while (it != ssocks.end()) {
		} //else
	} // else if (ret > 0)
	return false;
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
				if (errno == ECONNRESET) {
					LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
				} else {
					LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
				}
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
					if (errno == ECONNRESET) {
						LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
					} else {
						LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
					}
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
				if (errno == ECONNRESET) {
					LOG_SYS(("connection closed by service provider [%s:%d]", __FILE__, __LINE__));
				} else {
					LOG_SYS(("socket error (errno=%d) [%s:%d]", errno, __FILE__, __LINE__));
				}
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
#else
	int rbytes = ::read(s, m_readHead, n);
#endif
	if (rbytes <= 0) { return rbytes; }
	m_readHead += rbytes;
	return rbytes;
}

int CTReader::Buffer::read(SOCKET s, int bytes)
{
#ifdef WIN32
	int rbytes = ::recv(s, m_readHead, bytes, 0);
#else
	int rbytes = ::read(s, m_readHead, bytes);
#endif
	if (rbytes <= 0) { return rbytes; }
	m_readHead += rbytes;
	return rbytes;
}

void CTReader::Buffer::setDecodedByte(int decoded) {
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

bool CTReader::recvData(SOCKET sock, char* msg, int size) {
	int recieved = 0;
	while (1) {

		int r = recv(sock, msg + recieved, size - recieved, 0);
		if (r < 0) 
			{
				if (errno == EAGAIN ||
					errno == EWOULDBLOCK) {
#ifndef WIN32
					sleep(0.1);
#else
					Sleep(100);
#endif
					continue;
				}
				LOG_ERR(("Controller: Failed to recieve data. erro[%d]",r));
				return false;
			}

		recieved += r;

		if (size == recieved) break;
	}
	return true;
}

