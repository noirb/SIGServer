#ifndef Comm_Encode_GetPointedObject_h
#define Comm_Encode_GetPointedObject_h

#include "Encoder.h"

#include <vector>


namespace CommData {

class GetPointedObjectRequestEncoder : public Encoder
{
private:
	enum { BUFSIZE = 256, };
private:
	std::string m_speakerName;
	std::string m_SStr1;
	std::string m_SStr2;
	int m_TypicalType;
public:
	GetPointedObjectRequestEncoder(const char *speakerName, std::string SStr1, std::string SStr2, int typicalType)
		: Encoder(COMM_REQUEST_GET_POINTED_OBJECT, BUFSIZE), m_speakerName(speakerName), m_SStr1(SStr1), m_SStr2(SStr2), m_TypicalType(typicalType){}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


class GetPointedObjectResultEncoder : public Encoder
{
	enum { BUFSIZE = 1024, };
private:
	std::vector<std::string>  m_candidateNameVec;
public:
	GetPointedObjectResultEncoder() : Encoder(COMM_RESULT_GET_POINTED_OBJECT, BUFSIZE) {}

	void	push(const char *name) {
		if (name != NULL && strlen(name) > 0) {
			m_candidateNameVec.push_back(std::string(name));
		}
	}
	void	push(const std::vector<std::string> &o) {
		std::vector<std::string>::const_iterator i;
		for (i=o.begin(); i!=o.end(); i++) {
			std::string  name = *i;
			m_candidateNameVec.push_back(name);
		}
	}

	int 	packetNum() { return 1; }
	char *	encode(int seq, int &);
};


} // namespace

typedef CommData::GetPointedObjectRequestEncoder CommRequestGetPointedObjectEncoder;
typedef CommData::GetPointedObjectResultEncoder CommResultGetPointedObjectEncoder;

#endif // Comm_Encode_GetPointedObject_h
 

