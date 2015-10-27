/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Attrs_h
#define Attrs_h

#include <sigverse/commonlib/comm/encoder/Encoder.h>

#include <string>
#include <vector>

namespace CommData {

class GetAttributesRequest : public Encoder
{
public:
	enum { BUFSIZE = 2048, }; //TODO: Magic number
private:
	std::vector<std::string> m_attrNames;
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
	std::vector<Attribute*> m_attrs;
public:
	// added by sekikawa (2009/02/20)
	GetAttributesResult() : Encoder(COMM_RESULT_GET_ATTRIBUTES, BUFSIZE) {}

	bool push(Attribute *attr) {
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

