/*
 * Created by okamoto on 2011-03-25
 */

#ifndef NameService_h
#define NameService_h

#include "systemdef.h"
#include "Encoder.h"
#include "Service.h"

namespace CommData {

class NSQueryRequest : public Encoder
{
	enum { BUFSIZE = 128, }; //TODO: Magic number
private:
	Service::Kind m_kind;
public:
	NSQueryRequest(Service::Kind k)
		: Encoder(COMM_NS_QUERY_REQUEST, BUFSIZE), m_kind(k) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class NSQueryResult : public Encoder
{
	enum { BUFSIZE = 128, }; //TODO: Magic number
private:
	Service  * m_service;
public:
	NSQueryResult(Service *s)
		: Encoder(COMM_NS_QUERY_RESULT, BUFSIZE), m_service(s) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class NSPingerRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, }; //TODO: Magic number
private:
	Service::Kind	m_kind;
public:
	NSPingerRequest(Service::Kind kind)
		: Encoder(COMM_NS_PINGER_REQUEST, BUFSIZE), m_kind(kind)
	{
		
	}
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class NSPingerResult : public Encoder
{
	enum { BUFSIZE = 128, }; //TODO: Magic number
private:
	bool	m_active;
public:
	NSPingerResult(bool active)
		: Encoder(COMM_NS_PINGER_RESULT, BUFSIZE), m_active(active) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace

typedef CommData::NSQueryRequest  CommRequestNSQueryEncoder;
typedef CommData::NSQueryResult   CommResultNSQueryEncoder;
typedef CommData::NSPingerRequest CommRequestNSPingerEncoder;
typedef CommData::NSPingerResult  CommResultNSPingerEncoder;

#endif // NameService_h
 

