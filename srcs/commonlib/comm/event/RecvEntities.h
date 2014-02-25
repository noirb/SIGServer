/* $Id: RecvEntities.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef CommEvent_RecvEntities_h
#define CommEvent_RecvEntities_h

#include <vector>

class SimWorld;
class SimObj;

class RecvEntitiesEvent
{
private:
	double	m_time;
protected:
	RecvEntitiesEvent() : m_time(-1) {;}
public:
	virtual ~RecvEntitiesEvent() {;}
public:
	bool	set(int packetNum, int seq, char *data, int n);
	double	time() { return m_time; }
private:
	virtual void push(SimObj *obj) = 0;
};

class RequestGetAllEntitiesEvent
{
public:
	bool	set(int, int, char *, int) { return true; }
};

class ResultGetAllEntitiesEvent : public RecvEntitiesEvent
{
private:
	typedef RecvEntitiesEvent Super;
private:
	SimWorld *m_world;
private:
	void push(SimObj *p);
public:
	ResultGetAllEntitiesEvent();
	~ResultGetAllEntitiesEvent();
	SimWorld *releaseWorld();
};

class RequestUpdateEntitiesEvent : public RecvEntitiesEvent
{
public:
	typedef std::vector<SimObj *> C;
private:
	C	m_objs;
private:
	void push(SimObj *p) { m_objs.push_back(p); }
public:
	const C & objs() { return m_objs; }
};


#endif // CommEvent_RecvEntities_h
 

