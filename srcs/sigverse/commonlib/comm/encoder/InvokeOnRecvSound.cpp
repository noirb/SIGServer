/*
 * Created by okamoto on 2011-03-25
 */

#include <sigverse/commonlib/comm/encoder/InvokeOnRecvSound.h>
#include <sigverse/commonlib/RawSound.h>
#include <sigverse/commonlib/binary.h>

#include <assert.h>

BEGIN_NS_COMMDATA();

char * InvokeOnRecvSound::getDataHeader(int &sz)
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

int InvokeOnRecvSound::getDataLen()
{
	return m_sound.getDataLen();
}

char * InvokeOnRecvSound::getData()
{
	return m_sound.getData();
}

END_NS_COMMDATA();

