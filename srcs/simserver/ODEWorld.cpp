/* 
 * Written by okamoto on 2012-03-19
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add English comments (Translation from v2.2.0 is finished)
 */

#include "ODEWorld.h"
#include "ODEObj.h"
#ifdef USE_ODE
#include <assert.h>
#include "SParts.h"
#include "SimObjBase.h"

#if 1
#define DUMP(MSG)
#else
#define DUMP(MSG) printf MSG;
#endif



ODEWorld *ODEWorld::s_inst = 0;

ODEWorld *ODEWorld::create(const Gravity &g, dReal erp)
{
	delete s_inst;
	return s_inst = new ODEWorld(g, erp);
}

ODEWorld::ODEWorld(const Gravity &g, double erp)
	: m_timeStep(0.01),
	  m_itrCnt(0),
	  m_nowtime(0.0),
	  m_quickStep(false),
	  m_mu(0.9),
	  m_mu2(0.02),
	  m_slip1(0.01),
	  m_slip2(0.01),
	  m_soft_erp(0.8),
	  m_soft_cfm(0.0001),
	  m_bounce(0.3),
	  m_bounce_vel(5.0)
{
	m_world = dWorldCreate();
	dWorldSetGravity(m_world, g.x, g.y, g.z);
	// default
	dWorldSetERP(m_world, 0.5);
	dWorldSetCFM(m_world, 1e-8);
	m_space = dHashSpaceCreate(0);
	m_jgroup = dJointGroupCreate(1000000);
}

void ODEWorld::free_()
{
	dJointGroupDestroy(m_jgroup);
	dSpaceDestroy(m_space);
	dWorldDestroy(m_world);
}

ODEWorld *ODEWorld::get()
{
	assert(s_inst);
	return s_inst;
}

static void nearCB(void *data, dGeomID o1, dGeomID o2);

typedef std::map<std::string, bool> M;
static M s_chash;
static ODEWorld::CollisionC s_collisions;

#define SEP_STR "-+-"

static std::string chash_key(const char * name1, const char * name2) {
	typedef std::string S;
	if (!name1 || strlen(name1) <= 0){ return ""; }
	if (!name2 || strlen(name2) <= 0){ return ""; }
	if (strcmp(name1, name2) < 0) {
		return S(name1) + S(SEP_STR) + S(name2);
	} else {
		return S(name2) + S(SEP_STR) + S(name1);
	}
}

static bool chash_find(std::string key) {
	M::iterator i = s_chash.find(key);
	return i != s_chash.end()? true: false;
}


int ODEWorld::collideWith(const char *name, WithC &c)
{
	typedef CollisionC C;
	for (C::iterator i=s_collisions.begin(); i!=s_collisions.end(); i++) {
		// Store the 'other body' + 'other's parts' + 'my parts'
		if (strcmp(i->body1.c_str(), name) == 0) {
			std::string bp = i->body2 + ":" + i->parts2 + ":" + i->parts1 + ":";
			c.push_back(bp);
		} else if (strcmp(i->body2.c_str(), name) == 0) {
			std::string bp = i->body1 + ":" + i->parts1 + ":" + i->parts2 + ":";
			c.push_back(bp);
		}
	}
	return c.size();
}

void ODEWorld::nextStep()
{
	s_chash.clear();
	s_collisions.clear();

	dWorldID w = world();
	dSpaceID s = space();

	// konao
	DUMP(("** 1 ** [%s:%d]\n", __FILE__, __LINE__));

	dSpaceCollide(s, 0, &nearCB);
	DUMP(("** 2 ** [%s:%d]\n", __FILE__, __LINE__));

	// DO NOT use dWorldQuickStep, which causes some problems. 080113
	if(m_quickStep){
		dWorldQuickStep(w, m_timeStep);
	}
	else{
		dWorldStep(w, m_timeStep);
	}

	DUMP(("** 3 ** [%s:%d]\n", __FILE__, __LINE__));

	dJointGroupID jgroup = jointGroup();

	DUMP(("** 4 ** [%s:%d]\n", __FILE__, __LINE__));

	dJointGroupEmpty(jgroup);

	DUMP(("** 5 ** [%s:%d]\n", __FILE__, __LINE__));
 
	m_itrCnt++;
	m_nowtime += m_timeStep;
}

void ODEWorld::nextStep(double stepsize, bool quick)
{
	s_chash.clear();
	s_collisions.clear();

	dWorldID w = world();
	dSpaceID s = space();

	dSpaceCollide(s, 0, &nearCB);

	// Quick Mode
	if(quick) {
		dWorldQuickStep(w, stepsize);
	}
	else {
		dWorldStep(w, stepsize);
	}

	dJointGroupID jgroup = jointGroup();

	dJointGroupEmpty(jgroup);
	m_itrCnt++;
	m_nowtime += stepsize;
}

void ODEWorld::setERP(double value)
{
	dWorldSetERP(m_world, value);
}

void ODEWorld::setCFM(double value)
{
	dWorldSetCFM(m_world, value);
}

void ODEWorld::setCollisionParam(std::string name, double value)
{
	//LOG_MSG(("%s = %f", name.c_str(), value));
	if(name == "mu")               m_mu = value;
	else if (name == "mu2")        m_mu2 = value;
	else if (name == "slip1")      m_slip1 = value;
	else if (name == "slip2")      m_slip2 = value;
	else if (name == "soft_erp")   m_soft_erp = value;
	else if (name == "soft_cfm")   m_soft_cfm = value;
	else if (name == "bounce")     m_bounce = value;
	else if (name == "bounce_vel") m_bounce_vel = value;
}

double ODEWorld::getCollisionParam(std::string name)
{
	if(name == "mu")               return m_mu;
	else if (name == "mu2")        return m_mu2; 
	else if (name == "slip1")      return m_slip1;
	else if (name == "slip2")      return m_slip2;
	else if (name == "soft_erp")   return m_soft_erp;
	else if (name == "soft_cfm")   return m_soft_cfm;
	else if (name == "bounce")     return m_bounce;
	else if (name == "bounce_vel") return m_bounce_vel;
}


// TODO: when is this function called?
static void collideCB(void *data, dGeomID o1, dGeomID o2)
{

	ODEObj *odeobj1 = ODEObjectContainer::getInstance()->getODEObjFromGeomID(o1);
	ODEObj *odeobj2 = ODEObjectContainer::getInstance()->getODEObjFromGeomID(o2);

	//	const int N = 32;
	const int N = 10; // TODO: Magic number should be removed
	dContact contacts[N];

	int n = dCollide(o1, o2, N, &(contacts[0].geom),  sizeof(contacts[0]));
	if (n > 0) {

		ODEWorld *world = ODEWorld::get();
		for (int i=0; i<n; i++) {

			dContact *c = &contacts[i];


			c->surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactBounce;
			//
			// Reflection of material parameters of the collided object
			// Fliction force should be regarded as average of contiguous material (???)
			// TODO: Calclation of fliction force sould be considered
			//
			if(odeobj1 && odeobj2) {
				c->surface.mu       = ( odeobj1->getMu1()     + odeobj2->getMu1() )     / 2.0;
				c->surface.mu2      = ( odeobj1->getMu2()     + odeobj2->getMu2() )     / 2.0;
				c->surface.slip1    = ( odeobj1->getSlip1()   + odeobj2->getSlip1() )   / 2.0;
				c->surface.slip2    = ( odeobj1->getSlip2()   + odeobj2->getSlip2() )   / 2.0;
				c->surface.soft_erp = ( odeobj1->getSoftErp() + odeobj2->getSoftErp() ) / 2.0;
				c->surface.soft_cfm = ( odeobj1->getSoftCfm() + odeobj2->getSoftCfm() ) / 2.0;
				c->surface.bounce   = ( odeobj1->getBounce()  + odeobj2->getBounce() )  / 2.0;
			}
			else {
				c->surface.mu       = 0.9;
				c->surface.mu2      = 0.02;
				c->surface.slip1    = 0.0;
				c->surface.slip2    = 0.0;
				c->surface.soft_erp = 0.8;
				c->surface.soft_cfm = 0.01;
				c->surface.bounce   = 0.0;
			}


			dJointID cj = dJointCreateContact(world->world(), world->jointGroup(), c);
			dJointAttach(cj, dGeomGetBody(o1), dGeomGetBody(o2));
		}
	}

}


static void nearCB(void *data, dGeomID o1, dGeomID o2)
{
#if 0
	static int cnt = 0;
	cnt++;
#endif

	ODEObj *odeobj1 = ODEObjectContainer::getInstance()->getODEObjFromGeomID(o1);
	ODEObj *odeobj2 = ODEObjectContainer::getInstance()->getODEObjFromGeomID(o2);

	/*
	SSimRobotEntity *ent1 = ODEObjectContainer::getInstance()->getSSimRobotEntityFromGeomID(o1);
	SSimRobotEntity *ent2 = ODEObjectContainer::getInstance()->getSSimRobotEntityFromGeomID(o2);
	
	if(ent1 != NULL && ent2 != NULL && ent1->name() == ent2->name()){
	  //LOG_MSG(("name (%s, %s)",ent1->name().c_str(), ent2->name().c_str()));
	  return;
	}
	*/

	SParts *p1 = NULL;
	SParts *p2 = NULL;
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	if(b1 == b2) {
		return;
	}

	ODEWorld *world = ODEWorld::get();
	dGeomID ground = world->getGround();

	if (b1 && b2) {
		if (dAreConnected(b1, b2)) { 
			return; 
		}
	}
	if (b1) {
		void *d = dBodyGetData(b1);
		if (d) {
			p1 = (SParts*)d;
			if (p1->isBlind()) { return; }
		}
	}
	
	if (b2) {
		void *d = dBodyGetData(b2);
		if (d) {
			p2 = (SParts*)d;
			if (p2->isBlind()) { return; }
		}
	}

	if (p1 && p2 && p1->sameParent(*p2)) { return; }

	if (dGeomIsSpace(o1) && dGeomIsSpace(o2)) {
		dSpaceCollide2(o1, o2, data, &collideCB);
		return;
	}


#define F_SCHOLAR(V) sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2])

	static dJointFeedback fb;

#define MAX_COLLISION 32
	const int N = MAX_COLLISION;
	dContact contacts[N];
	int n = dCollide(o1, o2, N, &(contacts[0].geom),  sizeof(contacts[0]));
	if (n > 0) {
		ODEWorld *world = ODEWorld::get();
		for (int i=0; i<n; i++) {
			dContact *c = &contacts[i];
			dContactGeom &g = c->geom;

			if (p1 && p2) {
#if 0
				printf("Collision #%d %s(%s) - %s(%s) \n", cnt,
					   p1->getParent()->name(), p1->name(),
					   p2->getParent()->name(), p2->name());
				printf("\tpos = (%f, %f, %f)\n", g.pos[0], g.pos[1], g.pos[2]);
				printf("\tnormal = (%f, %f, %f)\n", g.normal[0], g.normal[1], g.normal[2]);
				printf("\tfdir = (%f, %f, %f)\n", c->fdir1[0], c->fdir1[1], c->fdir1[2]);
#endif
				const char *name1 = p1->getParent()->name();
				const char *name2 = p2->getParent()->name();
				std::string key = chash_key(name1, name2);
				if (key.length() <= 0) { continue; }
				if (chash_find(key)) { continue; }
				s_chash[key] = true;
				s_collisions.push_back(ODEWorld::Collision(name1, name2, p1->name(), p2->name()));
				// Set the collision flag to be ON
				p1->setOnCollision(true);
				p2->setOnCollision(true);
			}
			
			//c->surface.mode = dContactBounce;
			c->surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactBounce;
			//c->surface.mode = dContactSlip1 | dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactBounce;
			//
			// Reflection of material parameters of the collided object
			// Fliction force should be regarded as average of contiguous material (???)
			// TODO: Calclation of fliction force sould be considered
			//
			
			if(odeobj1 && odeobj2) {
				c->surface.mu       = ( odeobj1->getMu1()     + odeobj2->getMu1() )     / 2.0;
				c->surface.mu2      = ( odeobj1->getMu2()     + odeobj2->getMu2() )     / 2.0;
				c->surface.slip1    = ( odeobj1->getSlip1()   + odeobj2->getSlip1() )   / 2.0;
				c->surface.slip2    = ( odeobj1->getSlip2()   + odeobj2->getSlip2() )   / 2.0;
				c->surface.soft_erp = ( odeobj1->getSoftErp() + odeobj2->getSoftErp() ) / 2.0;
				c->surface.soft_cfm = ( odeobj1->getSoftCfm() + odeobj2->getSoftCfm() ) / 2.0;
				c->surface.bounce   = ( odeobj1->getBounce()  + odeobj2->getBounce() )  / 2.0;
			}
			else {
				c->surface.bounce_vel = world->getCollisionParam("bounce_vel");
				c->surface.bounce     = world->getCollisionParam("bounce");
				c->surface.mu         = world->getCollisionParam("mu");
				c->surface.mu2        = world->getCollisionParam("mu2");
				c->surface.slip1      = world->getCollisionParam("slip1");
				c->surface.slip2      = world->getCollisionParam("slip2");
				c->surface.soft_erp   = world->getCollisionParam("soft_erp");
				c->surface.soft_cfm   = world->getCollisionParam("soft_cfm");
				/*
				  c->surface.mu	 = 0.5;       // for fliction coefficient between stone and cray
				  c->surface.mu2 = 0.02;
				  c->surface.soft_erp = 0.8;  // parameter for modify the error of Joint position (0..1); if it is 1, modification will be perfect
				  c->surface.soft_cfm = 0.01; // If this value=0, joint velocity is strange
				  c->surface.bounce = 0.3;    // is a little smaller than ball
				*/
			}
			//c->surface.bounce = 0.0;
			//c->surface.bounce_vel = 0.0;

			dJointID cj = dJointCreateContact(world->world(),
							  world->jointGroup(), c);
			dJointAttach(cj, dGeomGetBody(o1), dGeomGetBody(o2));

#if 0
			if (p1 && p2) {
				dJointSetFeedback(cj, &fb);
				dJointFeedback *pfb = dJointGetFeedback(cj);
				if (F_SCHOLAR(pfb->f1) > 1.0) {
					printf("\tF1 = (%f, %f, %f)\n", pfb->f1[0], pfb->f1[1], pfb->f1[2]);
					printf("\tF2 = (%f, %f, %f)\n", pfb->f2[0], pfb->f2[1], pfb->f2[2]);
				}
			}
#endif
		}
	}
}



//	Initialization of instance(Behave as Singleton)
ODEObjectContainer *ODEObjectContainer::instance = NULL;

/**
 * Generate ODEObj instance
 */
ODEObj *ODEObjectContainer::createODEObj(
	dWorldID world,
	dGeomID geom_,
	double surfaceMu1,
	double surfaceMu2,
	double surfaceSlip1,
	double surfaceSlip2,
	double surfaceSoftErp,
	double surfaceSoftCfm,
	double surceBounce
)
{
	ODEObj *pObj = new ODEObj(
	                          world,
	                          geom_,
	                          surfaceMu1,
	                          surfaceMu2,
	                          surfaceSlip1,
	                          surfaceSlip2,
	                          surfaceSoftErp,
	                          surfaceSoftCfm,
	                          surceBounce
	                          );

	odeObjTable[geom_] = pObj;
	return pObj;
}


ODEObjectContainer *ODEObjectContainer::getInstance()
{
	if(instance == NULL) {
		instance = new ODEObjectContainer();
	}
	return instance;
}


ODEObj *ODEObjectContainer::getODEObjFromGeomID(dGeomID geom_)
{
	std::map<dGeomID,ODEObj*>::iterator itr = odeObjTable.find(geom_);
	if(itr != odeObjTable.end()) {
		return odeObjTable[geom_];
	}
	else {
		return NULL;
	}
}

SSimRobotEntity *ODEObjectContainer::getSSimRobotEntityFromGeomID(dGeomID geom_)
{
	std::map<dGeomID,SSimRobotEntity*>::iterator itr = m_allEntities.find(geom_);
	if(itr != m_allEntities.end()) {
		return m_allEntities[geom_];
	}
	else {
		return NULL;
	}
}

#endif // USE_ODE
