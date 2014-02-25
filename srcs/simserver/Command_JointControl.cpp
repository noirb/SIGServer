/*
 * Written by okamoto on 2011-03-25
 * This file should be removed: by inamura on 2013-12-29
 */
// Just a sample program, not used in the main SIGServer
// Code for control a joint from command prompot mode. (not used now)

#ifdef DEPRECATED
#include "CommandImpl.h"
#include "Logger.h"
#include "SSimObj.h"
#include "Joint.h"

//#define PI 3.1415926536  // TODO: why M_PI is not used? by inamura

bool JointControlCommand::apply(double t, SSimObj &agent)
{
#if 0
	LOG_DEBUG1(("%f : %f\n", t, m_startTime));
	if (m_startTime < 0) {
		m_startTime = t;
	} else {
		if (t - m_startTime > 1) {
			LOG_DEBUG1(("This command takes too log. Ignored\n"));
			return true;
		}
	}
		
	unsigned  complete = 0;
	for (M::iterator i=m_values.begin(); i!=m_values.end(); i++) {
		S name = i->first;
		Joint *j = agent.getJoint(name.c_str());
		if (!j) {
			LOG_ERR(("Agent(%s) : no joint named \"%s\"\n", agent.name(), name.c_str()));
			continue;
		}
#if 0
		double targetAngle = i->second;
		//		while (targetAngle < 0) { targetAngle += 2*PI; }

		j->setAngle(targetAngle);

#else
		if (j->type() != Joint::TYPE_HINGE) {
			LOG_ERR(("Joint \"%s\" is not HINGE\n", name.c_str()));
			continue;
		}
		double targetAngle = i->second;
		while (targetAngle < 0) { targetAngle += 2 * M_PI; }
		
		HingeJoint *hj = static_cast<HingeJoint*>(j);
		double angle = hj->getAngle();
		while (angle < 0) { angle += 2 * M_PI; }
		
		double d = targetAngle - angle;
		while (d < 0) { d += 2 * M_PI; }
		LOG_DEBUG1(("%s %f(%f) d = %f\n", name.c_str(), angle, targetAngle, d));
		if (fabs(d) > 5*M_PI/180.0) {
			int sign = d < M_PI? 1:-1;
			LOG_DEBUG1(("\tv = %f", sign*M_PI/8));
			hj->setVelocity(sign*M_PI/4);
		} else {
			hj->setVelocity(0);
			complete++;
		}
#endif
	}
	return complete == m_values.size() ? true: false;
#endif
	return true;
}


#endif // DEPRECATED
