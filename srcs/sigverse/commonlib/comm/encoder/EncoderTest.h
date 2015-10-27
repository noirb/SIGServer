/*
 * Created by okamoto on 2011-03-25
 */

#ifndef EncoderTest_h
#define EncoderTest_h

#include <sigverse/commonlib/comm/Header.h>

template <class ENC, class EVT>
class EncoderTest
{
public:
	bool operator()(ENC &enc, EVT &evt)
	{
		int n = enc.packetNum();
		for (int i=0; i<n; i++) {
			int bytes;
			char *p = enc.encode(i, bytes);
			CommDataHeader h;
			int r = CommData::getHeader(p, bytes, h);
			printf("%d/%d", h.seq, h.packetNum);

			if (evt.set(h.packetNum, h.seq, p+r, bytes-r)) {
				printf(" -> ok\n");
			}  else {
				return false;
			}
		}
		return true;
	}
};


#endif // EncoderTest_h
 
