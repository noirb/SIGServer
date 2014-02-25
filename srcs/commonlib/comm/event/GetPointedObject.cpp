#include "GetPointedObject.h"
#include "binary.h"



bool RequestGetPointedObjectEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	char *speakerName = BINARY_GET_STRING_INCR(p);
	if (speakerName) {
		m_speakerName = speakerName;
	}
	BINARY_FREE_STRING(speakerName);

	char *name = BINARY_GET_STRING_INCR(p);
	if (name) {
		m_SStr1 = std::string(name);
	}
	BINARY_FREE_STRING(name);
	name = BINARY_GET_STRING_INCR(p);
	if (name) {
		m_SStr2 = std::string(name);
	}
	BINARY_FREE_STRING(name);

	char* typicalTypeStr = BINARY_GET_STRING_INCR(p);
	if(typicalTypeStr){
		m_TypicalType = atoi(typicalTypeStr);
	}
	BINARY_FREE_STRING(typicalTypeStr);

	return true;
}


bool ResultGetPointedObjectEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	const short N = BINARY_GET_DATA_S_INCR(p, short);

	for (int i=0; i<(int)N; i++) {
		int left = n - ( p - data);
		if (left <= 0) { return false; }
		char *s = BINARY_GET_STRING_INCR(p);
		if (s) {
			m_candidateNameVec.push_back(s);
		}
		BINARY_FREE_STRING(s);
	}



	return true;
}


