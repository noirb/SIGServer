/* $Id: ResultAttach.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/event/ResultAttach.h>
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/X3DDB.h>

ResultAttachViewEvent::~ResultAttachViewEvent()
{
	delete m_db; m_db = NULL;
}


bool ResultAttachViewEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;
	m_result = BINARY_GET_DATA_S_INCR(p, CommDataType);
	char *msg = BINARY_GET_STRING_INCR(p);
	if (msg) {
		m_errmsg = msg;
	}
	BINARY_FREE_STRING(msg);

	if (m_result == COMM_RESULT_OK) {
		short num = BINARY_GET_DATA_S_INCR(p, short);

		if (num > 0) {
			m_db = new X3DDB();

			for (int i=0; i<num; i++) {
				char *k = BINARY_GET_STRING_INCR(p);
				if (k) {
					char *v = BINARY_GET_STRING_INCR(p);
					m_db->set(k, v);
					BINARY_FREE_STRING(v);
				}
				BINARY_FREE_STRING(k);
			}
		}
	}
	return true;
}
	

