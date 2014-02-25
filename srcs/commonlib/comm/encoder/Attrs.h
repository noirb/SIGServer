/* $Id: Attrs.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#ifndef Attrs_h
#define Attrs_h

#include "Encoder.h"

#include <string>
#include <vector>

namespace CommData {

class GetAttributesRequest : public Encoder
{
public:
	enum { BUFSIZE = 2048, };
private:
	typedef std::string S;
	typedef std::vector<S> C;
private:
	C 	m_attrNames;
public:
	GetAttributesRequest() : Encoder(COMM_REQUEST_GET_ATTRIBUTES, BUFSIZE) {}

	bool push(const char *attrName) {
		if (m_attrNames.size() < BUFSIZE/10) {
			m_attrNames.push_back(attrName);
			return true;
		} else {
			return false;
		}
	}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class Attribute;

class GetAttributesResult : public Encoder
{
private:
	enum { BUFSIZE = 5*GetAttributesRequest::BUFSIZE };
private:
	typedef std::string S;
	typedef std::vector<Attribute*> C;
private:
	C 	m_attrs;
public:
	// added by sekikawa (2009/02/20)
	GetAttributesResult() : Encoder(COMM_RESULT_GET_ATTRIBUTES, BUFSIZE) {}

	bool 	push(Attribute *attr) {
		m_attrs.push_back(attr);
		return true;
	}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace 

typedef CommData::GetAttributesRequest CommRequestGetAttributesEncoder;
typedef CommData::GetAttributesResult CommResultGetAttributesEncoder;

#endif //

