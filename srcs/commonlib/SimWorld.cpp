/* $Id: SimWorld.cpp,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */ 
#include "SimWorld.h"
#include "X3DDB.h"
#include "Logger.h"

#include <algorithm>
#include <functional>


SimWorld::SimWorld(const char *name_)
	: m_name(name_),
	  m_ground(0),
	  m_time(-1.0)
{
	;
}


#define FREE(P) if (P) { delete P; P = 0;}

void SimWorld::push(SimObjBase *obj)
{
  m_objs[obj->name()] = obj;
}

// add by sekikawa (2007/10/3)
SimObjBase * SimWorld::getObjById(SimObjBase::Id id)
{
	M::iterator i;
	for (i=m_objs.begin(); i!=m_objs.end(); i++) {
		SimObjBase *obj = i->second;
		if (obj->id() == id) return obj;
	}
	return NULL;
}

void SimWorld::free_()
{
	M::iterator i;
	for (i=m_objs.begin(); i!=m_objs.end(); i++) {
		SimObjBase *obj = i->second;
		delete obj;
	}
	m_objs.clear();
}

void SimWorld::dump()
{
	for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
		i->second->dump();
	}
}

int SimWorld::getAllEntitiesDataSize()
{
  int size = 0;

  M::iterator i;
  for (i=m_objs.begin(); i!=m_objs.end(); i++) {

    // name
    size += i->first.size();
    
    //position
    size += sizeof(double) * 3;
  }
  return size;
}

#ifdef IMPLEMENT_DRAWER
#include "SimGround.h"

void SimWorld::draw(GLContext &c)
{
	assert(m_ground);
	m_ground->draw(c);

	for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
		SimObjBase *obj = i->second;
		obj->draw(c);
	}

}
#endif // IMPLEMENT_DRAWER




