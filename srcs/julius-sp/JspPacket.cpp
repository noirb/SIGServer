#include "JspPacket.h"
#include "JspLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>	// for ntohs()

using namespace std;

namespace Jsp
{
	// packet start mark
	const unsigned short Packet::HEAD_MARK = 0xABCD;

	// packet end mark
	const unsigned short Packet::TAIL_MARK = 0xDCBA;

	// total size of start mark and packet size, in bytes.
	const int Packet::HEAD_SIZE_LEN = sizeof(Packet::HEAD_MARK) + sizeof(Packet::PACKET_SIZE);

	// total size of stark mark, packet size, and packet type, in bytes.
	const int Packet::HEAD_SIZE_TYPE_LEN = sizeof(Packet::HEAD_MARK) + sizeof(Packet::PACKET_SIZE) + sizeof(Packet::DATA_TYPE);

	// size of end mark, in bytes.
	const int Packet::TAIL_LEN = sizeof(Packet::TAIL_MARK);

	Packet::Packet(int initBufLen) : m_pData(NULL)
	{
		allocMemory(initBufLen);
	}

	bool Packet::resize(int newBufLen)
	{
		return reallocMemory(newBufLen);
	}

	Packet::~Packet()
	{
		freeMemory();
	}

	CommDataType Packet::getType()
	{
		if (m_pData && (m_capacity >= HEAD_SIZE_TYPE_LEN))
		{
			CommDataType dataType = ntohs(*((CommDataType *)(m_pData + HEAD_SIZE_LEN)));

			return dataType;
		}

		return COMM_DATA_TYPE_NOT_SET;
	}

	void Packet::writeToFile(const char *fname, bool bAppend)
	{
		FILE *fp = fopen(fname, bAppend ? "a" : "w");
		if (fp)
		{
			fwrite(m_pData, 1, m_capacity, fp);
			fclose(fp);
		}
	}

	bool Packet::allocMemory(int len)
	{
		freeMemory();

		m_pData = (char *)malloc(sizeof(char) * len);

		if (m_pData)
		{
			m_capacity = len;
			return true;
		}
		else
		{
			Jsp::printLog(LOG_FATAL, "out of memory [%s:%d]\n", __FILE__, __LINE__);

			m_capacity = 0;
			return false;
		}
	}

	bool Packet::reallocMemory(int newLen)
	{
		m_pData = (char *)realloc(m_pData, sizeof(char) * newLen);
		if (!m_pData)
		{
			Jsp::printLog(LOG_FATAL, "realloc failed [%s:%d]\n", __FILE__, __LINE__);
			return false;
		}

		m_capacity = newLen;

		return true;
	}

	void Packet::freeMemory()
	{
		if (m_pData)
		{
			free(m_pData);
			m_pData = NULL;
		}

		m_capacity = 0;
	}
};
