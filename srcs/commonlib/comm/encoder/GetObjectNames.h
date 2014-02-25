/* $Id: GetObjectNames.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Encoder_GetObjectNames_h
#define Comm_Encoder_GetObjectNames_h

#include <string>
#include <vector>

#include "Encoder.h"

namespace CommData {

class GetObjectNamesRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
public:
	enum {
		ALL = 3,    
	};
	typedef unsigned short Type;
private:
	Type	m_type;
public:
	GetObjectNamesRequest(Type t)
		: Encoder(COMM_REQUEST_GET_OBJECT_NAMES, BUFSIZE), m_type(t) {}
	int 	packetNum() { return 1; }
	char *	encode(int seq, int &);
};


class GetEntityNamesRequest : public GetObjectNamesRequest
{
	typedef GetObjectNamesRequest Super;
public:
	GetEntityNamesRequest() : Super(Super::ALL) {}
};

class GetObjectNamesResult : public Encoder
{
private:
	typedef std::string S;
	typedef std::vector<S> C;
private:
	enum { BUFSIZE = 4096, UNIT=50};
private:
	C	m_names;
public:
	GetObjectNamesResult()
		: Encoder(COMM_RESULT_GET_OBJECT_NAMES, BUFSIZE) {}

	void	push(const char *name) {
		if (name != NULL && strlen(name) > 0) {
			m_names.push_back(S(name));
		}
	}
	void	push(const C &o) {
		C::const_iterator i;
		for (i=o.begin(); i!=o.end(); i++) {
			S name = *i;
			m_names.push_back(name);
		}
	}
	
	int 	packetNum();
	char *	encode(int seq, int &);
};

} // namespace


typedef CommData::GetEntityNamesRequest CommRequestGetEntityNamesEncoder;

typedef CommData::GetObjectNamesResult CommResultGetObjectNamesEncoder;

#endif // Comm_Encoder_GetObjectNames_h
 

