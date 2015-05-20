/*
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add English comments (Translation from v2.2.0 is finished)
 * Modified by Tetsunari Inamura on 2013-12-30
 *   Change setVelocity to setLinearVelocity
 */

#include "systemdef.h"

#ifdef EXEC_SIMULATION

#include "SParts.h"
#include "ODEWorld.h"
#include "ODEObj.h"
#include "Joint.h"
#include "SSimObj.h"

#include <assert.h>

/**
 * @brief Constructor
 * @param t     parts type
 * @param name  parts name
 * @param pos   position (agent coordinate)
 */
SParts::SParts(PartsType t, const char *name, const Position &pos) : Parts(t, name, pos),
               m_odeobj(NULL),
               m_parent(NULL),
               m_parentJoint(NULL),
               m_posx(0),
               m_posy(0),
               m_posz(0),
               m_mass(1.0),
               m_onGrasp(false),
               m_onCollision(false)
{
}


SParts::~SParts()
{
	for(ChildC::iterator i=m_children.begin(); i!=m_children.end(); i++) {
		Child *child = *i;
		delete child;
	}
	m_children.clear();
};


bool SParts::sameParent(const SParts &o)
{
	assert(o.m_parent);
	assert(this->m_parent);
	return o.m_parent == this->m_parent? true: false;
}

bool SParts::removeChild(Joint *nextj)
{
	ChildC::iterator i;
	for (i= m_children.begin(); i!=m_children.end(); i++) {
		Child *p = *i;
		if (p->nextj == nextj) {
			break;
		}
	}
	if (i == m_children.end()) { return false; }
	Child *target = *i;
	m_children.erase(i);
	delete target;
	return true;
}

const dReal * SParts::getPosition()
{
  const dReal *pos = dBodyGetPosition(m_odeobj->body());
  return pos;
}

const dReal * SParts::getRotation()
{
	return dBodyGetRotation(m_odeobj->body());
}

const dReal * SParts::getQuaternion()
{
	return dBodyGetQuaternion(m_odeobj->body());
}

const dReal * SParts::getForce()
{
	return dBodyGetForce(m_odeobj->body());
}

const dReal * SParts::getTorque()
{
	return dBodyGetTorque(m_odeobj->body());
}

void SParts::setPosition(const Vector3d &v)
{
  dBodySetPosition(m_odeobj->body(), v.x(), v.y(), v.z());
}


void SParts::setInitialQuaternion(dReal q0, dReal q1, dReal q2, dReal q3)
{
	m_rot.setQuaternion(q0, q1, q2, q3);
}

void SParts::setQuaternion(dReal q0, dReal q1, dReal q2, dReal q3)
{
	if (m_odeobj) {
		dReal q[] = { q0, q1, q2, q3, };
		dBodySetQuaternion(m_odeobj->body(), q);
	}
}

void SParts::enableDynamics(bool b)
{
	if (m_odeobj) {
		dBodyID body = m_odeobj->body();
		if (b) {
			dBodyEnable(body);
		} else {
			dBodyDisable(body);
		}
	}
}

void SParts::build(const Vector3d &ov, const Rotation &r, bool dynamics)
{
	if (isBody()) {
		const dReal *q = r.q();
		setPosition(ov);
		setQuaternion(q);
	}
	else {
		Vector3d v(m_pos.x(), m_pos.y(), m_pos.z());
		v.rotate(r);
		v += ov;
		setPosition(v);

		Rotation rr(r);
		rr *= m_rot;

		setRotation(rr);
	}

	enableDynamics(dynamics);
}

#if 1
#define DUMP(MSG)
#else
#define DUMP(MSG) printf MSG;
#endif

void SParts::calcPosition(Joint *currj, Joint *nextj, const Vector3d &anchorv, const Rotation &R)
{
	if (currj) {
		DUMP(("currj  = %s\n", currj->name()));
	}
	DUMP(("name = %s\n", name()));

	if (nextj) {
		DUMP(("nextj  = %s\n", nextj->name()));
	}
	DUMP(("anchorv = (%f, %f, %f)\n", anchorv.x(), anchorv.y(), anchorv.z()));

	{
		Vector3d v(m_pos.x(), m_pos.y(), m_pos.z());

		// Calculate shift vector from positoin/anchor/orientation
		if (currj) {
			v -= currj->getAnchor();
			DUMP(("parent anchor = %s (%f, %f, %f)\n",
				  currj->name(),
				  currj->getAnchor().x(),
				  currj->getAnchor().y(), currj->getAnchor().z()));
		}

		DUMP(("v1 = (%f, %f, %f)\n", v.x(), v.y(), v.z()));
		Rotation rr(R);
		if (currj) {
			rr *= currj->getRotation();
		}

		v.rotate(rr);
		DUMP(("v2 = (%f, %f, %f)\n", v.x(), v.y(), v.z()));

		v += anchorv;
		DUMP(("v3 = (%f, %f, %f)\n", v.x(), v.y(), v.z()));


		//DUMP(("v4 = (%f, %f, %f)\n", v.x(), v.y(), v.z()));
		setPosition(v);

		if(m_onGrasp) {

			dBodyID body = odeobj().body();
			//int num = dBodyGetNumJoints(body);

			// get grasp joint
			dJointID joint = dBodyGetJoint(body, 0);

			// get target body from joint
			dBodyID targetBody = dJointGetBody(joint, 1);
		  
			// get position of grasp target and parts
			const dReal *pos  = dBodyGetPosition(body);
			const dReal *tpos = dBodyGetPosition(targetBody);

			// Set if it is moved
			if(pos[0] != tpos[0] || pos[1] != tpos[1] || pos[2] != tpos[2]) {
				dBodySetPosition(targetBody, pos[0], pos[1], pos[2]);
			}

			// get position of grasp target and parts
			const dReal *qua  = dBodyGetQuaternion(body);
			const dReal *tqua = dBodyGetQuaternion(targetBody);

			// rotation from initial orientation
			dQuaternion rot;
			dQMultiply2(rot, qua, m_gini);

			// Set if it is rotated
			if(rot[0] != tqua[0] || rot[1] != tqua[1] || rot[2] != tqua[2] || rot[3] != tqua[3]) {
				dBodySetQuaternion(targetBody, rot);
			}
			//LOG_MSG(("target2 %d", targetBody));
			//LOG_MSG(("(%f, %f, %f)", tpos[0], tpos[1], tpos[2]));
			//LOG_MSG(("onGrasp!! num = %d, joint = %d", num));
		}
	}
	Rotation rr(R);
	rr *= m_rot;
	if (currj) {
		rr *= currj->getRotation();
	}
	setRotation(rr);

	if (!nextj) { return; }

	for (ChildC::iterator i=m_children.begin(); i!=m_children.end(); i++) {
		Child *child = *i;

		Rotation R_(R);
		if (currj) {
			R_ *= currj->getRotation();
		}

		Vector3d nextv;

		nextv = nextj->getAnchor();
		if (currj) {
			nextv -= currj->getAnchor();
		}
		nextv.rotate(R_);
		nextv += anchorv;

		DUMP(("nextv1 : (%f, %f, %f)\n", nextv.x(), nextv.y(), nextv.z()));
		// act in a case that multiple JOINTs are connected to one link
		if(strcmp(nextj->name(),child->currj->name()) == 0) {
			child->nextp->calcPosition(child->currj, child->nextj, nextv, R_);
		}
	}
}

void SParts::calcPosture()
{
	assert(isBody());
	if (m_children.size() <= 0) { return; }

	const dReal *pos = getPosition();
	const dReal *q = getQuaternion();

	for (ChildC::iterator i=m_children.begin(); i!=m_children.end(); i++) {
		Child *child = *i;

		Vector3d v(pos[0], pos[1], pos[2]);

		Rotation r;
		r.setQuaternion(q);
		if (child->currj) {
			v += child->currj->getAnchor();
		}

		child->nextp->calcPosition(child->currj, child->nextj, v, r);
	}
}

void SParts::setForce(dReal fx, dReal fy, dReal fz)
{
	dBodySetForce(m_odeobj->body(), fx, fy, fz);
}

void SParts::addForce(dReal fx, dReal fy, dReal fz)
{
	dBodyAddForce(m_odeobj->body(), fx, fy, fz);
}

void SParts::addRelForce(dReal fx, dReal fy, dReal fz)
{
	dBodyAddRelForce(m_odeobj->body(), fx, fy, fz);
}

void SParts::addForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
	dBodyAddForceAtPos(m_odeobj->body(), fx, fy, fz, px, py, pz);
}

void SParts::addRelForceAtPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
	dBodyAddRelForceAtPos(m_odeobj->body(), fx, fy, fz, px, py, pz);
}

void SParts::addForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
	dBodyAddForceAtRelPos(m_odeobj->body(), fx, fy, fz, px, py, pz);
}

void SParts::addRelForceAtRelPos(dReal fx, dReal fy, dReal fz, dReal px, dReal py, dReal pz)
{
	dBodyAddRelForceAtRelPos(m_odeobj->body(), fx, fy, fz, px, py, pz);
}

void SParts::getAngularVelocity(Vector3d &v)
{
	const dReal *avel = dBodyGetAngularVel(m_odeobj->body());
	v.x(avel[0]);
	v.y(avel[1]);
	v.z(avel[2]);
}

// Changed from getVelocity: by inamura on 2013-12-30
void SParts::getLinearVelocity(Vector3d &v)
{
	const dReal *avel = dBodyGetLinearVel(m_odeobj->body());
	v.x(avel[0]);
	v.y(avel[1]);
	v.z(avel[2]);
}

void SParts::setGravityMode(bool gravity)
{
	dBodySetGravityMode(m_odeobj->body(), gravity);
}

void SParts::setCollisionEnable(bool gravity)
{
	if(gravity) 
		dGeomEnable(m_odeobj->geom());
	else
		dGeomDisable(m_odeobj->geom());
}

void SParts::setTorque(dReal tx, dReal ty, dReal tz)
{
	dBodySetTorque(m_odeobj->body(), tx, ty, tz);
}


/**
 * @brief Set linear velocity of the parts
 *
 * @param vx x element of velocity
 * @param vy y element of velocity
 * @param vz z element of velocity
 */
// changed from setVelocity: by inamura on 2013-12-30
void SParts::setLinearVelocity(dReal vx,dReal vy,dReal vz)
{
	// call API of the ODE
	dBodySetLinearVel(m_odeobj->body(),vx,vy,vz);
}

void SParts::addTorque(dReal fx, dReal fy, dReal fz)
{
	dBodyAddTorque(m_odeobj->body(), fx, fy, fz);
}


/**
 * Set maximum angular velocity
 */
//void SParts::setAngularMaxVelocity(dReal max){
	//dBodySetMaxAngularSpeed(m_odeobj->body(),max);
	//maxAngularVel = max;
//}


/**
 * @brief Set angular velocity of the parts
 *
 * @param x angular velocity around x axis
 * @param y angular velocity around y axis
 * @param z angular velocity around z axis
 */
void SParts::setAngularVelocity(dReal x,dReal y,dReal z)
{
	//TODO: using maxAngularVel
	// call API of the ODE
	dBodySetAngularVel(m_odeobj->body(),x,y,z);
	const dReal *dat = dBodyGetAngularVel(m_odeobj->body());
	//TODO: this function does not set the value: by inamura on 2013-12-29
}


double SParts::getMass()
{
	dMass m;
	dBodyGetMass(m_odeobj->body(), &m);
	return m.mass;
}

void SParts::setMass(double mass)
{
	m_mass = mass;
	if(m_odeobj != NULL) {
		dMass m;
		dBodyGetMass(m_odeobj->body(), &m);
		dMassAdjust(&m, m_mass);
		dBodyID body = m_odeobj->body();
		dBodySetMass(body, &m);
	}
}


void SParts::graspObj(std::string objName)
{
	m_graspObj = objName;
	m_onGrasp = true;

	// keep the orientation when the grasp is started
	const dReal *qua = dBodyGetQuaternion(m_odeobj->body());
	m_gini[0] = qua[0];
	m_gini[1] = qua[1];
	m_gini[2] = qua[2];
	m_gini[3] = qua[3];
}


#if 1
void SParts::dump()
{
	char *typestrs[] = { (char *)"box", (char *)"cylinder", (char *)"sphere", };

	// Parts ID, name, type
	printf("\tparts(%d): %s(%s)\n", id(), name(), typestrs[m_type]);

	// position
	printf("\t\tpos(%f, %f, %f)\n", m_pos.x(), m_pos.y(), m_pos.z());

	// size
	PartsCmpnt *c = extdata();
	if (c) {
		c->dump();
	}

	// Position and orientation if ODEobj is existed
	if (m_odeobj) {
		const dReal *odePos = getPosition();
		const dReal *odeQ   = getQuaternion();

		printf("\t\todePos(%f, %f, %f)\n",   odePos[0], odePos[1], odePos[2]);
		printf("\t\todeQ(%f, %f, %f, %f)\n", odeQ[0], odeQ[1], odeQ[2], odeQ[3]);
	}
}


void SParts::dumpConnectionInfo(int level)
{
	char s[256]; //TODO : Magic number

	printIndent(level);
	printf("*** %s ***\n", m_name.c_str());

	strcpy(s, m_parentJoint ? m_parentJoint->name() : "NULL");
	printIndent(level+1);
	printf("parentJoint=(%s)\n", s);

	ChildC::iterator i;
	for (i=m_children.begin(); i!=m_children.end(); i++)
	{
		Child *c = *i;
		if (c)
		{
			Joint *childJ = c->currj;
			strcpy(s, childJ ? childJ->name() : "NULL");
			printIndent(level+1);
			printf("childJ=(%s)\n", s);

			Joint *nextJ = c->nextj;
			strcpy(s, nextJ ? nextJ->name() : "NULL");
			printIndent(level+1);
			printf("nextJ=(%s)\n", s);

			SParts *childP = c->nextp;
			strcpy(s, childP ? childP->name() : "NULL");
			printIndent(level+1);
			printf("childP=(%s)\n", s);

			childP->dumpConnectionInfo(level+2);
		}
	}
}

void SParts::dumpODEInfo(int level)
{
	printIndent(level);
	printf("*** %s ***\n", m_name.c_str());

	printIndent(level+1);
	printf("m_odeobj=(0x%0p)\n", m_odeobj);
	if (!m_odeobj) printf("+++++ ERROR! m_odeobj=NULL! +++++\n");

	const dReal *pos = getPosition();
	printIndent(level+1);
	printf("pos=(%f, %f, %f)\n", pos[0], pos[1], pos[2]);

	const dReal *q = getQuaternion();
	printIndent(level+1);
	printf("q=(%f, %f, %f, %f)\n", q[0], q[1], q[2], q[3]);

	ChildC::iterator i;
	for (i=m_children.begin(); i!=m_children.end(); i++) {
		Child *c = *i;
		if (c) {
			SParts *childP = c->nextp;
			if (childP) childP->dumpODEInfo(level+2);
		}
	}
}

void SParts::printIndent(int level)
{
	for (int i=0; i<level; i++) printf("  ");
}
#endif

#define DENSITY 0.05

void SBoxParts::set(dWorldID w, dSpaceID space)
{
	Size &sz = m_cmpnt.size();
	/*
	const dReal hx = sz.x();
	const dReal hy = sz.y();
	const dReal hz = sz.z();
	*/
	dReal hx = sz.x();
	dReal hy = sz.y();
	dReal hz = sz.z();

// konao
DUMP(("[SBoxParts::set] ODE geom created (hx, hy, hz)=(%f, %f, %f) [%s:%d]\n", hx, hy, hz, __FILE__, __LINE__));

	if(hz == 0) hz = 0.001;
	if(hy == 0) hy = 0.001;
	if(hx == 0) hx = 0.001;

	dGeomID geom = dCreateBox(0, hx, hy, hz);
	m_odeobj = ODEObjectContainer::getInstance()->createODEObj
	(
		w,
		geom,
		0.9,
		0.01,
		0.5,
		0.5,
		0.8,
		0.001,
		0.0
	);

	dBodyID body = m_odeobj->body();
	dMass m;
	dMassSetZero(&m);

	// x-axis and z-axis is swapped between ODE/SIGVerse
	dMassSetBox(&m, DENSITY, hz, hy, hx); //TODO: mass of cube should be configurable
	dMassAdjust(&m, m_mass);
	dBodySetMass(body, &m);

	// Gap between ODE shape and body
	dGeomSetOffsetPosition(geom, m_posx, m_posy, m_posz);

	// Initial orientation
	dReal offq[4] = {m_inirot.qw(), m_inirot.qx(), m_inirot.qy(), m_inirot.qz()};
	dGeomSetOffsetQuaternion(geom, offq);
	//dMassAdjust(&m, 1.0);

	m_rot.setQuaternion(1.0, 0.0, 0.0, 0.0);

	dSpaceAdd(space, geom);

	dBodySetData(body, this);
}

//added by noma@tome 20120223
double SBoxParts::getCircumRadius(void)
{
	Size& tmpSize = m_cmpnt.size();
	return sqrt(tmpSize.x()*tmpSize.x()+tmpSize.y()*tmpSize.y()+tmpSize.z()*tmpSize.z())/2;
}


//added by noma@tome 20120223
double SBoxParts::getCubicRootOfVolume(void)
{
	Size& tmpSize = m_cmpnt.size();
	return pow(tmpSize.x()*tmpSize.y()*tmpSize.z(), 1.0/3.0);
}



//	added by kawamoto@tome (2011/04/04)
/**
 * @brief Initialization of orientation
 * @param x orientation around x axis
 * @param y orientation around y axis
 * @param z orientation around z axis
 */
void SCylinderParts::initializeAngle(double x,double y,double z)
{
	// radius of sphere which touches the point (x,y,z)
	double radius = sqrt((x*x + y*y + z*z));

	// the angle made by x and y 
	// double theta = atan2(y,x); // not used, deleted by inamura
	double k = sqrt((radius * radius - z * z));
	// the angle made by z and xy-plane
	double phi = atan2(z,k);
	// cross product of m and r, m=(x,y,0) and r=(x,y,z)
	double lx = y * z;
	double ly = -x * z;
	double lz = 0.0;

	// angle to be rotated
	double angle = M_PI / 2.0 - phi;

	// Rotate -90[deg] around y axis if the (x,y,z) is on x axis
	if(fabs(y) < 0.01 && fabs(z) < 0.01)
	{
		lx = 0.0;
		ly = -1.0;
		lz = 0.0;
	}
	// Rotate -90[deg] around x axis if the (x,y,z) is on y axis
	if(fabs(x) < 0.01 && fabs(z) < 0.01)
	{
		lx = 1.0;
		ly = 0.0;
		lz = 0.0;
	}
	// Do not rotate if the (x,y,z) is on z axis
	else if(fabs(x) < 0.01 && fabs(y) < 0.01)
	{
		lx = 0.0;
		ly = 0.0;
		lz = 1.0;
		angle = 0.0;
	}
	// set normalized value
	double vectorLength = sqrt( (lx*lx + ly*ly + lz*lz) );
	x_axis = lx / vectorLength;
	y_axis = ly / vectorLength;
	z_axis = lz / vectorLength;
	angleData = angle;
}


void SCylinderParts::set(dWorldID w, dSpaceID space)
{
	double radius = m_cmpnt.radius();
	double length = m_cmpnt.length();

// konao
	//LOG_MSG(("[SCylinderParts::set] ODE geom created (r, l)=(%f, %f) [%s:%d]\n", radius, length, __FILE__, __LINE__))
	// TODO: Ideally, cylinder should be constructed here. However, collision detection
	// between two cylinders could not be realized. So, Capsule is required
	// by okamoto@tome on 2011-10-12

	//dGeomID geom = dCreateCapsule(0, radius, length);
	dGeomID geom = dCreateCylinder(0, radius, length);

	m_odeobj = ODEObjectContainer::getInstance()->createODEObj
	(
		w,
		geom,
		0.9,
		0.01,
		0.5,
		0.5,
		0.8,
		0.001,
		0.0
	);


	dBodyID body = m_odeobj->body();
	dMass m;
	dMassSetZero(&m);
	//dMassSetCapsule(&m, DENSITY, 1, radius, length);
	dMassSetCylinder(&m, DENSITY, 1, radius, length); //TODO: mass of the cylinder should be configurable

	dMassAdjust(&m, m_mass);
	dBodySetMass(body, &m);
	dGeomSetOffsetPosition(geom, m_posx, m_posy, m_posz); // gap between ODE shape and body

	// set the long axis as y axis
	dReal offq[4] = {0.707, 0.707, 0.0, 0.0};
	dReal offq2[4] = {m_inirot.qw(), m_inirot.qx(), m_inirot.qy(), m_inirot.qz()};

	dQuaternion qua;
	dQMultiply2(qua, offq2, offq);
	dGeomSetOffsetQuaternion(geom, qua);
	//dGeomSetOffsetQuaternion(geom, offq2);

	//dReal tmpq[4] = {0.707, 0.0, 0.0, 0.707};
	//dGeomSetQuaternion(geom, tmpq);
	//dBodySetQuaternion(body, tmpq);
	
	/*TODO: Consideration is required whether this procedure is needed
	 * Reflection of orientation of the cylinder
	 * dMatrix3 R;
	 * dRFromAxisAndAngle(dMatrix3 R, dReal rx, dReal ry, dReal rz,  dReal angle)
	 * dRFromAxisAndAngle(R,x_axis,y_axis,z_axis,angleData);
	 * dBodySetRotation(body,R);  // Request of actual rotation
	*/

	// Not used, deleted by inamura
	// real part of the quaternion
	// double q = cos(angleData/2.0);
	// imaginary part of the quaternion
	// double i,j,k;
	// i = x_axis * sin(angleData/2.0);
	// j = y_axis * sin(angleData/2.0);
	// k = z_axis * sin(angleData/2.0);

	m_rot.setQuaternion(1.0, 0.0, 0.0, 0.0);

	dSpaceAdd(space, geom);
	dBodySetData(body, this);
}


//added by noma@tome 20120223
double SCylinderParts::getCircumRadius(void)
{
	return sqrt(m_cmpnt.radius()*m_cmpnt.radius() +m_cmpnt.length()*m_cmpnt.length());
}

//added by noma@tome 20120223
double SCylinderParts::getCubicRootOfVolume(void)
{
	//const double pi = 3.14159265358979323846; //TODO: why M_PI is not used?
	double volume = M_PI * m_cmpnt.radius()*m_cmpnt.radius() * m_cmpnt.length();
	return pow(volume, 1/3.0);
}



void SSphereParts::set(dWorldID w, dSpaceID space)
{
	double rad = m_cmpnt.radius();

	// konao
	DUMP(("[SSphereParts::set] ODE geom created (r=%f) [%s:%d]\n", rad, __FILE__, __LINE__));

	dGeomID geom = dCreateSphere(0, rad);

	m_odeobj = ODEObjectContainer::getInstance()->createODEObj
	(
		w,
		geom,
		0.9,
		0.01,
		0.5,
		0.5,
		0.8,
		0.001,
		0.0
	);

	dBodyID body = m_odeobj->body();
	dMass m;
	dMassSetZero(&m);
	dMassSetSphere(&m, DENSITY, rad);
	//dMassAdjust(&m, 1.0);
	dMassAdjust(&m, m_mass);
	dBodySetMass(body, &m);
	dGeomSetOffsetPosition(geom, m_posx, m_posy, m_posz); // gap between ODE shape and body

	m_rot.setQuaternion(1.0, 0.0, 0.0, 0.0);

	dSpaceAdd(space, geom);

	dBodySetData(body, this);
}

//added by noma@tome 20120223
double SSphereParts::getCircumRadius(void)
{
	return m_cmpnt.radius();
}


//added by noma@tome 20120223
double SSphereParts::getCubicRootOfVolume(void)
{
	//const double pi = 3.14159265358979323846; //TODO: why M_PI is not used?
	double tmpFactor = 4 * M_PI/3.0;
	return pow(tmpFactor, 1/3.0) * m_cmpnt.radius();
}


SBlindParts::SBlindParts(const char *name, const Position &pos)
	: SParts(PARTS_TYPE_SPHERE, name, pos)
{
	setBlind(true);
}


/*
 * Create microscopic sphere as a dummy parts
 * Error will arise if the radius is too small
 * Do not set mass
 * Do not detect collision between other parts
 */
void SBlindParts::set(dWorldID w, dSpaceID space)
{
	double rad = 1.0;
	dGeomID geom = dCreateSphere(0, rad);

	m_odeobj = ODEObjectContainer::getInstance()->createODEObj
	(
		w,
		geom,
		0.9,
		0.01,
		0.5,
		0.5,
		0.8,
		0.001,
		0.0
	);

	dBodyID body = m_odeobj->body();

	/*
	dMass m;
	dMassSetZero(&m);
	dMassSetSphere(&m, 0.0001, rad);
	dBodySetMass(body, &m);
	*/

	m_rot.setQuaternion(1.0, 0.0, 0.0, 0.0);

	//dSpaceAdd(space, geom);

	dBodySetData(body, this);
}


//added by noma@tome 20120223
double SBlindParts::getCircumRadius(void)
{
	return 0.0;
}

//added by noma@tome 20120223
double SBlindParts::getCubicRootOfVolume(void)
{
	return 0.0;
}

//	Check Joint Type
/*
void SParts::printJointDataList() {
	int length = getChild().size();

	for(int n=0;n<length;n++) {
		Child *child = m_children[n];
		int type = child->currj->type();
	}
}
*/

#endif
