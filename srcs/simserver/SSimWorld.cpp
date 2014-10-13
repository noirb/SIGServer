/* $Id: SSimWorld.cpp,v 1.5 2012-06-14 01:48:07 okamoto Exp $ */
#include "SSimWorld.h"
#include "SimGround.h"

#define FREE(P) if (P) { delete P; P = 0; }

SSimWorld::SSimWorld(const char *name)
  : Super(name), m_odeWorld(0), m_run(false), m_autostep(true), m_v21(false)
{
}

void SSimWorld::free_()
{
  Super::free_();

#if (defined USE_OPENGL) || (defined EXEC_SIMULATION)
  FREE(m_ground);
#endif
}

double	SSimWorld::time() const { return m_odeWorld->time(); }
void	SSimWorld::time(double t) { assert(0); }

void SSimWorld::set(const ODEWorld::Gravity &g, double erp)
{
  m_odeWorld = ODEWorld::create(g, erp);
  m_ground = new SimGround(m_odeWorld->space());
  m_odeWorld->setGround(m_ground);
}

void SSimWorld::setup()
{
  if (!m_odeWorld) {
    set(ODEWorld::Gravity(0.0, -980., 0.0), 0.0);
  }
  for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
    SSimObj *obj = (SSimObj*)i->second;
    obj->applyParams(true, m_odeWorld->step());
  }
}

void SSimWorld::start()
{
  m_run = true;
}

void SSimWorld::stop()
{
  m_run = false;
}

void SSimWorld::preStep()
{
  for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
    SSimObj *obj = (SSimObj*)i->second;
    obj->applyParams(false, m_odeWorld->step());
  }
}

void SSimWorld::postStep()
{
  for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
    SSimObj *obj = (SSimObj*)i->second;
    obj->loadParams();
    //	obj->dump();
  }
}

void SSimWorld::nextStep()
{
  if (m_run) {
    preStep();
    m_odeWorld->nextStep();
    postStep();
  }
}

void SSimWorld::nextStep(double stepsize, bool quick)
{
  if (m_run) {
    preStep();
    m_odeWorld->nextStep(stepsize, quick);
    postStep();
  }
}

SSimWorld::NameC & SSimWorld::copyNames(bool needEntities, NameC &v)
{

  for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
    SSimObj *obj = (SSimObj*)i->second;
    if (obj->isAgent()) {
      v.push_back(obj->name());
    } else {
      if (needEntities) {
	v.push_back(obj->name());
      }

    }
  }
  return (SSimWorld::NameC )NULL;
}


void SSimWorld::print(){
  for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
    SSimObj *obj = (SSimObj*)i->second;
    printf("@@@%s\n",obj->name());
  }
}


