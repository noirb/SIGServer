/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Comm_Encoder_GetObjectNames_h
#define Comm_Encoder_GetObjectNames_h

#include <string>
#include <vector>

#include <sigverse/commonlib/comm/encoder/Encoder.h>

namespace CommData {

class GetObjectNamesRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, }; //TODO: Magic number
public:
	enum {
		ALL = 3,
	};
	typedef unsigned short Type;
private:
	Type  m_type;
public:
	GetObjectNamesRequest(Type t)
		: Encoder(COMM_REQUEST_GET_OBJECT_NAMES, BUFSIZE), m_type(t) {}

	int     packetNum() { return 1; }
	char *  encode(int seq, int &);
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
	enum { BUFSIZE = 4096, UNIT=50}; //TODO: Magic number
private:
	std::vector<std::string> m_names;
public:
	GetObjectNamesResult()
		: Encoder(COMM_RESULT_GET_OBJECT_NAMES, BUFSIZE) {}

	void	push(const char *name) {
		if (name != NULL && strlen(name) > 0) {
			m_names.push_back(std::string(name));
		}
	}
	void push(const std::vector<std::string> &o) {
		std::vector<std::string>::const_iterator i;

		for (i=o.begin(); i!=o.end(); i++) {
			std::string name = *i;
			m_names.push_back(name);
		}
	}
	
	int    packetNum();
	char * encode(int seq, int &);
};

} // namespace


typedef CommData::GetEntityNamesRequest CommRequestGetEntityNamesEncoder;

typedef CommData::GetObjectNamesResult CommResultGetObjectNamesEncoder;

#endif // Comm_Encoder_GetObjectNames_h
 

