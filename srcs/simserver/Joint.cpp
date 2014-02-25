/* 
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add English comments (Translation from v2.2.0 is finished)
 *    Add TODO comments
 */
#include "Joint.h"
#include "Logger.h"
#include "SParts.h"
#include "JointForce.h"
#include <assert.h>


Joint::~Joint()
{
	if (m_joint) {
		dJointDestroy(m_joint);
	}
	if (m_jfb) {
		delete m_jfb; m_jfb = NULL;
	}
}

void Joint::setWorld(dWorldID w)
{
	m_world = w;
}

void Joint::pushBody(dBodyID body)
{
	if (m_bodyNum < BODY_NUM) {
		m_bodies[m_bodyNum] = body;
		m_bodyNum++;
	}
}


SParts * Joint::getParts(int idx)
{
	if (idx < m_bodyNum) {
		dBodyID b = m_bodies[idx];
		return (SParts*)dBodyGetData(b);
	} else {
		return NULL;
	}
}

		

void Joint::build(const Vector3d &v, const Rotation &r, bool dynamics)
{
	if (m_bodyNum != BODY_NUM) { return; }
	assert(m_world);
	m_joint = createJoint(m_bodies[0], m_bodies[1]);

	Parts *parts = (Parts*) dBodyGetData(m_bodies[1]);
	assert(parts);
	double x, y, z;
	parts->givePosition(x, y, z);
	m_rotv.set(x, y, z);
	
	m_rotv -= m_anchor;
	
	Vector3d av = m_anchor;
	av.rotate(r);
	av += v;
	applyAnchor(av.x(), av.y(), av.z());

	if (m_fixed) {
		dJointSetFixed(m_joint);
	}

	if (dynamics) {
		m_jfb = new dJointFeedback;
		dJointSetFeedback(m_joint, m_jfb);
	}
}

// Set of offsetquaternion added by okamoto (2011/2/18)
void Joint::setOffsetQuaternion(double qw, double qx, double qy, double qz,Vector3d iniAnchor,  int cnt)
{
	// TODO: ODE joint is ignored now  : by okamoto
	// see the comments in v2.2.0's source by inamura on 2013-12-27
	m_inirot.setQuaternion(qw, qx, qy, qz);
}

// added by okamoto (2011/2/18)
void Joint::setQuaternion(double qw, double qx, double qy, double qz)
{
	m_rot.setQuaternion(qw, qx, qy, qz);
}


#define F_SCHOLAR(V) sqrt(V[0]*V[0] + V[1]*V[1] + V[2]*V[2])


bool Joint::getJointForces(JointForce *jfs)
{
	dJointFeedback *pfb = dJointGetFeedback(m_joint);
	if (pfb != NULL && F_SCHOLAR(pfb->f1) > 1.0) {

		int idx = 0;
		JointForce *jf = &jfs[idx];

		SParts *parts = (SParts*)dBodyGetData(m_bodies[idx]);
		if (!parts) { return false; }
		jf->set(parts->name(),
			Vector3d(pfb->f1[0], pfb->f1[1], pfb->f1[2]),
			Vector3d(pfb->t1[0], pfb->t1[1], pfb->t1[2]));

		idx++;

		jf = &jfs[idx];
		parts = (SParts*)dBodyGetData(m_bodies[idx]);
		if (!parts) { return false; }
		jf->set(parts->name(),
			Vector3d(pfb->f2[0], pfb->f2[1], pfb->f2[2]),
			Vector3d(pfb->t2[0], pfb->t2[1], pfb->t2[2]));
		
		return true;
	} else {
		return false;
	}

}

#ifdef _DEBUG
void Joint::dump()
{
	dJointFeedback *pfb = dJointGetFeedback(m_joint);
	if (pfb != NULL && F_SCHOLAR(pfb->f1) > 1.0) {
		printf("\tF1 = (%f, %f, %f)\n", pfb->f1[0], pfb->f1[1], pfb->f1[2]);
		printf("\tF2 = (%f, %f, %f)\n", pfb->f2[0], pfb->f2[1], pfb->f2[2]);
	}
}
#endif



void HingeJoint::applyAnchor(dReal x, dReal y, dReal z)
{
	dJointSetHingeAnchor(m_joint, x, y, z);

	// Set of max and min of joint angle
	dJointSetHingeAxis (m_joint, m_axis.x(), m_axis.y(), m_axis.z());
	dJointSetHingeParam(m_joint, dParamLoStop, -2.0*M_PI);
	dJointSetHingeParam(m_joint, dParamHiStop,  2.0*M_PI);
}

dJointID HingeJoint::createJoint(dBodyID b1, dBodyID b2)
{
	dJointID j = dJointCreateHinge(m_world, 0);
	dJointAttach(j, b1, b2);

	return j;
}
 
void HingeJoint::setAngle(double angle)
{
	LOG_DEBUG1(("Joint (%s)", name()));
	LOG_DEBUG1(("HingeJoint::setAngle (%f, %f, %f, %f)", m_axis.x(), m_axis.y(), m_axis.z(), angle));
	m_rot.setAxisAndAngle(m_axis.x(), m_axis.y(), m_axis.z(), angle);

}

//! Refer the current anchor position in world coordinate
// added by noma on 2012-02-27
Vector3d HingeJoint::getCurrentAnchorPosition()
{
	dReal tmpPos[4];
	dJointGetHingeAnchor(m_joint, tmpPos);
	return Vector3d(tmpPos[0],tmpPos[1],tmpPos[2]);
}


void FixedJoint::setAngle(double angle)
{
	LOG_DEBUG1(("Joint (%s)", name()));
	LOG_DEBUG1(("FixedJoint::setAngle %f", angle));
}

void BallJoint::setAngle(double angle)
{
	LOG_DEBUG1(("Joint (%s)", name()));
	LOG_DEBUG1(("BallJoint::setAngle %f", angle));
}


//! Refer the current anchor position in world coordinate
// added by noma on 2012-02-27
Vector3d BallJoint::getCurrentAnchorPosition()
{
	dReal tmpPos[4];
	dJointGetBallAnchor(m_joint, tmpPos);
	return Vector3d(tmpPos[0],tmpPos[1],tmpPos[2]);
}



//! Add torque to joint
void HingeJoint::addTorque(double torque)
{
	// - is required, because the direction is opposite from setAngle
	dJointAddHingeTorque(m_joint,-torque);
}
void FixedJoint::addTorque(double torque)
{
}
void BallJoint::addTorque(double torque)
{
}

