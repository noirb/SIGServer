/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Entity_h
#define Entity_h

#include "Encoder.h"

class SimObjBase;

namespace CommData {

class GetEntityRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, }; //TODO: Magic number
private:
	std::string m_name;
public:
	GetEntityRequest(const char *name)
		: Encoder(COMM_REQUEST_GET_ENTITY, BUFSIZE), m_name(name) {;}
	int packetNum()	{ return 1; }
	char *encode(int seq, int &sz);
};

class GetEntityResult : public Encoder
{
private:
	enum { BUFSIZE = 1024, RESIZE = BUFSIZE, }; //TODO: Magic number
private:
	double	m_time;
	SimObjBase * m_obj;
public:
	GetEntityResult(double t, SimObjBase *obj)
		: Encoder(COMM_RESULT_GET_ENTITY, BUFSIZE), m_time(t), m_obj(obj) {;}
	int packetNum()	{ return 1; }
	char *encode(int seq, int &sz);
};

} // namespace

typedef CommData::GetEntityRequest CommRequestGetEntityEncoder;
typedef CommData::GetEntityResult CommResultGetEntityEncoder;

#endif // Entity_h
 

