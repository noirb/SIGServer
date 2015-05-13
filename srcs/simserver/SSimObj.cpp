/* 
 * Written by okamoto on 2011-12-28
 * Modified by Tetsunari Inamura on 2013-12-29
 *   Add English comments (Translation from v2.2.0 is finished)
 * Modified by Tetsunari Inamura on 2013-12-30
 *   Change setVelocity to setLinearVelocity
 */

#include "SSimObj.h"
#include "SimObjBase.h"
#include "ODEObj.h"
#include "Joint.h"
#include "Logger.h"

SSimObj::SSimObj(dSpaceID parent) : Super() , m_space(0), m_gmode(true), m_ID(0), m_type(0), m_grasped(false)
{
	m_space = dSimpleSpaceCreate(parent);
	dSpaceSetCleanup(m_space, 0);
}


SSimObj::~SSimObj()
{
	for (PartsM::iterator i=m_parts.begin(); i!=m_parts.end(); i++) {
		SParts *p = i->second;
		delete p;
	}
	m_parts.clear();

	for (JointM::iterator i=m_joints.begin(); i!=m_joints.end(); i++) {
		Joint *p = i->second;
		delete p;
	}
	m_joints.clear();	// fix(sekikawa)(2007/11/30)
}

SSimObj::Id SSimObj::s_cnt = 0;

void SSimObj::addId()
{
	s_cnt++;
	m_id = s_cnt;
}

void SSimObj::push(Joint *p)
{
	m_joints[p->name()] = p;
}


/**
 * @brief  Refer the # of joints
 * @return # of joints
 */
int SSimObj::getJointSize()
{
	return m_joints.size();
}

/**
 * @brief Refer the target joint
 * @param s name of joint
 */
Joint * SSimObj::getJoint(const char *s)
{
	if(m_joints.find(s) != m_joints.end()) 
		return m_joints[s];
	else
		return NULL;
}

bool SSimObj::removeJoint(Joint *j)
{
	JointM::iterator i;

	for (i=m_joints.begin(); i!=m_joints.end(); i++) {
		Joint *p = i->second;
		if (p == j) {
			break;
		}
	}
	if (i != m_joints.end()) {
		m_joints.erase(i);
		return true;
	} else {
		return false;
	}
}

void SSimObj::applyParams(bool init, double stepWidth)
{
	SParts *body = getSBody();
	const bool dyn = dynamics();
	dBodyID dbody = body->odeobj().body();

	for (PartsM::iterator i=m_parts.begin(); i!=m_parts.end(); i++) {

		SParts *sp = i->second;
		if(sp->getOnCollision()){
			sp->setOnCollision(false);
		}
	}
	if (init) {
		// Initial position and orientation
		Vector3d v(x(), y(), z());
		Rotation r;
		r.setQuaternion(qw(), qx(), qy(), qz());

		for (PartsM::iterator i=m_parts.begin(); i!=m_parts.end(); i++) {
			SParts *sp = i->second;
			//SParts *sp = dynamic_cast<SParts*>(p);
			sp->build(v, r, dyn);
		}

		for (JointM::iterator i=m_joints.begin(); i!=m_joints.end(); i++) {
			Joint *j = i->second;
			j->build(v, r, dyn);
		}

		double sum = 0.0;
		for (PartsM::iterator i=m_parts.begin(); i!=m_parts.end(); i++) {
			SParts *sp = i->second;
			if (sp->isBlind()) { continue; }
			double m = sp->getMass();
			sum += m;
		}
		mass(sum);
		LOG_DEBUG1(("Agent(%s) : mass = %f", name(), sum));

	}
	else {
		// get position
		const dReal *pos = dBodyGetPosition(dbody);
		Vector3d v(pos[0], pos[1], pos[2]);
		// get rotation
		const dReal *qua = dBodyGetQuaternion(dbody);
		Rotation r;
		r.setQuaternion(qua[0], qua[1], qua[2], qua[3]);

		if (m_ops) {
			LOG_DEBUG1(("operations : %#x", m_ops));
		}

		// movement in dynamics off mode
		if (!dyn) {
			// Find the rorating joint
			if(m_jvel.size() > 0) {
				std::map<std::string, double>::iterator it = m_jvel.begin();
				while(it != m_jvel.end()) {
					HingeJoint *joint = (HingeJoint*)getJoint((*it).first.c_str());
					double angle = joint->getAngle();
					// Rorate the joint by delta
					double delta = (*it).second * stepWidth;
					joint->setAngle(angle + delta);
					it++;
				}
			}

			// In a case of wheel based mobile robot
			if(m_type == 1) {
				//SRobotObj *robj = dynamic_cast<SRobotObj*>(this);
				SRobotObj *robj = (SRobotObj*)this;

				// Check whether the robot is moving
				if(robj != NULL && robj->getOnMove()) {
					// Refer wheel's angular velocity
					double lvel, rvel;
					robj->getWheelVelocity(lvel, rvel);

					double radius = robj->getWheelRadius();

					// Refer the orientation of the robot agent
					const dReal *qua = dBodyGetQuaternion(dbody);
					Rotation rot(qua[0], qua[1], qua[2], qua[3]);

					// When the right and left wheel velocities are the same
					if(lvel == rvel) {
						// Transfer the direction of movement in global coordinate
						Vector3d ini(0.0, 0.0, 1.0);
						Vector3d direction = ini.rotate(rot);

						// Calculate linear velocity of the robot body
						double vel = lvel*radius;
						// Calculate displacement in 1 step
						double delta = vel * stepWidth;
						direction *= delta;
						
						// Execution of the movement (in dynamics off mode)
						dBodySetPosition(dbody, pos[0] + direction.x(), pos[1] + direction.y(), pos[2] + direction.z());
					}
					else {
						// When the right and left wheel velocities are different
						double dis    = robj->getWheelDistance();       // distance between right and left wheel
						double vl     = lvel*radius;                    // linear velocity of the left wheel
						double vr     = rvel*radius;                    // linear velocity of the right wheel
						double vel    = (vl + vr) / 2;                  // linear velocity of the center of wheels
						double omega  = (vr - vl) / dis;                // angular velocity of the center of wheels
						double rho    = dis/2 * (vr + vl) / (vr - vl);  // turning radius
						double domega = omega * stepWidth;              // change of velocity in 1 step
						double z      = rho*sin(domega);                // next position in entity coordinate
						double x      = rho - rho*cos(domega);          // next position in entity coordinate

						// transform the direction of movement in global coordinate
						Vector3d ini(x, 0.0, z);
						Vector3d direction = ini.rotate(rot);

						// execution of the movement
						dBodySetPosition(dbody, pos[0] + direction.x(), pos[1] + direction.y(), pos[2] + direction.z());

						// calculate orientatoin after the movement
						rot.setAxisAndAngle(0.0, 1.0, 0.0, domega, -1.0);
						const dReal nqua[] = {rot.qw(), rot.qx(), rot.qy(), rot.qz()};

						// execution of the rotation
						dBodySetQuaternion(dbody, nqua);
					}
				}
			}
			// TODO: Is this needed?
			/*
			  if (m_ops & OP_SET_POSITION) {
			  body->setPosition(v);
			  LOG_DEBUG1(("Posotion (%f, %f, %f)", v.x(), v.y(), v.z()));
			  LOG_DEBUG1(("Position applied"));
			  }
			if (m_ops & OP_SET_ROTATION) {
				//Rotation r;
				//r.setQuaternion(qw(), qx(), qy(), qz());
				body->setQuaternion(r.q());
				LOG_DEBUG1(("Rotation applied"));
				}
			*/

			/*
			//	Set of linear velocity of the parts
			if(m_ops & OP_SET_VELOCITY)			body->setVelocity(vx(),vy(),vz());
			//	Set of angular velocity of the parts
			if(m_ops & OP_SET_ANGULAR_VELOCITY)	body->setAngularVelocity(avx(),avy(),avz());
			*/
		}
		// set linear velocity of the parts
		// Changed from setVelocity: by inamura on 2013-12-30
		if(m_ops & OP_SET_LINEAR_VELOCITY) {
			// Set linear velocity at ODE world
			body->setLinearVelocity(vx(),vy(),vz());  // body is an instance of SParts class
		}
		// set Angular velocity of the parts
		if(m_ops & OP_SET_ANGULAR_VELOCITY) {
			body->setAngularVelocity(avx(),avy(),avz());
		}
		// Force
		if (m_ops & OP_SET_FORCE) {
			body->setForce(fx(), fy(), fz());
			LOG_DEBUG1(("Force(%f, %f, %f) applied", fx(), fy(), fz()));
		}

		if (m_ops & OP_SET_TORQUE) {
			body->setTorque(tqx(), tqy(), tqz());
			LOG_DEBUG1(("Torque(%f, %f, %f) applied", tqx(), tqy(), tqz()));
		}
		m_ops = OP_NOT_SET;
	}
}


void SSimObj::loadParams()
{
	SParts *body = getSBody();

	const dReal *pos = body->getPosition();

	Vector3d v(pos[0], pos[1], pos[2]);
	x(v.x()); y(v.y()); z(v.z());

	const dReal *q = body->getQuaternion();
	qw(q[0]); qx(q[1]); qy(q[2]); qz(q[3]);

	// TODO: Are force and torque requierd as attribution?
	const dReal *f = body->getForce();
	fx(f[0]); fy(f[1]); fz(f[2]);

	const dReal *tq = body->getTorque();
	tqx(tq[0]); tqy(tq[1]); tqz(tq[2]);

	bool dyn = dynamics();
	if (!dyn) {
		// If dynamics mode is off, calculate the position and orientation by SIGServer
		body->calcPosture();
	}
}


#ifdef _DUBUG
void SSimObj::dump()
{
	Super::dump();

	for (JointM::iterator i=m_joints.begin(); i!=m_joints.end(); i++) {
		Joint *j = i->second;
		printf("\tJoint(%s) :", j->name());
		const Vector3d &av = j->getAnchor();
		printf("\t\tanchor = (%f, %f, %f) \n", av.x(), av.y(), av.z());
		j->dump();
	}
}
#endif


