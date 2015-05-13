/* $Id: RecvText.cpp,v 1.2 2011-05-12 08:33:51 msi Exp $ */
#include "RecvText.h"
#include "binary.h"

bool RecvTextEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_currTime = BINARY_GET_DOUBLE_INCR(p);

	char *caller = BINARY_GET_STRING_INCR(p);
	if (caller) {
		m_caller = caller;
	}
	BINARY_FREE_STRING(caller);

	// begin(FIX20110208)
	//
	char *target = BINARY_GET_STRING_INCR(p);
	BINARY_FREE_STRING(target);
	// end(FIX20100826)

	char *text = BINARY_GET_STRING_INCR(p);
	if (text ) {
		m_text = text;
	}
	BINARY_FREE_STRING(text);

	m_encode = BINARY_GET_DATA_S_INCR(p, Encode);

	return true;
}

