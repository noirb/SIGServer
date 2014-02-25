/*
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add English comments (Translation from v2.2.0 is finished)
 *    Add TODO comments
 */

#ifndef Joint_h
#define Joint_h

#include "systemdef.h"
#include "Vector3d.h"
#include "Rotation.h"

#include <string>

#ifdef USE_ODE
#include <ode/ode.h>

class JointForce;
class SParts;
class HingeJoint;

/**
 * @brief Joint Class to connect two parts
 */
class Joint
{
public:
	//! type of parts
	enum Type {
		TYPE_NOT_SET = -1,
		TYPE_FIXED, TYPE_BALL, TYPE_HINGE,
	};
protected:
	enum { BODY_NUM = 2, };
protected:
	Type            m_type;
	std::string     m_name;
	dWorldID        m_world;
	dJointID        m_joint;
	dJointFeedback *m_jfb;
	bool            m_attach;
	int             m_bodyNum;
	dBodyID         m_bodies[BODY_NUM];
	Vector3d        m_anchor;
	Vector3d        m_rotv;
	bool            m_fixed;

	// state
	Rotation	m_rot;

	// Rotation value which is the latest value sent to SIGViewer (for detection of value change)
	// TODO: v is too short to explain the 'sent to viewer'. More friendly naming is requierd.: by inamura
	Rotation	m_vrot;

	// Initial joint angle
	Rotation	m_inirot;

	// The latest initial orientation that is sent to the Viewer
	// TODO: v is too short to explain the 'sent to viewer'. More friendly naming is requierd.: by inamura
	Rotation	m_inivrot;

public:
	/**
	 * @brief Constructor
	 * @param t     joint type
	 * @param name  joint name
	 */
 	Joint(Type t, const char *name) : m_type(t), m_name(name), m_world(0), m_joint(0), m_jfb(NULL), m_attach(false), m_bodyNum(0), m_fixed(false) {;}

	//! Destructor
	virtual ~Joint();

	//! Get joint type
	Type         type() { return m_type; }

	//! Get joint name
	const char * name() { return m_name.c_str(); }

	//! Get Joint
	dJointID     joint() { return m_joint; }

	//! Set the World objects in ODE
	void	setWorld(dWorldID w);

	/**
	 *  @brief Set Body objects which is connected by the joint
	 * 
	 *  Max # of objects to be added is 2
	 */
	void	pushBody(dBodyID body);

	/**
	 * @brief Implementation of joint according to configuration
	 *
	 * @param v Position of the first part
	 * @param r Rotational matrix of the first part
	 * @param dynamics flag for dynamics simulation
	 */
	void	build(const Vector3d &v, const Rotation &r, bool dynamics);

	//! Set anchor position of the joint
	void	setAnchor(dReal x, dReal y, dReal z)
	{
		m_anchor.set(x, y, z);
	}

	//! Get part of target index
	SParts * getParts(int idx);

	//! Get anchor position of the joint
	const Vector3d & getAnchor() { return m_anchor; }

	//added by noma@tome (2012/02/27)
	/**
	 * @brief Get current anchor position (Absolute coordinate)
	 */
	virtual Vector3d getCurrentAnchorPosition() = 0;

	//! Get Initial Orientation
	const Rotation & getIniRotation() {  return m_inirot; }

	//! Get the latest Initial Orientation that is sent to viewer
	const Rotation & getIniVRotation() { return m_inivrot; }

	//! Get Orientation
	const Rotation & getRotation() {  return m_rot; }

	//! Get the latest orientation that is sent to viewer
	const Rotation & getVRotation() { return m_vrot; }

	/*
	//! get rotational angle from ODE
	Rotation getRotationFromODE() { 
	  Rotation rot;
	  // get angle from ODE directly
	  if(m_type == TYPE_HINGE) {
	    double angle = -dJointGetHingeAngle(m_joint);
	    dVector3 axis;
	    //dJointGetHingeAxis(m_joint, axis);
	    //rot.setAxisAndAngle(axis[0], axis[2], axis[1], angle);
	    //HingeJoint *hjoi = (HingeJoint*)this;
	    //double x = hjoi->getAxisX();
	    rot.setAxisAndAngle(m_axis.x(), 0, 0, angle);
	  }
	  return rot;
	}
	*/

	//! Set the latest orientation that is sent to viewer
	void setVQuaternion(double qw, double qx, double qy, double qz)
	{
	  m_vrot.setQuaternion(qw, qx, qy, qz);
	}

	//! Set the latest initial orientation that is sent to viewer
	void setIniVQuaternion(double qw, double qx, double qy, double qz)
	{
	  m_inivrot.setQuaternion(qw, qx, qy, qz);
	}

	/**
	 * @brief  Get joint torque
	 * @param  jfs   joint torque
	 * @retval true  success
	 * @retval false failure
	 */
	// TODO: API name is bad, Force should be changed to torque: by inamura
	bool getJointForces(JointForce *jfs);

	/**
	 * @brief Set joint angle
	 * @param d joint angle (radian)
	 */
	virtual void	setAngle(double d) = 0;

	//added by okamoto (2011/2/18)
	/**
	 * @brief Set Quaternion
	 * @param qw qw w element
	 * @param qx qx x element
	 * @param qy qy y element
	 * @param qz qz z element
	 */
	void	setQuaternion(double qw, double qx, double qy, double qz) ;

	//added by okamoto (2011/2/18)
	/**
	 * @brief Set OffsetQuaternion. Initial orientation is determined by this quaternion.
	 * @param qw qw w element
	 * @param qx qx x element
	 * @param qy qy y element
	 * @param qz qz z element
	 * @param iniAnchor Anchor position of the rotated joint
	 * @param cnt counter for the child joints
	 */
	void	setOffsetQuaternion(double qw, double qx, double qy, double qz, Vector3d iniAnchor, int cnt = 0) ;

	//added by okamoto (2011/3/4)
	/**
	 * @brief Add torque to the joint
	 * @param t   torque value [Nm]
	 */
	virtual void addTorque(double t) = 0;

#ifdef _DEBUG
	void	dump();
#else
	void	dump() {}
#endif
protected:
	/**
	 * @brief Set Anchor position
	 */
	virtual void	applyAnchor(dReal x, dReal y, dReal z) = 0;

 public:	
	/**
	 * @brief Create joint object in ODE
	 * @param b1 The first part
	 * @param b2 The second part
	 */
	virtual dJointID	createJoint(dBodyID b1, dBodyID b2) = 0;
};


/**
 * @brief Class for fixed joint
 */
class FixedJoint : public Joint
{
public:
	/**
	 * @brief Constructor
	 * @param name joint name
	 */
	FixedJoint(const char *name) : Joint(TYPE_FIXED, name)
	{
		m_fixed = true;
	}

private:
	void	applyAnchor(dReal x, dReal y, dReal z) {;}

	dJointID	createJoint(dBodyID b1, dBodyID b2)
	{
		dJointID j = dJointCreateFixed(m_world, 0);
		dJointAttach(j, b1, b2);
		return j;
	}

	void	setAngle(double d);
	void	addTorque(double t);

	//added by noma@tome (2012/02/27)
	/**
	 * @brief Get the current joint angle
	 * Since ODE does not have API which answer the anchor position of FixedJoint, initial value is returned.
	 */
	Vector3d getCurrentAnchorPosition(){return m_anchor;};
};


/**
 * @brief Class for ball joint(BALL)
 */
class BallJoint : public Joint
{
public:
	/**
	 * @brief Constructor
	 * @param name joint name
	 */
	BallJoint(const char *name) : Joint(TYPE_BALL, name) {;}

private:
	void	applyAnchor(dReal x, dReal y, dReal z)
	{
		dJointSetBallAnchor(m_joint, x, y, z);
	}

	dJointID	createJoint(dBodyID b1, dBodyID b2)
	{
		dJointID j = dJointCreateBall(m_world, 0);
		dJointAttach(j, b1, b2);
		return j;
	}
	void	setAngle(double d);
	void	addTorque(double d);

	//added by noma@tome (2012/02/27)
	Vector3d getCurrentAnchorPosition();
};


/**
 * @brief Class for hinge joint (HING)
 */
class HingeJoint : public Joint
{
private:
	Vector3d m_axis;
	double	 m_angle;

	bool     m_mirror;
public:
public:
	/**
	 * @brief Constructor
	 * @param name joint name
	 * @param axis rotational axis
	 */
 HingeJoint(const char *name, const Vector3d &axis) : Joint(TYPE_HINGE, name), m_axis(axis), m_angle(-10), m_mirror(false) {;}

	~HingeJoint() {
	}

	//! Set rotational axis
	void setAxis(double ax, double ay, double az) {
		m_axis.set(ax, ay, az);
	}

	//! Get rotational axis
	double getAxisX() {
		return m_axis.x();
	}
	double getAxisY() {
		return m_axis.y();
	}
	double getAxisZ() {
		return m_axis.z();
	}

	/**
	 * @brief  Get rotational (angle) value
	 * @return angle value (radian)
	 */	 
	double	getAngle() {
		// right-handed system or left-handed system
		if(m_mirror) return  dJointGetHingeAngle(m_joint);
		else         return -dJointGetHingeAngle(m_joint);
	}

	void	setAngle(double angle);
	void	addTorque(double t);
	

	/**
	 * @brief Set angular velocity
	 * @param v angular velocity [rad]
	 * @param max max torque [Nm]
	 */
	//modified by okamoto@tome (2011/3/9)
	//modified by inamura on 2013-12-30 from setVelocity
	void	setAngularVelocityMaxTorque(dReal v, dReal max)
	{
		dJointSetHingeParam(m_joint, dParamVel, -v);
		dJointSetHingeParam(m_joint, dParamFMax, max);
	}


private:
	void	applyAnchor(dReal x, dReal y, dReal z);

	dJointID	createJoint(dBodyID b1, dBodyID b2);

	//added by noma@tome (2012/02/27)
	Vector3d getCurrentAnchorPosition();
};


#endif // USE_ODE

#endif // Joint_h
 

