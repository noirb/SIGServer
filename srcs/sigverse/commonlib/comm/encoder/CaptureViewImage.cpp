#include <sigverse/commonlib/comm/encoder/CaptureViewImage.h>
#include <sigverse/commonlib/binary.h>
#include "assert.h"

#ifdef WIN32
#ifdef SIGVERSE_OGRE_CLIENT
#include "SgvLog.h"
#endif // SIGVERSE_OGRE_CLIENT

#ifdef IRWAS_OGRE_CLIENT
#include "IrcApp.h"
#endif // IRWAS_OGRE_CLIENT
#endif // WIN32

BEGIN_NS_COMMDATA();

char *CaptureViewImageRequest::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_STRING_INCR(p, m_agentName.c_str());
	BINARY_SET_DATA_S_INCR(p, ImageDataType,  m_info.getDataType());
	BINARY_SET_DATA_S_INCR(p, ColorBitType,   m_info.getColorBitType());
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_info.getWidth());
	BINARY_SET_DATA_S_INCR(p, unsigned short, m_info.getHeight());
	BINARY_SET_DATA_S_INCR(p, short,          m_id);
	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	assert(sz <= BUFSIZE);
	setPacketSize(m_buf, sz);
	return m_buf;
}


CaptureViewImageResult::CaptureViewImageResult(ViewImage &img)
	: RawDataEncoder(COMM_RESULT_CAPTURE_VIEW_IMAGE), m_img(img)
{
	
}


char *	CaptureViewImageResult::getDataHeader(int &sz)
{
	char *p = m_dataHeaderBuf;
	const ViewImageInfo & info = m_img.getInfo();

	BINARY_SET_DATA_S_INCR(p, ImageDataType,  info.getDataType());
	BINARY_SET_DATA_S_INCR(p, ColorBitType,   info.getColorBitType());
	BINARY_SET_DATA_S_INCR(p, unsigned short, info.getWidth());
	BINARY_SET_DATA_S_INCR(p, unsigned short, info.getHeight());
	BINARY_SET_DATA_L_INCR(p, unsigned,       m_img.getBufferLength());
	sz = p - m_dataHeaderBuf;
	assert (sz == DATA_HEADER_SIZE);
	return m_dataHeaderBuf;
}
	
int CaptureViewImageResult::getDataLen()
{
	return m_img.getBufferLength();
}

char * CaptureViewImageResult::getData()
{
	return m_img.getBuffer();
}

END_NS_COMMDATA();

