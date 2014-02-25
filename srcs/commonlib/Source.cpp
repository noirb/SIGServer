/* $Id: Source.cpp,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */
#include "Source.h"

#ifdef EXEC_SIMULATION
#include "CommDataEncoder.h"
#include "CommUtil.h"

int Source::send(CommDataEncoder &enc)
{
	m_locker.lock();
	SOCKET sock = socket();
	int r = enc.send(sock);
	m_locker.unlock();
	return r;
}

int Source::send(char *data , int bytes)
{
	m_locker.lock();
	SOCKET sock = socket();
	int r = CommUtil::sendData(sock, data, bytes);
	m_locker.unlock();
	return r;
}

#endif
