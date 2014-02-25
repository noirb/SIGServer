/* $Id: SoundRecog.cpp,v 1.2 2011-03-31 08:16:37 okamoto Exp $ */
#include "SoundRecog.h"
#include "RawSound.h"
#include "binary.h"
#include "Header.h"
#include <assert.h>
#include <string.h>

#define FREE(P) if (P) { delete P; P = NULL; }

RequestSoundRecogEvent::~RequestSoundRecogEvent()
{
	FREE(m_sound);
	FREE(m_soundTmp);
}

bool RequestSoundRecogEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	if (seq == 0) {
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


bool ResultSoundRecogEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	unsigned short enc = BINARY_GET_DATA_S_INCR(p, unsigned short);
	char *text = BINARY_GET_STRING_INCR(p);
	m_text = new Text(enc, text);
	
	BINARY_FREE_STRING(text);

	return true;
}

