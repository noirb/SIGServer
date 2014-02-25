#include "JspSocket.h"
#include "JspPacket.h"
#include "JspLog.h"
#include "JspWave.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Jsp
{
	Socket::Socket() : m_sock(-1) {}
	Socket::~Socket() {
		close();
	}

	bool Socket::connect(const char *hostname, int port) {

		int sock = ::socket(AF_INET, SOCK_STREAM, 0);

		if (sock < 0) {
			Jsp::printLog(LOG_FATAL, "cannot open socket\n");
			return false;
		}

		struct hostent *hp = gethostbyname(hostname);
		if (!hp) {
			Jsp::printLog(LOG_FATAL, "unknown host : %s\n", hostname);
			return false;
		}

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;

		bcopy((char *)hp->h_addr, (char*)&addr.sin_addr, hp->h_length);

		addr.sin_port = htons(port);

		if (::connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			Jsp::printLog(LOG_FATAL, "cannot connect server\n");
			return false;
		}

		m_sock = sock;

		return true;
	}

	bool Socket::bind(int port) {
		int sock = ::socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0) {
			Jsp::printLog(LOG_FATAL, "cannot open socket\n");
			return false;
		}

		struct sockaddr_in addr;

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);

		::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, NULL, 0);

		if (::bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			Jsp::printLog(LOG_FATAL, "cannot bind socket\n");
			return false;
		}

		::listen(sock, 5);

		m_sock = sock;

		return true;
	}

	bool Socket::accept(Jsp::Socket &newSock, std::string &hostName) {
		struct sockaddr_in addr;
		socklen_t len;

		int s = ::accept(m_sock, (sockaddr*)&addr, &len);
		if (s < 0)
		{
			Jsp::printLog(LOG_FATAL, "accept failed. (errno=%d)\n", errno);
			return false;
		}

		newSock.m_sock = s;
		hostName = inet_ntoa(addr.sin_addr);

		return true;
	}

	void Socket::close() {
		if (m_sock >= 0) {
			::close(m_sock);
			m_sock = -1;
		}
	}

	int Socket::sendData(char *p, int len) {
		if (!connected()) return -1;

		int sentBytes;

		// first, send data size
		// datasize == 0 means end of segment
		sentBytes = ::write(m_sock, (char *)&len, sizeof(int));
		if (sentBytes != sizeof(int)) {
			Jsp::printLog(LOG_FATAL, "failed to write length of data\n");
			return(-1);
		}

		if (len > 0) {
			sentBytes = ::write(m_sock, p, len);
			if (sentBytes < 0) {
				Jsp::printLog(LOG_FATAL, "failed to write data (%d bytes)\n", len);
				return(-1);
			}
		} else {
			sentBytes = 0;
		}

		return(sentBytes);
	}

	int Socket::recvData(char *p, int bufLen, int &recvLen) {

		recvLen = ::read(m_sock, p, bufLen);

		if (recvLen < 0)
		{
			// error
			Jsp::printLog(LOG_FATAL, "Error at recvData() : errno = %d", errno);

			return -1;
		}
		else if (recvLen == 0)
		{
			// connection is gracefully closed (by opponents, in blocking mode)
			// or no data reached. (from opponents, in non-blocking mode)
			return 0;
		}

		// success
		return 1;
	}

	bool Socket::sendWaveFile(const char *waveFile) {
		Jsp::Wave wave;

		if (!wave.loadFromFile(waveFile)) return false;

		char *p = wave.getWaveDataPtr();
		int len = wave.getWaveDataLen();

		if (sendData(p, len) < 0) return false;

		return true;
	}

	char *Socket::getTextData(char *buf, int buflen) {
		int c;
		char *p = buf;

		for (;;) {
			c = ::read(m_sock, p, 1);

			if (c <= 0) return NULL;	// error or EOF
			if ((*p == '\n') && (p > buf)) {
				*p = '\0';
				break;
			} else {
				if (++p >= (buf + buflen)) {
					Jsp::printLog(LOG_FATAL, "[ERROR] short of buflen. you need longer buffer\n");
					exit(1);
				}
			}
		}

		return buf;
	}

	int Socket::recvPacket(Jsp::Packet **ppPacket)
	{
		if (!ppPacket) return -1;

		Jsp::Packet *packet = new Packet(Packet::HEAD_SIZE_LEN);
		if (!packet)
		{
			Jsp::printLog(LOG_FATAL, "out of memory [%s:%d]\n", __FILE__, __LINE__);
			return -1;
		}

		int status;
		int totalRecvLen;
		int recvlen;

		// -----------------------------------------
		//	read header
		// -----------------------------------------
		totalRecvLen = 0;
		while (totalRecvLen < Packet::HEAD_SIZE_LEN)
		{
			status = recvData(packet->getData() + totalRecvLen, Packet::HEAD_SIZE_LEN - totalRecvLen, recvlen);
			if (status <= 0) return status;

			totalRecvLen += recvlen;
		}

		// -----------------------------------------
		//	check packet start mark
		// -----------------------------------------
		unsigned short headMark = ntohs(*((unsigned short *)packet->getData()));
		if (headMark != Packet::HEAD_MARK)
		{
			Jsp::printLog(LOG_FATAL, "head mark (0x%04X) is wrong. packet is broken!\n", headMark);
			return -1;
		}

		unsigned short packetLen = ntohs(*((unsigned short *)(packet->getData()+sizeof(Packet::HEAD_MARK))));

		// -----------------------------------------
		//	prepare buffer for storing data
		// -----------------------------------------
		if (!packet->resize(packetLen))
		{
			Jsp::printLog(LOG_FATAL, "out of memory [%s:%d]\n", __FILE__, __LINE__);
			return -1;
		}

		// -----------------------------------------
		//	read until total packet size is filled.
		// -----------------------------------------
		while (totalRecvLen < packetLen)
		{
			status = recvData(packet->getData() + totalRecvLen, packetLen-totalRecvLen, recvlen);
			if (status <= 0) return status;

			totalRecvLen += recvlen;
		}

#if 0
// debug
		FILE *dumpFp = fopen("packet.dat", "w");
		if (dumpFp)
		{
			fwrite(packet->getData(), 1, packet->getLength(), dumpFp);
			fclose(dumpFp);
		}
#endif

		// -----------------------------------------
		//	check packet end mark
		// -----------------------------------------
		unsigned short tailMark = ntohs(*((unsigned short *)(packet->getData() + totalRecvLen - sizeof(Packet::TAIL_MARK))));
		if (tailMark != Packet::TAIL_MARK)
		{
			Jsp::printLog(LOG_FATAL, "tail mark (0x%04X) is wrong. packet is broken!\n", tailMark);
			return -1;
		}

		// debug
		Jsp::printLog(LOG_DEBUG, "totalRecvLen : %d\n", totalRecvLen);

		*ppPacket = packet;

		return 1;
	}
};
