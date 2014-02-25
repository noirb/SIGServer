/*
 * Written by okamoto on 2011-03-25
 * This file should be removed: by inamura on 2013-12-29
 */
// Just a sample program, not used in the main SIGServer
// Code for control a joint from command prompot mode. (not used now)

#ifdef DEPRECATED

#ifdef IRWAS_SIMSERVER

#include "CommandImpl.h"
#include "SSimObj.h"
#include "ODEObj.h"

bool MoveCommand::apply(double t, SSimObj &agent)
{
	LOG_MSG(("MoveCommand\n"));
	
	bool ret = false;
	ODEObj &obj = agent.getSBody()->odeobj();
	dBodyID body = obj.body();

	dMass m;
	dBodyGetMass(body, &m);
	LOG_MSG(("mass = %f\n", m.mass));

	const Vector3d &dest = destination();

	Vector3d dir(dest);
	{
		const dReal *v = dBodyGetPosition(body);
		Vector3d pos(v[0], 0, v[2]);
		dir -= pos;
	}

	const dReal *f_ = dBodyGetForce(body);
	Vector3d currF(f_[0], f_[1], f_[2]);

	double dist = dir.length();

	if (dist < 0.1 && currF.length() < 0.1) {
		LOG_MSG(("arrived\n"));
		agent.setForce(0.0, 0.0, 0.0);
		ret = true;
	} else if (dist < 1.0) {
		double angle = currF.angle(dir);
		if (angle > 0) {
			Vector3d f(currF);
			f *= -10;
			agent.setForce(f.x(), f.y(), f.z());
		} else {
			if (currF.length() < 0.05) {
				Vector3d f(dir);
				f.normalize();
				agent.setForce(f.x(), f.y(), f.z());
			}
					       
		}
	} else {
		LOG_MSG(("F = (%f, %f, %f)\n", currF.x(), currF.y(), currF.z()));

		if (currF.length() < 1.0) {
			Vector3d f(dir);
			f.normalize();
			LOG_MSG(("accelerate\n"));
			agent.setForce(f.x(), f.y(), f.z());
		}

		LOG_MSG(("F = (%f, %f, %f)\n", currF.x(), currF.y(), currF.z()));
	}
	{
		Vector3d pos;
		agent.getPosition(pos);
		
		LOG_MSG(("destination : (%f, %f, %f)\n", dest.x(), dest.y(), dest.z()));
		LOG_MSG(("current position : (%f, %f, %f)\n", pos.x(), pos.y(), pos.z()));
		LOG_MSG(("distance : %f\n", dist));

	}
	return ret;
}

#endif
#endif // DEPRECATED
