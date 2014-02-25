/* $Id: InvokeOnRecvSound.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include "InvokeOnRecvSound.h"
#include "RawSound.h"
#include "binary.h"

#include <assert.h>

BEGIN_NS_COMMDATA();

char *	InvokeOnRecvSound::getDataHeader(int &sz)
{
	char *p = m_dataHeaderBuf;
	BINARY_SET_DOUBLE_INCR(p, m_time);
	BINARY_SET_STRING_INCR(p, m_caller.c_str());
	RawSoundHeader & h = m_sound.getHeader();
	BINARY_SET_DATA_S_INCR(p, short, h.getChannelNum());
	BINARY_SET_DATA_S_INCR(p, unsigned short, h.getSamplingRate());
	BINARY_SET_DATA_S_INCR(p, unsigned short, h.getBitPerSample());
	BINARY_SET_DATA_S_INCR(p, RawSoundEndian, h.getEndian());
	BINARY_SET_DATA_L_INCR(p, unsigned, getDataLen());

	sz = p - m_dataHeaderBuf;
	assert(sz < DATA_HEADER_BUFSIZE);
	return m_dataHeaderBuf;
}

int 	InvokeOnRecvSound::getDataLen()
{
	return m_sound.getDataLen();
}

char *	InvokeOnRecvSound::getData()
{
	return m_sound.getData();
}

END_NS_COMMDATA();

#ifdef InvokeOnRecvSound_test

#include "EncoderTest.h"
#include "../controller/RecvSound.h"

int main()
{
	typedef CommData::InvokeOnRecvSound Enc;
	typedef RecvSoundEvent Evt;
	typedef EncoderTest<Enc, Evt> Test;


	RawSoundHeader h(2, 16000, 16, RAW_SOUND_LITTLE_ENDIAN);
	RawSound sound(h, 50000);
	Enc enc(1.000, "me", "you", sound);
	Evt evt;
	Test test;

	if (test(enc, evt)) {
		printf("time : %f\n", evt.time());
		printf("callter : %s\n", evt.getCaller());
		RawSound *s = evt.getRawSound();
		RawSoundHeader &h = s->getHeader();
		printf("channel num : %d\n", h.getChannelNum());
		printf("sampling rate : %d\n", h.getSamplingRate());
		printf("bits per sample : %d\n", h.getBitPerSample());
	}
	return 0;
}

#endif
