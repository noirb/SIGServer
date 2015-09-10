/* $Id: RecvEntities.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/comm/event/RecvEntities.h>
#include <sigverse/commonlib/binary.h>
#include <sigverse/commonlib/SimObj.h>
#include <sigverse/commonlib/SimWorld.h>
#include <sigverse/commonlib/comm/Header.h>

bool RecvEntitiesEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	m_time = BINARY_GET_DOUBLE_INCR(p);

	for (;;) {
		int head = p - data;
		int left = n - head;
		if (left <= 0) {
			fprintf(stderr, "RecvEntitiesEvent : packet too small left = %d\n", left);
			return false;
		}
		
		if (CommData::isPacketEnd(p)) {
			break;
		}
		
		SimObj *obj = new SimObj();
		int r = obj->setBinary(p, left);
		if (r < 0) { return false; }
		push(obj);
		p += r;
	}
	return true;
}

ResultGetAllEntitiesEvent::ResultGetAllEntitiesEvent()
	: Super(), m_world(0)
{
	m_world = new SimWorld("viewtest");
}

#define FREE(P) if (P) { delete P; P = 0; }

ResultGetAllEntitiesEvent::~ResultGetAllEntitiesEvent()
{
	FREE(m_world);
}

void ResultGetAllEntitiesEvent::push(SimObj *obj)
{
	m_world->push(obj);
}

SimWorld *ResultGetAllEntitiesEvent::releaseWorld()
{
	SimWorld *tmp = m_world;
	m_world = 0;
	return tmp;
}


