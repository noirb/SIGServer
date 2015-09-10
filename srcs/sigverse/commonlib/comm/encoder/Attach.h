/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Attach_h
#define Attach_h

#include <sigverse/commonlib/comm/encoder/Encoder.h>
#include <sigverse/commonlib/comm/encoder/Result.h>

class Service;
class X3DDB;

namespace CommData {

class AttachRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, }; //TODO: Magic number
private:
	std::string m_name;
public:
	AttachRequest(CommDataType type,  const char *name)
		: Encoder(type, BUFSIZE), m_name(name) {;}
	virtual ~AttachRequest() {;}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
protected:
	virtual int moreData(char *buf) = 0;
};

class AttachControllerRequest : public AttachRequest
{
public:
	AttachControllerRequest(const char *name)
		: AttachRequest(COMM_REQUEST_ATTACH_CONTROLLER, name) {;}
private:
	int moreData(char *buf)
	{
		return 0;
	}
};

class AttachControllerResult : public Result
{
public:
	AttachControllerResult(CommResultType r, const char *msg) :
		Result(COMM_RESULT_ATTACH_CONTROLLER, r, msg) {};

};

class AttachViewRequest : public AttachRequest
{
private:
	bool m_polling;
public:
	AttachViewRequest(const char *name)
		: AttachRequest(COMM_REQUEST_ATTACH_VIEW, name), m_polling(false) {;}

	AttachViewRequest(const char *name, bool polling)
		: AttachRequest(COMM_REQUEST_ATTACH_VIEW, name),
		  m_polling(polling) {;}

private:
	int moreData(char *buf);
};

class AttachViewResult : public Encoder
{
	enum { BUFSIZE = 8192 };
private:
	CommResultType m_result;
	std::string	m_msg;
	X3DDB &m_db;
public:
	AttachViewResult(CommResultType r, const char *msg, X3DDB &db) :
		Encoder(COMM_RESULT_ATTACH_VIEW, BUFSIZE),
		m_result(r),
		m_db(db) {
		if (msg) {
			m_msg = msg;
		}
	}

private:
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class ConnectDataPortRequest : public AttachRequest
{
public:
	ConnectDataPortRequest(const char *name)
		: AttachRequest(COMM_REQUEST_CONNECT_DATA_PORT, name) {;}
private:
	int moreData(char *buf)
	{
		return 0;
	}
};

class ProvideServiceRequest : public Encoder
{
	enum { BUFSIZE = 512, }; //TODO: Magic number
private:
	Service	&m_service;
public:
	ProvideServiceRequest(Service &service)
		: Encoder(COMM_REQUEST_PROVIDE_SERVICE, BUFSIZE), m_service(service) {;}
private:
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace 

typedef CommData::AttachControllerRequest CommRequestAttachModelEncoder;
typedef CommRequestAttachModelEncoder CommRequestAttachControllerEncoder;
typedef CommData::AttachControllerResult CommResultAttachControllerEncoder;

typedef CommData::AttachViewRequest CommRequestAttachViewEncoder;
typedef CommData::AttachViewResult CommResultAttachViewEncoder;

typedef CommData::ConnectDataPortRequest CommRequestConnectDataPortEncoder;
typedef CommData::ProvideServiceRequest CommRequestProvideServiceEncoder;

#endif // Attach_h
 

