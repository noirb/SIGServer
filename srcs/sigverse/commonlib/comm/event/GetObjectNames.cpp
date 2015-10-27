/* $Id: GetObjectNames.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/event/GetObjectNames.h>
#include <sigverse/commonlib/binary.h>

bool RequestGetObjectNamesEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_type = BINARY_GET_DATA_S_INCR(p, CommData::GetObjectNamesRequest::Type);
	return true;
}

bool ResultGetObjectNamesEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	unsigned short N = BINARY_GET_DATA_S_INCR(p, unsigned short);

	for (unsigned short i=0; i<N; i++) {
		char *name = BINARY_GET_STRING_INCR(p);
		m_names.push_back(std::string(name));
	}
	return true;
}
