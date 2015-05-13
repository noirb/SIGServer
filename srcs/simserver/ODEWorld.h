/*
 * Modified by okamoto on 2012-03-19
 */

#ifndef ODEWorld_h
#define ODEWorld_h

#include "systemdef.h"

#include <string>
#include <vector>

#ifdef USE_ODE

#include <ode/ode.h>
#include "ODEObj.h"
#include "SimGround.h"
#include "SSimEntity.h"

//static dGeomID _ground;

class ODEWorld
{
public:
	struct Gravity
	{
		dReal x, y, z;
		Gravity(dReal x_, dReal y_, dReal z_) : x(x_), y(y_), z(z_) {;}
	};

	struct Collision
	{
		std::string body1;
		std::string body2;
		std::string parts1;
		std::string parts2;
		Collision(std::string b1, std::string b2, std::string p1, std::string p2) : body1(b1), body2(b2), parts1(p1), parts2(p2){}
	};

private:
	dWorldID      m_world;
	dSpaceID      m_space;
	dJointGroupID m_jgroup;
	double        m_timeStep;
	double        m_nowtime;
	unsigned      m_itrCnt;
	dGeomID       m_ground;
	bool          m_quickStep;

	double  m_mu;
	double  m_mu2;
	double  m_slip1;
	double  m_slip2;
	double  m_soft_erp;
	double  m_soft_cfm;
	double  m_bounce;
	double  m_bounce_vel;
	
private:
	ODEWorld(const Gravity &g, dReal erp);
	void free_();
public:
	~ODEWorld() { free_(); }
	dWorldID world() { return m_world; }
	dSpaceID space() { return m_space; }
	dJointGroupID jointGroup() { return m_jgroup; }
	void   nextStep();
	void   nextStep(double stepsize, bool quick = false);
	void   resetTime() { m_itrCnt = 0; }
	double time() const { return m_nowtime; }

	double step() const { return m_timeStep; }

	void setStepSize(double stepsize) { m_timeStep = stepsize; }

	void setQuickStep(bool flag){ m_quickStep = flag; }
	
	unsigned stepCount() const { return m_itrCnt; }
	//! 
	void setGround(SimGround *ground) {m_ground = ground->getGeom();}
	//! 
	dGeomID getGround() {return m_ground;}

	void setERP(double value);
	
	void setCFM(double value);

	void setCollisionParam(std::string name, double value);

	double getCollisionParam(std::string name);

	int collideWith(const char *, std::vector<std::string> &);

private:
	static ODEWorld *s_inst;
public:
	static ODEWorld *create(const Gravity &g, dReal erp);
	static ODEWorld *get();
};



class ODEObjectContainer {
public:
	std::map<dGeomID,ODEObj*> odeObjTable;
	std::map<dGeomID, SSimRobotEntity*> m_allEntities;
	/**
	 * Generate ODEObj instance
	 */
	ODEObj *createODEObj(
		dWorldID world,
		dGeomID geom_,
		double surfaceMu1,
		double surfaceMu2,
		double surfaceSlip1,
		double surfaceSlip2,
		double surfaceSoftErp,
		double surfaceSoftCfm,
		double surceBounce
	);

	static ODEObjectContainer *getInstance();

	ODEObj *getODEObjFromGeomID(dGeomID geom_);
	SSimRobotEntity *getSSimRobotEntityFromGeomID(dGeomID geom_);

private:
	static ODEObjectContainer *instance;
};


#endif

#endif // ODEWorld_h


