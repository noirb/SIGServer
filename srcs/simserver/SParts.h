/*
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add English comments (Translation from v2.2.0 is finished)
 * Modified by Tetsunari Inamura on 2014-01-29
 *    Delete Magic numbers
 */

#ifndef Parts_Server_h
#define Parts_Server_h

#ifdef EXEC_SIMULATION
#include <vector>
#include "Logger.h"
#include <math.h>

#include "Parts.h"
#include "PartsCmpnt.h"

#define SPARTS_MU1     dInfinity
#define SPARTS_MU2     dInfinity
#define SPARTS_SLIP1   0.01
#define SPARTS_SLIP2   0.01
#define SPARTS_ERP     0.2
#define SPARTS_CFM     0.0001
#define SPARTS_BOUNCE  0.1
#define SPARTS_DAMPING 0.8


class Joint;

/**
 * @brief Class of entity parts used in server side
 *
 * @see CParts
 */
class SParts : public Parts
{
 public:
	struct Child
	{
		Joint  *currj;
		Joint  *nextj;
		SParts *nextp;

		Child(Joint *cj, Joint *nj, SParts *np) : currj(cj), nextj(nj), nextp(np) {}
	};
	typedef std::vector<Child *> ChildC;
 protected:
	ODEObj *     m_odeobj;
	SimObjBase * m_parent;
	Joint *      m_parentJoint;
	ChildC       m_children;
	double       m_posx, m_posy, m_posz;
	double       m_mass;
	std::string  m_graspObj;
	bool         m_onGrasp;     // flag whether it is grasped
	dQuaternion  m_gini;        // quaternion of the parts when the grasp is started
	bool         m_onCollision; // flag whether it is cllided
protected:
	/**
	 * @brief Constructor
	 * @param t     parts type
	 * @param name  parts name
	 * @param pos   position (agent coordinate)
	 */
	SParts(PartsType t, const char *name, const Position &pos);
public:
	virtual ~SParts();

	//! Set parent joint
	void	setParentJoint(Joint *j)
	{
		m_parentJoint = j;
	}
	// Get parent joint
	Joint * getParentJoint() { return m_parentJoint; }

	/**
	 * @brief Set child joint/parts
	 *
	 * connect via following way
	 * currj -> <this> -> nextj -> nextp
	 *
	 * @param currj
	 * @param nextj
	 * @param nextp
	 */
	void	pushChild(Joint *currj, Joint *nextj, SParts *nextp)
	{
		m_children.push_back(new Child(currj, nextj, nextp));
	}

	void	graspObj(std::string objName);

	void	releaseObj() 
	{
	  m_graspObj.clear();
	  m_onGrasp = false;
	  m_gini[0] = 1.0;
	  m_gini[1] = 0.0;
	  m_gini[2] = 0.0;
	  m_gini[3] = 0.0;
	}

	bool	getOnGrasp(){return m_onGrasp;}

	/**
	 * @brief Set child joint/parts
	 *
	 * This parts should be dummy; connect the previous parts and the next parts
	 *
	 * @param currj
	 * @param nextj
	 * @param nextp
	 */
	void	pushChild(Joint *nextj, SParts *nextp)
	{
		m_children.push_back(new Child(m_parentJoint, nextj, nextp));
	}

	//! Remove the child joint
	bool	removeChild(Joint *nextj);

	//! Set entity which is configured by this parts
	void	setParent(SimObjBase *p) { m_parent = p; }

	SimObjBase *	getParent() { return m_parent; }

	//added by okamoto@tome (2011/2/18)
	//! Get child joint
	ChildC		getChild()
	{
	  return m_children;
	}
	//added by okamoto@tome (2011/10/4)
	//! Set position of ODE object
	void setODEPos(double x, double y, double z)
	{
		m_posx = x;
		m_posy = y;
		m_posz = z;
	}
	//! Get position of ODE object
	void getODEPos(Vector3d &pos)
	{
		pos.x(m_posx);
		pos.y(m_posy);
		pos.z(m_posz);
	}
	//! Check whether the both of parts belonged to the same entity
	bool	sameParent(const SParts &o);

	//! Get the mass
	double	getMass();
	
	//added by okamoto @tome (2011/10/5)
	//! Set the mass
	void setMass(double mass);

	//! Get object name which is grasped by this parts
	std::string getGraspingObject(){return m_graspObj;}

private:
	// currj: the previous joint
	// anchorV: absolute position of previous joint anchor
	// R: rotation (whole)
	void	calcPosition(Joint *currj, Joint *nextj, const Vector3d &anchorV, const Rotation &R);

	//dReal maxAngularVel;
public:
	/**
	 * @brief Connection between parts and joint
	 *
	 * @param v	 central position of the agent (world coordinate)
	 * @param r	 orientation of the agent (world coordinate)
	 * @dynamics flag for dynamics calculation
	 */
	void	build(const Vector3d &v, const Rotation &r, bool dynamics);
	/**
	 * @brief Calculation of position and orientation of parts
	 *
	 * Available only when the dynamics mode is off
	 */
	void	calcPosture();

	//! Get wrapper class of ODE Body
	ODEObj &odeobj() { return *m_odeobj; }

	//! Switch the dynamics mode
	void	enableDynamics(bool b);

	//added by noma@tome 20120223
	/**
	 * @brief Get radius of circum sphere
	 *
	 */
	virtual double getCircumRadius(void) = 0;
	/**
	 * @brief Calculate cube root of the volume
	 *
	 */
	virtual double getCubicRootOfVolume(void) = 0;



	// --------------------------------------------------------
	// Access function to member variables of Parts class (ex: m_pos, m_rot)
	//
	// Position and orientation value set by the following functins are reflected in
	// ODE body using build().
	// In other words, only the setting value does not make reflection.
	// call of build() is required to be reflected in ODE world.
	// --------------------------------------------------------
	//! Set initial position of parts (agent coordinate)
	void	setInitialPosition(double x, double y, double z)
	{
		m_pos.set(x, y, z);
	}
	//! Set initial position of parts (agent coordinate)
	void	setInitialPosition(const Vector3d &v)
	{
		m_pos.set(v.x(), v.y(), v.z());
	}

	//! Set initial orientation (by quaternion) of the parts (agent coordinate)
	void	setInitialQuaternion(dReal q0, dReal q1, dReal q2, dReal q3);

	//! Set initial orientation (by rotation matrix) of the parts (agent coordinate)
	void	setInitialQuaternion(Rotation &r)
	{
		const dReal *q = r.q();
		setInitialQuaternion(q[0], q[1], q[2], q[3]);
	}

	// -----------------------------------------------------
	// Access function to ODE body
	// -----------------------------------------------------

	//! Set position in world coordinate (Only for BODY)
	void	setPosition(const Vector3d &v);

	//! Set Force added to the parts
	void	setForce(dReal fx, dReal fy, dReal fz);
	
	// added by okamoto@tome(2011/12/21)
	//! Set Force added to the parts (global coordinate)
	void	addForce(dReal fx, dReal fy, dReal fz);
	//! Set Force added to the parts (parts coordinate)
	void	addRelForce(dReal fx, dReal fy, dReal fz);
	//! Set force and point of action for the parts (global coordinate)
	void	addForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);
	//! Set force (parts coordinate) and point of action (global coordinate)
	void	addRelForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);
	//! Set force and point of action (parts coordinate)
	void	addForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);
	//! Set force (parts coordinate) and point of action (parts coordinate)
	void	addRelForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz);
	//! Get angular velocity of the parts
	void	getAngularVelocity(Vector3d &v);
	//! Get velocity of the parts // Changed from getVelocity: by inamura on 2013-12-30
	void	getLinearVelocity(Vector3d &v);
	//! Set flag whether gravity is given to the parts
	void	setGravityMode(bool gravity);

	// added by okamoto@tome(2012/3/16)
	//! Set flag whether collision detection is valid
	void    setCollisionEnable(bool collision);

	//! Set torque to the parts
	void	setTorque(dReal tx, dReal ty, dReal tz);

	/**
	 * Set linear velocity of the parts
	 * @param vx x element of velocity
	 * @param vy y element of velocity
	 * @param vz z element of velocity
	 */
	// Change from setVelocity: by inamura on 2013-12-30
	void setLinearVelocity(dReal vx,dReal vy,dReal vz);

	/**
	 * Set maximum angular velocity
	 */	
	//void setAngularMaxVelocity(dReal max);

	/**
	 * Set angular velocity of the parts
	 * @param x x element of angular velocity
	 * @param y y element of angular velocity
	 * @param z z element of angular velocity
	 */
	void setAngularVelocity(dReal x,dReal y,dReal z);

	//! Set orientation (rotate matrix) in global coordinate
	void	setRotation(const Rotation &r)
	{
		const dReal *q = r.q();
		SParts::setQuaternion(q[0], q[1], q[2], q[3]);
	}
	//! Set orientation (quaternion) in global coordinate
	void	setQuaternion(const dReal *q)
	{
		SParts::setQuaternion(q[0], q[1], q[2], q[3]);
	}
	//! Set orientation (quaternion) in global coordinate
	void	setQuaternion(dReal, dReal, dReal, dReal);

	//! Set flag to check collision detection
	void    setOnCollision(bool flag){m_onCollision = flag;}

	//! Get the result of collision detection
	bool    getOnCollision(){return m_onCollision;}

	//! Get the current position
	const dReal * getPosition();

	//! Get the current orientation in rotate matrix
	const dReal * getRotation();
	
	//! Get the current orientation in quaternion
	const dReal * getQuaternion();

	//! Get the force added to the parts
	const dReal * getForce();

	//! Get the torque added to the parts
	const dReal * getTorque();

public:
	virtual void set(dWorldID w, dSpaceID s) = 0;

	//! Dumping of the SParts contents
	void dump();
	void dumpConnectionInfo(int level);
	void dumpODEInfo(int level);
	void printIndent(int level);
};


/**
 * @brief Class of box parts for Server side
 */
class SBoxParts : public SParts
{
private:
	BoxPartsCmpnt m_cmpnt;
public:
	/**
	 * @brief Constructor
	 *
	 * @param name parts name
	 * @param pos  center position of parts
	 * @param sz   size of box (depth x width x height)
	 */
	SBoxParts(const char *name, const Position &pos, const Size &sz)
	  : SParts(PARTS_TYPE_BOX, name, pos), m_cmpnt(sz) {}
	/**
	 * @brief Constructor
	 *
	 * @param name parts name
	 * @param pos  center position of parts
	 * @param sz   size of box (depth x width x height)
	 * @param rot  rotation
	 */
 SBoxParts(const char *name, const Position &pos, const Size &sz, Rotation rot)
           : m_inirot(rot), SParts(PARTS_TYPE_BOX, name, pos), m_cmpnt(sz) {}

public:
	//! Get the size
	Size & getSize() { return m_cmpnt.size(); }

	//added by noma@tome 20120223
	/**
	 * @brief Get radius of circum sphere
	 *
	 */
	double getCircumRadius(void);
	/**
	 * @brief Get cube root of volume
	 *
	 */
	double getCubicRootOfVolume(void);

	//! Set size of the parts
	void setSize(dReal sx, dReal sy, dReal sz)
	{
		m_cmpnt.size(sx, sy, sz);
	}

private:
	// Initial orientation
	Rotation m_inirot;

	// SParts implementation
	void set(dWorldID w, dSpaceID s);

	// Parts implementation
	PartsCmpnt * extdata() { return &m_cmpnt; }
};


/**
 * @brief Class for cylinder parts in Server side
 */
class SCylinderParts : public SParts
{
private:
	CylinderPartsCmpnt m_cmpnt;
public:
	/**
	 * @brief Constructor
	 *
	 * @param name parts name
	 * @param pos  center position of parts
	 * @param rad  radius of the cylinder
	 * @param len  length of the cylinder
	 */
	SCylinderParts(const char *name, const Position &pos, dReal rad, dReal len)
                   : SParts(PARTS_TYPE_CYLINDER, name, pos), m_cmpnt(rad, len) {}

	/**
	 * @brief Constructor
	 *
	 * @param name parts name
	 * @param pos  center position of parts
	 * @param rad  radius of the cylinder
	 * @param len  length of the cylinder
	 * @param rot  initial orientation
	 */
	SCylinderParts(const char *name, const Position &pos, dReal rad, dReal len, Rotation rot)
                   :m_inirot(rot), SParts(PARTS_TYPE_CYLINDER, name, pos), m_cmpnt(rad, len) {}

	//	added by kawamoto@tome (2011/04/04)
	/**
	 * @brief Initialization of angle
	 *
	 * @param x x element
	 * @param y y element
	 * @param z z element
	 */
	void initializeAngle(double x,double y,double z);

	//! Get radius of the cylinder
	dReal getRadius() { return m_cmpnt.radius(); }
	
	//! Get length of the cylinder
	dReal getLength() { return m_cmpnt.length(); }

	//added by noma@tome 20120223
	/**
	 * @brief Get radius of circum sphere
	 */
	double getCircumRadius(void);
	/**
	 * @brief Calculate cube root of the volume
	 */
	double getCubicRootOfVolume(void);

	/**
	 * @brief Set radius of circum sphere
	 * @param r radius
	 */
	void setRadius(dReal r) { m_cmpnt.radius(r); }
	/**
	 * @brief Set length of circum sphere
	 * @param l length
	 */
	void setLength(dReal l) { m_cmpnt.length(l); }

private:
	// SParts implementation
	void set(dWorldID w, dSpaceID s);

	/**
	 * @brief creation of cylinder with specification of axis
	 * @param w
	 */
	void setFromAxis(dWorldID w, dSpaceID s,double x1,double y1,double z1,double x2,double y2,double z2);

	// Parts implementation
	PartsCmpnt * extdata() { return &m_cmpnt; }
	// initial orientation when the cylinder created
	double x_axis;
	double y_axis;
	double z_axis;
	double angleData;
	Rotation m_inirot;
};


/**
 * @brief Class for spherical parts in Server side
 */
class SSphereParts : public SParts
{
private:
	SpherePartsCmpnt m_cmpnt;
public:
	/**
	 * @brief Constructor
	 *
	 * @param name parts name
	 * @param pos  center position of the parts
	 * @param radius  radius of the sphere
	 */
	SSphereParts(const char *name, const Position &pos, double radius)
		: SParts(PARTS_TYPE_SPHERE, name, pos), m_cmpnt(radius) {}

	//! Get radius of the sphere
	double getRadius() { return m_cmpnt.radius(); }

	//added by noma@tome 20120223
	/**
	 * @brief Get radius of circum sphere
	 */
	double getCircumRadius(void);
	/**
	 * @brief Calculate cube root of the volume
	 */
	double getCubicRootOfVolume(void);

	/**
	 * @brief Get radius of circum sphere
	 * @param r radius
	 */
	void setRadius(double r) { m_cmpnt.radius(r); }


private:
	// SParts implementation
	void set(dWorldID w, dSpaceID s);

	// Parts implementation
	PartsCmpnt * extdata() { return &m_cmpnt; }
};


/**
 * @brief Class for invisible parts in Server side
 */
class SBlindParts : public SParts
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param name parts name
	 * @param pos  center position of parts
	 */
	SBlindParts(const char *name, const Position &pos);

	//added by noma@tome 20120223
	/**
	 * @brief Get radius of circum sphere
	 */
	double getCircumRadius(void);
	/**
	 * @brief Calculate cube root of the volume
	 */
	double getCubicRootOfVolume(void);

private:
	// SParts implementation
	void set(dWorldID w, dSpaceID s);
	// Parts implementation
	PartsCmpnt * extdata() { return NULL; }
};

#endif

#endif // Parts_Server_h


