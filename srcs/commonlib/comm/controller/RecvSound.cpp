/* $Id: RecvSound.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include "RecvSound.h"
#include "RawSound.h"
#include "binary.h"
#include "Header.h"

#include <assert.h>

#define FREE(P) if (P) { delete P; P = 0; }

void RecvSoundEvent::free_()
{
	FREE(m_sound);
	FREE(m_soundTmp);
}

bool RecvSoundEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	if (seq == 0) {
		m_currTime = BINARY_GET_DOUBLE_INCR(p);
		char * caller = BINARY_GET_STRING_INCR(p);
		if (caller) {
			m_caller = caller;
		}
		BINARY_FREE_STRING(caller);
		short channels = BINARY_GET_DATA_S_INCR(p, short);
		unsigned short samplingRate = BINARY_GET_DATA_S_INCR(p, unsigned short);
		unsigned short bitPerSample = BINARY_GET_DATA_S_INCR(p, unsigned short);
		RawSoundEndian endian = BINARY_GET_DATA_S_INCR(p, RawSoundEndian);
		unsigned datalen = BINARY_GET_DATA_L_INCR(p, unsigned);
		bool b = CommData::isPacketEnd(p);

		if (!b) { return false; }

		RawSoundHeader h(channels, samplingRate, bitPerSample, endian);
		m_soundTmp = new RawSound(h, datalen);
		m_curr = m_soundTmp->getData();

	} else {
		if (seq != m_prevSeq+1) { return false; }

		unsigned short datalen = BINARY_GET_DATA_S_INCR(p, unsigned short);
		if (n < datalen + (int)sizeof(unsigned short)) { return false; }
		
		assert(m_curr);
		memcpy(m_curr, p, datalen);
		m_curr += datalen;

		if (seq+1 == packetNum) {
			m_sound = m_soundTmp;
			m_soundTmp = 0;
		}

	}
	m_prevSeq = seq;
	return true;
}
	
