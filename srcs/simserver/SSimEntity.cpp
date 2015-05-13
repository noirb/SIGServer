/*
 * Modified by Tetsunari Inamura on 2014-02-25
 *    Remove Japanese comments
 */

#include "SSimEntity.h"
#include "Logger.h"

//! add geometry
void SSimEntity::addGeom(dGeomID geom)
{
	LOG_MSG(("addGeomAndBody! (g,b) = (%d, %d)",geom, m_parts.body))
		dGeomSetBody(geom, m_parts.body);
	m_parts.geoms.push_back(geom);
}


bool SSimEntity::getPosition(Vector3d &pos, bool pre)
{
	const dReal *p = dBodyGetPosition(m_parts.body);

	// Get the gap of position between SIGVerse and ODE
	Vector3d opos = m_parts.pos;
	pos.set(p[0], p[1], p[2]);

	// return false if the position is not moved
	if (m_px == p[0] && m_py == p[1] && m_pz == p[2]) {
		return false;
	}
	// if it is moved
	else{
		// Position for motion detection is reset
		if (pre) {
			m_px = p[0];
			m_py = p[1];
			m_pz = p[2];
		}
		return true;
	}
}

bool SSimEntity::getQuaternion(dQuaternion q, bool pre)
{ 
	int geomNum = getGeomNum();

	// All of the quaternion connected by fixed joint should be the same
	dReal qu[] = {0.0, 0.0, 0.0, 0.0}; 
	const dReal *tmp_qu = dBodyGetQuaternion(m_parts.body);

	q[0] = tmp_qu[0];
	q[1] = tmp_qu[1];
	q[2] = tmp_qu[2];
	q[3] = tmp_qu[3];

	// return false if it is not rotated
	if (m_qw == q[0] && m_qx == q[1] && m_qy == q[2] && m_qz == q[3]){
		return false;
	}
	// if it is rotated
	else{
		return true;
	}
}


void SSimEntity::setInitPosition(Vector3d pos)
{
	m_inipos.set(pos.x(), pos.y(), pos.z());
	// If a part is already added
	dBodyID body = m_parts.body;
	dBodySetPosition(body, pos.x(), pos.y(), pos.z());
}


//! add ODE parts with fixed joint
void SSimEntity::setMass(double mass)
{
	m_parts.mass = mass;
	int geomNum = getGeomNum();

	if (geomNum != 0) {
		// if a part is already added
		// mass per part
		double ms = mass / geomNum;

		// refer geometry and body
		dBodyID body = m_parts.body;

		dMass m;
		dMass m2;
		dMassSetZero(&m); dMassSetZero(&m2);

		for (int i = 0; i < geomNum; i++) {

			// refer the type of geometory
			dGeomID geom = dGeomTransformGetGeom(m_parts.geoms[i]);
			int type = dGeomGetClass(geom);

			// setting of mass
			// sphere
			if (type == 0) {
				dReal radius = dGeomSphereGetRadius(geom);
				dMassSetSphereTotal(&m2, ms, radius);
			}

			// box
			else if (type == 1) {
				dVector3 size;
				dGeomBoxGetLengths(geom, size);
				dMassSetBoxTotal(&m2, ms, size[0], size[1], size[2]);
			}

			// cylinder
			else if (type == 3) {
				dReal radius = 0.0;
				dReal length = 0.0;
				dGeomCylinderGetParams(geom, &radius, &length);
				// TODO: confirm: Is 2 suitable for long axis?
				dMassSetCylinderTotal(&m2, ms, 2, radius, length);
			}
			const dReal *pos = dGeomGetPosition(geom);
			dMassTranslate(&m2, pos[0], pos[1], pos[2]);
			dMassAdd(&m, &m2);
		}

		/*
		  for (int i = 0; i < geomNum; i++) {
		    dGeomID geom = dGeomTransformGetGeom(m_parts.geoms[i]);
		    const dReal *pos = dGeomGetPosition(geom);      
			// Change to a relative position from CoG
		    dGeomSetPosition(geom, pos[0] - m.c[0], pos[1] - m.c[1], pos[2] - m.c[2]);
		  }
		  // keep the CoG position
		  //m_parts.pos.set(m.c[0], m.c[1], m.c[2]);
		  */

		// Adjustment of the gap from CoG
		//const dReal *p = dBodyGetPosition(body);
		//dBodySetPosition(body,p[0]+m.c[0], p[1]+m.c[1], p[2]+m.c[2]);    
		dMassTranslate (&m,-m.c[0],-m.c[1],-m.c[2]);

		// Setting of mass
		dBodySetMass(body, &m);
		dBodySetDamping(body, 0.8, 0.8); // added by inamura on 2014-01-29 for test
	} // if (partsNum != 0) {
}


//! Add ODE parts with fixed joint
void SSimEntity::setCollision(bool col)
{
	if (col)m_collision = true;
	else m_collision = false;

	// if a part is already added
	int geomNum = getGeomNum();
	if (geomNum != 0) {
		for (int i = 0; i < geomNum; i++) {
			// refer the geometry
			dGeomID geom = m_parts.geoms[i];
			if (col) {
				dGeomEnable(geom);
			}
			else{
				dGeomDisable(geom);
			}
		}
	}
}

//! setting of scale
void SSimEntity::setScale(Vector3d scale) {
	m_scale.set(scale.x(), scale.y(), scale.z());

	// if a part is already added
	int geomNum = getGeomNum();
	if (geomNum != 0) {

		dBodyID body = m_parts.body;
		// loop for all of the parts
		for (int i = 0; i < geomNum; i++) {
			// refer the geometry
			dGeomID geom = m_parts.geoms[i];

			// refer the position (gap from CoG)
			const dReal *pos = dGeomGetPosition(geom);

			// Reflection of scale
			dGeomSetPosition(geom, pos[0]*scale.x(), pos[1]*scale.y(), pos[2]*scale.z());

			// Refer the type of the geometory
			int type = dGeomGetClass(geom);

			// setting of mass
			// sphere
			if (type == 0) {

				// average of scale
				double mean = (scale.x() + scale.y() + scale.z())/ 3;

				// refer the radius
				dReal radius = dGeomSphereGetRadius(geom);

				// reflection of scale
				dGeomSphereSetRadius(geom, radius*mean);
			}

			// box
			else if (type == 1) {
				// refer the size
				dVector3 size;
				dGeomBoxGetLengths(geom, size);

				// reflection of scale
				dGeomBoxSetLengths(geom, size[0]*scale.x(), size[1]*scale.y(), size[2]*scale.z());
			}

			// cylinder
			else if (type == 3) {
				dReal radius, length;
				dGeomCylinderGetParams(geom, &radius, &length);

				// average of scale in horizontal plane
				double mean = (scale.x() + scale.z()) / 2;
	
				// TODO: confirm: is 2 suitable for long axis?
				dGeomCylinderSetParams(geom, radius*mean, length*scale.y());
			}
		} //  for (int i = 0; i < partsNum; i++) {
	}
}


void SSimRobotEntity::addJoint(SSimJoint *joint)
{
	// if a case that does not have geommetry
	if (!joint->has_geom) {

		// create dummy body
		joint->robotParts.objParts.body = dBodyCreate(m_world);
		dBodyDisable(joint->robotParts.objParts.body);

		/*
		  dGeomID geom = dCreateSphere(m_space, 0.1);
		  dGeomSetBody(geom, joint->robotParts.objParts.body);
		  dGeomDisable(geom);

		  dMass mass;
		  dMassSetZero(&mass);
		  dMassSetSphereTotal(&mass, 0.0000001, 0.0000001);

		  dBodySetMass(joint->robotParts.objParts.body, &mass);
		*/
		//LOG_MSG(("dummy body %d",joint->robotParts.objParts.body));
	}
	// setting mass
	else{
		double mass = joint->robotParts.objParts.mass; 
		this->setMass(&(joint->robotParts.objParts), mass);

		// add robot parts which have geometry to member variables
		m_allParts.push_back(joint->robotParts);
	}

	// add joint to member variables
	m_allJoints.push_back(joint);

	// refer the joint ID and body ID
	dJointID myJoint = joint->joint;
	dBodyID  cbody   = joint->robotParts.objParts.body;

	if (joint->isRoot) {
		// connect with root body
		//dJointAttach(myJoint, cbody, m_rootBody);  //TODO!
		return;
	}

	// refer the name of parent joint
	std::string parent = joint->parent_joint;

	// refer the parent joint structure from the joint name
	//SSimJoint pjoint;
	SSimJoint *pjoint = getJoint(parent);
	//LOG_MSG(("parent %s %s",parent.c_str(), pjoint.name.c_str()));

	// connect with parent body
	dBodyID pbody = pjoint->robotParts.objParts.body;

	//dJointAttach(myJoint, pbody, cbody);
	dJointAttach(myJoint, cbody, pbody);
	int type = dJointGetType(myJoint);
	if (type == dJointTypeHinge) {
		dJointSetHingeParam(myJoint, dParamLoStop, -2.0*M_PI);
		dJointSetHingeParam(myJoint, dParamHiStop,  2.0*M_PI);
	}
	LOG_MSG(("joint(%d) attach body(%d, %d)",myJoint, cbody, pbody));

	//dGeomID geom1 = pjoint->robotParts.objParts.mass;  
	//dGeomID geom2 = joint->robotParts.objParts.mass;  

	//LOG_MSG(("geom (%d, %d)", geom1, geom2));
	/*
	// if geommetry exists
	if (joint.has_geom) {

	/*
	////// search the parent part to be connected
	// refer the parent joint
	std::string parent = joint.parent_joint;

	while (1) {

	// get joint structure from joint name
	SSimJoint pjoint = getJoint(parent);

	// check whether geometry to be connected exist?
	if (pjoint.has_geom) {
	
	// refer a body to be connected
	dBodyID pbody = pjoint.robotParts.objParts.body;
	dJointAttach(myJoint, cbody, pbody);
	break;
	}

	// there is no parent any more
	if (pjoint.isRoot) break;
	else{
	// Search the parents in addition
	parent = pjoint.parent_name;
	}
	}
	 */
}


void SSimRobotEntity::setInitPosition(Vector3d pos)
{
	int jsize = m_allJoints.size();

	for (int i = 0; i < jsize; i++) {

		SSimJoint *sjoint = m_allJoints[i];
		SSimRobotParts rparts = sjoint->robotParts;
		//dBodyID  body  = m_allJoints[i]->robotParts.objParts.body;
		dBodyID  body  = rparts.objParts.body;
		dJointID joint = sjoint->joint;
		//dGeomID geom = rparts.objParts.geoms[0];    

		if (i == 0) {
			dBodySetPosition(body, pos.x(), pos.y(), pos.z());
			//dGeomSetPosition(geom,pos.x(),pos.y(),pos.z());

			//const dReal *gpos = dGeomGetPosition(geom);
			//LOG_MSG(("root body pos = (%f, %f, %f)", pos.x(), pos.y(), pos.z()));
			//LOG_MSG(("root geom pos = (%f, %f, %f)", gpos[0], gpos[1], gpos[2]));
		}
		else {
			// TODO: deal with a case in non-hinge joint

			// gap between joint from root joint
			dReal trans_x = sjoint->posFromRoot.x();
			dReal trans_y = sjoint->posFromRoot.y();
			dReal trans_z = sjoint->posFromRoot.z();

			//LOG_MSG(("zure (%f, %f, %f)", trans_x, trans_y, trans_z));
			//LOG_MSG(("zure of body(%f, %f, %f)", rparts.com.x(), rparts.com.y(), rparts.com.z()));

			dBodySetPosition(body,
							 pos.x()+trans_x+rparts.com.x(),
							 pos.y()+trans_y+rparts.com.y(),
							 pos.z()+trans_z+rparts.com.z());

			//const dReal *gpos = dGeomGetPosition(geom);
			LOG_MSG(("body(id:%d) pos = (%f, %f, %f)",body, pos.x()+trans_x+rparts.com.x(), pos.y()+trans_y+rparts.com.y(), pos.z()+trans_z+rparts.com.z()));
			//LOG_MSG(("geom pos = (%f, %f, %f)", gpos[0], gpos[1], gpos[2]));

			// setting of joint position
			int type = dJointGetType(joint);
			if (type == dJointTypeHinge) {
				dJointSetHingeAnchor(joint,
									 pos.x()+trans_x,
									 pos.y()+trans_y,
									 pos.z()+trans_z);
			}
			LOG_MSG(("joint(%d) pos = (%f, %f, %f)",joint, pos.x()+trans_x, pos.y()+trans_y, pos.z()+trans_z));

			/*
			  dGeomSetPosition(geom,
			  pos.x()+trans_x+rparts.com.x(),
			  pos.y()+trans_y+rparts.com.y(),
			  pos.z()+trans_z+rparts.com.z());
			*/
		}
	}
}


//! setting of mass
void SSimRobotEntity::setMass(SSimObjParts *parts, double mass)
{
	// if a part is already added
	int geomNum = parts->geoms.size();
	if (geomNum != 0) {
		// mass per each part
		double ms = mass / geomNum;

		// refer geometry and body
		dBodyID body = parts->body;

		dMass m;
		dMass m2;
		dMassSetZero(&m);
		dMassSetZero(&m2);

		for (int i = 0; i < geomNum; i++) {

			// Refer the type of geometry
			dGeomID geom = dGeomTransformGetGeom(parts->geoms[i]);
			int type = dGeomGetClass(geom);

			// setting of mass
			// sphere
			if (type == 0) {
				dReal radius = dGeomSphereGetRadius(geom);
				dMassSetSphereTotal(&m2, ms, radius);
			}

			// box
			else if (type == 1) {
				dVector3 size;
				dGeomBoxGetLengths(geom, size);
				dMassSetBoxTotal(&m2, ms, size[0], size[1], size[2]);
			}

			// cylinder
			else if (type == 3) {
				dReal radius = 0.0;
				dReal length = 0.0;
				dGeomCylinderGetParams(geom, &radius, &length);
				// TODO: confirm: Is 2 suitable for long axis?
				dMassSetCylinderTotal(&m2, ms, 2, radius, length);
			}
			// 
			const dReal *pos = dGeomGetPosition(geom);
			//LOG_MSG(("pos = (%f, %f, %f)", pos[0], pos[1], pos[2]));
			dMassTranslate(&m2, pos[0], pos[1], pos[2]);
			dMassAdd(&m, &m2);
		}

		// adjustment of the gap between CoG
		//const dReal *p = dBodyGetPosition(body);
		//dBodySetPosition(body,p[0]+m.c[0], p[1]+m.c[1], p[2]+m.c[2]);    
		dMassTranslate (&m,-m.c[0],-m.c[1],-m.c[2]);

		// seeting of mass
		dBodySetMass(body, &m);
		dBodySetDamping(body, 0.8, 0.8); // added by inamura on 2014-01-29 for test
	} // if (partsNum != 0) {
}


//! Setting of flag for collision detection
void SSimRobotEntity::setCollision(bool col) {

	if (col)m_collision = true;
	else m_collision = false;

	// Setting all of the parts
	int pSize = m_allParts.size();

	for (int i = 0; i < pSize; i++) {

		int gSize = m_allParts[i].objParts.geoms.size();

		for (int j = 0; j < gSize; j++) {
			// reference of the geometry
			dGeomID geom = m_allParts[i].objParts.geoms[j];
			if (col) {
				dGeomEnable(geom);
			}
			else {
				dGeomDisable(geom);
			}
		}
	}
}

/*
  void SSimRobotEntity::addParts(const char *name) {
  int pnum = getPartsNum();
  SSimRobotParts rparts;
  if (pnum == 0) {
  // There is no parent joint
  rparts.root = true;
  }  
  // Add the part which is now focused
  rparts.parts = m_parts;

  // Add name of the part
  rparts.parts_name = name;

  m_allParts.push_back(rparts);
  m_parts.geoms.clear();
  }


  void SSimRobotEntity::addParts(std::vector<SSimJoint> joint, const char *parent_name, const char *name) {
  int pnum = getPartsNum();
  SSimRobotParts rparts;

  if (pnum == 0) {
  // There is no parent joint
  return;
  }  

  // Add the part which is now focused
  rparts.parts = m_parts;

  m_allParts.push_back(rparts);
  m_parts.geoms.clear();
  }
*/
