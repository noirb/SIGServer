/* $Id: ResultCaptureViewImage.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <sigverse/commonlib/comm/event/ResultCaptureViewImage.h>
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/ViewImage.h>

#include <string.h>
#include <stdio.h>

#define FREE(P) if (P) { delete P; P = 0; }

void ResultCaptureViewImageEvent::free_()
{
	FREE(m_imgtmp);
	FREE(m_img);
}

bool ResultCaptureViewImageEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	if (seq == 0) {
		if (n < (int)(sizeof(unsigned short)*4 + sizeof(unsigned))) {
			return false;
		}
		ImageDataType dataType = BINARY_GET_DATA_S_INCR(p, ImageDataType);
		ColorBitType  cbType   = BINARY_GET_DATA_S_INCR(p, ColorBitType);
		unsigned short w = BINARY_GET_DATA_S_INCR(p, unsigned short);
		unsigned short h = BINARY_GET_DATA_S_INCR(p, unsigned short);
		int datalen = BINARY_GET_DATA_L_INCR(p, unsigned);
		
		ViewImageInfo info(dataType, cbType, w, h);
		m_imgtmp = new ViewImage(info);

		if (datalen != m_imgtmp->getBufferLength()) {
			return false;
		}
		m_curr = m_imgtmp->getBuffer();
		
	} else {
		if (seq != m_prevSeq+1) { return false; }

		unsigned short datalen = BINARY_GET_DATA_S_INCR(p, unsigned short);
		if (n < datalen + (int)sizeof(unsigned short)) { return false; }
		assert(m_curr);
		memcpy(m_curr, p, datalen);
		m_curr += datalen;

		if (seq+1 == packetNum) {
			m_img = m_imgtmp;
			m_imgtmp = 0;
		}
	}

	// printf("%s(%d) : %d/%d OK\n", __FILE__, __LINE__, seq, packetNum);
	// fflush(stdout);
	m_prevSeq = seq;
	return true;
}

