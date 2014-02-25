/*
 * Modified by Okamoto on 2012-03-19
 */

#include "SSimObjBuilder.h"
#include "SSimObj.h"
#include "ODEWorld.h"
#include "Joint.h"
#include "ODEObj.h"
#include "Position.h"
#include "Logger.h"

//#define ENABLE_DUMP
#include "ParserDump.h"

SSimObjBuilder::SSimObjBuilder(SSimObj &o, ODEWorld &w) : m_obj(o), m_world(w)
{
	try {
		double sx = o.scalex();
		double sy = o.scaley();
		double sz = o.scalez();
		m_scale.set(sx, sy, sz);
	} catch(SimObjBase::NoAttributeException &e) {
		LOG_ERR((e.msg()));
		o.dump();
		throw e;
	}
}


bool SSimObjBuilder::addJoint(Joint *j)
{
	if (j) {
		DUMP(("Joint added : name=(%s)\n", j->name()));
		j->setWorld(m_world.world());
		m_obj.push(j);
	}

	return true;
}



bool SSimObjBuilder::addParts(SParts *parts)
{
	SParts *prev = m_obj.getSParts(parts->name());

	if (prev != NULL) {
		// If the part is already existed, return as error
		LOG_MSG(("SParts(name=%s) already exist [%s:%d]\n", parts->name(), __FILE__, __LINE__));
		return false;
	}
	else {
		DUMP(("Parts added : name=(%s)", parts->name()));
		// ===================================================
		// Setting of position of the parts
		// ===================================================
		// SParts has two types of position in different classes
		// [a] Position in Base class (=Parts::m_pos)
		// [b] Position hold by ODE object (=SParts::m_odeobj)
		//
		// The position specified in the SParts constructor is stored in [a]
		// in the moment, [b] has no position information. (ODE object has not been created.)
		//
		// This function copies the position of [a] to [b].
		// The following three steps are corresponded to the copy.
		// (step 1) Get information from [a]
		// (step 2) Create ODE object by calling SParts::set()
		// (step 3) Set the position acquired in step 1, into [b]
		
		// ******* (step 1) *******
		// To refer [a], Parts::givePosition() should be used. Parts is a base class.
		// because SParts::getPosition refers position of ODE, that is [b]
		double x1, y1, z1;
		parts->givePosition(x1, y1, z1);
		DUMP(("\t(x1, y1, z1)=(%f, %f, %f)", x1, y1, z1));
		
		//LOG_MSG(("sobj->x() = %f",sobj->x()));

		// ******* step2 *******
		// Create ODE object to call SParts::set()
		// ODE object never exist before this call
		//DUMP(("\tworld=%d, space=%d\n", m_world.world(), m_world.space()));
		parts->set(m_world.world(), m_world.space());
		//DUMP(("\todeobj=%d\n", &(parts->odeobj())));
		parts->addId();

		// ******* step3 *******
		// Set information of [a] into [b]
		Vector3d v(x1, y1, z1);
		parts->setPosition(v);

		DUMP(("\t(%s) step 3", __FILE__));
		m_obj.push(parts);

		// Setting of collision condition: added by okamoto on 2012-03-19
		SimObjBase* sobj = parts->getParent();
		DUMP(("\t(%s) step 4", __FILE__));
		if(m_obj.isAttr("collision")) {
			DUMP(("\t(%s) step 5", __FILE__));
			bool col = sobj->collision();
			DUMP(("\t(%s) step 6", __FILE__));
			if(!col) {
				parts->setCollisionEnable(false);
				DUMP(("\t(%s) step 6.true", __FILE__));
			}
			else
				DUMP(("\t(%s) step 6.false", __FILE__));
		}

		DUMP(("\t(%s) step 7", __FILE__));
		// Setting of mass after the creation of ODE object
		if(strcmp(parts->name(),"body") == 0) {
		    if(m_obj.isAttr("mass")){
				double mass = m_obj.mass();
				parts->setMass(mass);
		    }
		}
		else {
			// Set default mass when the mass is not specified
			parts->setMass(1.0); //TODO: Magic number
		}
		DUMP(("\t(%s) step 8", __FILE__));

		DUMP(("\t(%s) end", __FILE__));
		return true;
	}
}


void SSimObjBuilder::connect(Joint *joint, SParts *p1, SParts *p2)
{
	DUMP(("connect\n"));
	DUMP(("\t%s (%s - %s)\n", joint->name(), p1->name(), p2->name()));

	joint->pushBody(p1->odeobj().body());
	joint->pushBody(p2->odeobj().body());

	p2->setParentJoint(joint);	
}

void SSimObjBuilder::connect2(Joint *sibJ, Joint *childJ, SParts *sibP, SParts *childP)
{
	DUMP(("connect2 %s\n", sibP->name()));
	DUMP(("\tjoint (%s) child (parts->%s, joint->%s)\n",
		  sibJ->name(),
		  childP ? childP->name() : "null",
		  childJ ? childJ->name() : "null"
		  ));

	sibP->pushChild(sibJ, childJ, childP);
}

Vector3d & SSimObjBuilder::applyScaleV(Vector3d &v)
{
	double xx = v.x() * m_scale.x();
	double yy = v.y() * m_scale.y();
	double zz = v.z() * m_scale.z();
	v.set(xx, yy, zz);
	return v;
}

Position & SSimObjBuilder::applyScaleP(Position &v)
{
	double xx = v.x() * m_scale.x();
	double yy = v.y() * m_scale.y();
	double zz = v.z() * m_scale.z();
	v.set(xx, yy, zz);
	return v;
}

Size & SSimObjBuilder::applyScaleS(Size &v)
{
	double xx = v.x() * m_scale.x();
	double yy = v.y() * m_scale.y();
	double zz = v.z() * m_scale.z();
	v.set(xx, yy, zz);
	return v;
}


