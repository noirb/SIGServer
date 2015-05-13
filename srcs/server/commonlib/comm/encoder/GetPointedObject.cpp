#include "GetPointedObject.h"
#include "binary.h"
#include <stdlib.h>

//DEBUG
#include <string>
#include <stdio.h>
#include <iostream>
//END DEBUG

BEGIN_NS_COMMDATA();

char * GetPointedObjectRequestEncoder::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	BINARY_SET_STRING_INCR(p, m_speakerName.c_str());
	BINARY_SET_STRING_INCR(p, m_SStr1.c_str());
	BINARY_SET_STRING_INCR(p, m_SStr2.c_str());

	char tmpStr[2];
	if(0 <= m_TypicalType || m_TypicalType <= 2){
		sprintf(tmpStr, "%d", m_TypicalType);
		BINARY_SET_STRING_INCR(p, tmpStr);
	}else{
		BINARY_SET_STRING_INCR(p, "-1");
	}

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}


char * GetPointedObjectResultEncoder::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	char *pCnt = p;
	BINARY_SET_DATA_S_INCR(p, short, 0);
	short cnt = 0;

	for (std::vector<std::string>::iterator i=m_candidateNameVec.begin(); i!=m_candidateNameVec.end(); i++) {
		std::string name = *i;

		int left = BUFSIZE - (p-m_buf + name.length()+2);
		if (left <= COMM_DATA_FOOTER_SIZE) {
			break;
		}
		BINARY_SET_STRING_INCR(p, name.c_str());
		cnt++;
	}

	BINARY_SET_DATA_S(pCnt, short, cnt);

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();

