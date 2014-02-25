/* $Id: Entity.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Entity_h
#define Entity_h

#include "Encoder.h"

class SimObjBase;

namespace CommData {

class GetEntityRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
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
	enum { BUFSIZE = 1024, RESIZE = BUFSIZE, };
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
 

