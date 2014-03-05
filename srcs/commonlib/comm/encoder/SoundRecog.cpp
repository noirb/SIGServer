/*
 * Created by okamoto on 2011-03-25
 */

#include "SoundRecog.h"
#include "RawSound.h"
#include "binary.h"
#include <assert.h>
#include <string.h>

BEGIN_NS_COMMDATA();

char *	RequestSoundRecog::getDataHeader(int &sz)
{
	char *p = m_dataHeaderBuf;
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

int 	RequestSoundRecog::getDataLen()
{
	return m_sound.getDataLen();
}

char *	RequestSoundRecog::getData()
{
	return m_sound.getData();
}


char *	ResultSoundRecog::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	unsigned short enc = m_data.getEncode();
	BINARY_SET_DATA_S_INCR(p, unsigned short, enc);
	BINARY_SET_STRING_INCR(p, m_data.getString());

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}




END_NS_COMMDATA();

