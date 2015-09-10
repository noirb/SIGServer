/*
 * Modified by okamoto on 2011-03-31
 */

#include <sigverse/commonlib/Source.h>

#include <sigverse/commonlib/CommDataEncoder.h>
#include <sigverse/commonlib/CommUtil.h>

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
