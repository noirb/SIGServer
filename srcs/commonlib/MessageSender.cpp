/* $Id: MessageSender.cpp,v 1.3 2011-05-12 08:33:50 msi Exp $ */
#include "MessageSender.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"
#include "Logger.h"

////////////////////////////////////////////////////
//
MessageSender::~MessageSender()
{
	if (m_sock >= 0 && m_close) {
		CommUtil::disconnectServer(m_sock);
		m_sock = -1;
	}
}

bool MessageSender::connect(char *host, int port)
{
	m_sock = CommUtil::connectServer(host, port);
	return m_sock >= 0? true: false;
}

bool MessageSender::sendto(const char *to, int n, char **strs)
{
	if (m_sock < 0) { return false; }

	CommInvokeMethodOnRecvMessageEncoder enc(m_name.c_str(), to, n, strs);
	int ret = enc.send(m_sock);
	return ret > 0? true: false;
}

bool MessageSender::broadcast(int n, char **strs)
{
	return sendto(NULL, n, strs);
}

// begin(FIX20110401)
////////////////////////////////////////////////////
//
SoundSender::~SoundSender()
{
	if (m_sock >= 0 && m_close)
	{
		CommUtil::disconnectServer(m_sock);
		m_sock = -1;
	}
}

bool SoundSender::connect(char *host, int port)
{
	m_sock = CommUtil::connectServer(host, port);

	return (m_sock >= 0) ? true : false;
}

bool SoundSender::sendto(const char *to, RawSound &sound)
{
//	LOG_MSG(("SoundSender::sendto called"));

	if (m_sock < 0) { return false; }

	CommInvokeMethodOnRecvSoundEncoder enc(0, m_name.c_str(), to, sound);
	int ret = enc.send(m_sock);

	return (ret > 0) ? true : false;
}
// end(FIX20110401)
