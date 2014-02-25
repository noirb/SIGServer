/* $Id: DetectEntities.cpp,v 1.3 2012-03-27 04:13:51 noma Exp $ */
#include "DetectEntities.h"
#include "Header.h"
#include "binary.h"
#include <assert.h>

BEGIN_NS_COMMDATA();

char * DetectEntitiesRequest::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_STRING_INCR(p, m_name.c_str());
	BINARY_SET_DATA_S_INCR(p, short, m_id);
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

char * DetectEntitiesResult::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	char *pCnt = p;
	BINARY_SET_DATA_S_INCR(p, short, 0);

	short cnt = 0;
	// modified by noma@tome (2012/02/20)
	for (C::iterator i=m_detected.begin(); i!=m_detected.end(); i++) {
		S name = *i;
		
		int left = BUFSIZE - (p-m_buf + name.length()+2);
		if (left <= COMM_DATA_FOOTER_SIZE) {
			break;
		}
		BINARY_SET_STRING_INCR(p, name.c_str());
		cnt++;
	}
/*Original*/
//	for (C::iterator i=m_detected.begin(); i!=m_detected.end(); i++) {
//		S name = *i;
//		BINARY_SET_STRING_INCR(p, name.c_str());
//
//		int left = BUFSIZE - (p-m_buf);
//		if (left <= COMM_DATA_FOOTER_SIZE) {
//			break;
//		}
//		cnt++;
//	}

	BINARY_SET_DATA_S(pCnt, short, cnt);
	
	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;

}

END_NS_COMMDATA();

#ifdef DetectEntities_test1

#include "../event/RequestDetectEntities.h"
#include "EncoderTest.h"

int main()
{
	typedef CommData::DetectEntitiesRequest ENC;
	typedef RequestDetectEntitiesEvent EVT;

	typedef EncoderTest<ENC, EVT> Test;
	CommData::DetectEntitiesRequest req("Robot1");
	EVT evt;
	Test test;
	test(req, evt);

	printf("Agent Name : %s\n", evt.getAgentName());
	return 0;
}

#endif

#ifdef DetectEntities_test2

#include "../event/ResultDetectEntities.h"
#include "EncoderTest.h"

int main()
{
	typedef CommData::DetectEntitiesResult ENC;
	typedef ResultDetectEntitiesEvent EVT;

	typedef EncoderTest<ENC, EVT> Test;
	ENC req("Robot1");

	req.pushDetectedEntity("Robo");
	req.pushDetectedEntity("Roboo");
	req.pushDetectedEntity("Robooo");
	req.pushDetectedEntity("Roboooo");
	req.pushDetectedEntity("Robooooo");
	req.pushDetectedEntity("Roboooooo");
	req.pushDetectedEntity("Robooooooo");
	req.pushDetectedEntity("Roboooooooo");
	req.pushDetectedEntity("Robooooooooo");
	req.pushDetectedEntity("Roboooooooooo");

	EVT evt;
	Test test;
	test(req, evt);

	int n = evt.getDetectedCount();
	for (int i=0; i<n ;i++) {
		printf("%d : %s\n", i, evt.getDetected(i));
	}
	return 0;
}

#endif

