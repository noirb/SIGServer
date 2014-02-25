/* $Id: ODEObj.h,v 1.3 2011-04-18 01:33:26 kawamoto Exp $ */ 
#ifndef ODEObj_h
#define ODEObj_h


#include "systemdef.h"

#ifdef USE_ODE

#include <ode/ode.h>
#include <map>


//class ODEObj;

class ODEObj
{
private:
	dBodyID  m_body;
	dGeomID  m_geom;

	dJointID m_joint;
	dWorldID m_world;

	double surfaceMu1;		
	double surfaceMu2;		
	double surfaceSlip1;		//	
	double surfaceSlip2;
	double surfaceSoftErp;
	double surfaceSoftCfm;
	double surfaceBounce;


public:

	ODEObj(
		dWorldID world,
		dGeomID geom_,
		double surfaceMu1,
		double surfaceMu2,
		double surfaceSlip1,
		double surfaceSlip2,
		double surfaceSoftErp,
		double surfaceSoftCfm,
		double surfaceBounce
		) : m_geom(geom_)
	{
		this->surfaceMu1 = surfaceMu1;
		this->surfaceMu2 = surfaceMu2;
		this->surfaceSlip1 = surfaceSlip1;
		this->surfaceSlip2 = surfaceSlip2;
		this->surfaceSoftErp = surfaceSoftErp;
		this->surfaceSoftCfm = surfaceSoftCfm;
		this->surfaceBounce = surfaceBounce;
		m_body = dBodyCreate(world);	
		dGeomSetBody(geom_, m_body);
		m_world = world;
	}

        ODEObj(dWorldID world, dGeomID geom_)
	  : m_geom(geom_)
	{
	  m_body = dBodyCreate(world);
	  dGeomSetBody(geom_, m_body);
	  m_world = world;
	}
        ODEObj(dGeomID geom, dBodyID body, dJointID joint)
	  : m_geom(geom), m_body(body), m_joint(joint)
	{

	}


	ODEObj(dWorldID world)
		: m_geom(0)
	{
		m_body = dBodyCreate(world);
		m_world = world;
	}
	~ODEObj()
	{
		// 070821 yoshi@msi cause segmentation fault
		/*
		if (m_geom) {
			dGeomDestroy(m_geom);
		}

		dBodyDestroy(m_body);
		*/
	}

	/**
	 * Get dBodyID
	 */
	dBodyID body()
	{
		return m_body;
	}

	/**
	 * Get dGeomID
	 */
	dGeomID geom() {
		return m_geom;
	}

	double getMu1() {
		return surfaceMu1;
	}

	dWorldID world(){ return m_world;}

	double getMu2() {
		return surfaceMu2;
	}

	double getSlip1() {
		return surfaceSlip1;
	}

	double getSlip2() {
		return surfaceSlip2;
	}

	double getSoftErp() {
		return surfaceSoftErp;
	}

	double getSoftCfm() {
		return surfaceSoftCfm;
	}

	double getBounce() {
		return surfaceBounce;
	}

};

#endif // USE_ODE

#endif // ODEObj_h
 

