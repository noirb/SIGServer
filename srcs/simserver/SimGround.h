/* $Id: SimGround.h,v 1.2 2011-10-18 03:09:38 okamoto Exp $ */ 
#ifndef SimGround_h
#define SimGround_h

#include "systemdef.h"
#include "Logger.h"

#ifdef EXEC_SIMULATION
#include <ode/ode.h>
#endif

class SimGround
{
#ifdef EXEC_SIMULATION
private:
        dGeomID	 m_geom;
public:

	SimGround(dSpaceID s)
	{
		m_geom = dCreatePlane(s, 0.0, 1.0, 0.0, 0.0);
	}
	dGeomID getGeom(){return m_geom;}

#endif
public:
#ifdef IMPLEMENT_DRAWER
	void	draw(DrawContext &c);
#endif
};

#endif // SimGround_h
 

