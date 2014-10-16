/*
 * Modified by Tetsunari Inamura on 2014-01-07
 *    Translation of English comments are finished
 *    Reduction of warning
 */

#include "ShapeFileReader.h"
#include "CSimplifiedShape.h"
#include "SSimObj.h"
#include "Joint.h"
#include "ODEObj.h"
#include "SSimObjBuilder.h"
#include "ODEWorld.h"

#include <map>
#include <string>

ShapeFileReader::ShapeFileReader(SSimEntity *ent) : m_entity(ent)
{
	m_pParser = new CX3DParser();
	m_shapeCount = 0;
}

ShapeFileReader::ShapeFileReader(SSimRobotEntity *ent) : m_rentity(ent)
{
	m_pParser = new CX3DParser();
	m_shapeCount = 0;
}

bool ShapeFileReader::createRobotObj(CX3DParser *parser)
{
	m_isRobot = true;
	// Downcast to entity for robots
	m_rentity = (SSimRobotEntity*)m_entity;

	MFNode *humanoid;
	// Finding Humanoid body field
	humanoid = parser->searchNodesFromAllChildrenOfRoot("Humanoid");
	if (!humanoid) {
		humanoid = parser->searchNodesFromAllChildrenOfRoot("Robot");
	}
	if (humanoid) {
		// Get ODE world
		dWorldID world = m_rentity->getWorld();

		// Creation of root body
		dBodyID body = dBodyCreate(world);
		//m_rentity->setRootBody(body);
      
		// Assumption of only one (node?)
		CX3DOpenHRPHumanoidNode *hnode =(CX3DOpenHRPHumanoidNode*)humanoid->getNode(0);
		if (hnode) {
			// -----------------------------------------
			//   Extract a value in HumanoidBody field
			// -----------------------------------------
			MFNode *hbody = hnode->getHumanoidBody();

			// Get the number of nodes which are included in humanoidBody
				int nbody = hbody->count();
				for (int i = 0; i < nbody; i++) {
					// Extract i-th node
					CX3DNode *pNode = hbody->getNode(i);
					if (pNode) {
						switch(pNode->getNodeType())
							{
							// In case of Joint node
							case OPENHRP_JOINT_NODE:
								{
									// No translation and orientation
									Vector3d trans(0.0, 0.0, 0.0);
									CX3DOpenHRPJointNode *jNode = (CX3DOpenHRPJointNode *)pNode;
									createRobotJoint(jNode, trans, "RootJoint");
									break;
								}
							case BASE_NODE:
								{
									break;
								}
							default:
								break;
							}
					}
				}
			}
		}
	return true;
}


bool ShapeFileReader::createRobotJoint(CX3DOpenHRPJointNode *jNode, Vector3d trans, const char *pjoint)
{
	// Get ID number
	int id = jNode->getJointId()->getValue();

	// Transfer the ID to string
	char buf[5];
	sprintf(buf,"%d",id);

	// Creation of joint structure
	SSimJoint *joint = new SSimJoint();

	// Get joint name
	std::string jname = jNode->getName()->getValue();

	// Get translation
	SFVec3f *ctrans = jNode->getTranslation();

	// Get rotation --> not used now
	//SFRotation *crot = jNode->getRotation();
  
	// Displacement (relative position) from the parent joint
	Vector3d ptrans(ctrans->x(), ctrans->y(), ctrans->z());
  
	// Accumulation of the displacement
	Vector3d atrans(trans.x() + ctrans->x(), trans.y() + ctrans->y(), trans.z() + ctrans->z());

	// Set joint name
	joint->name = jname;
 
	// In case of root joint
	if (strcmp(pjoint, "RootJoint") == 0) {
		LOG_MSG(("##########ROOT! joint#########"));
		joint->isRoot = true;
	}
	else{
		LOG_MSG(("##########Else joint!#########"));
		joint->isRoot = false;
		// set parent joint name
		joint->parent_joint = pjoint;
	}
	// Set displacement from the parent joint
	joint->posFromParent = ptrans;
  
	// Set displacement from root joint
	joint->posFromRoot   = atrans;
  
	LOG_MSG(("posFromParent (%f, %f, %f)", ptrans.x(), ptrans.y(), ptrans.z())); 
	LOG_MSG(("posFromRoot (%f, %f, %f)", atrans.x(), atrans.y(), atrans.z())); 
  
	// Get ODE world
	dWorldID world = m_entity->getWorld();

	// get joint type
	SFString *jtype = jNode->getJointType();

	// HingeJoint
	if (strcmp(jtype->getValue(), "rotate") == 0) {

		// Creation of joint
		joint->joint = dJointCreateHinge(world, 0);
    
		// Central point
		dJointSetHingeAnchor(joint->joint, atrans.x(), atrans.y(), atrans.z());

		// Get rotational axis
		SFVec3f *axis = jNode->getJointAxis();

		// Set rotational axis
		dJointSetHingeAxis(joint->joint, axis->x(), axis->y(), axis->z());
		joint->iniAxis.set(axis->x(), axis->y(), axis->z());
		dJointSetHingeParam(joint->joint, dParamStopCFM, 0.0000000001);
		LOG_MSG(("joint:%d axis(%f, %f, %f)",joint->joint,axis->x(), axis->y(), axis->z()));
	}
  
	// HingeJoint
	if (strcmp(jtype->getValue(), "fixed") == 0) {
		// Creation of fixedjoint
		joint->joint = dJointCreateFixed(world, 0);
	}
	// tmp_joint.push_back(joint);
  
	// Get children field
	MFNode *children = jNode->getChildren();
  
	// The number of segments
	int segmentNum = 0;

	// 
	bool addedJoint = false;

	// Creation of ODE shape
	for (int i = 0; i < children->count(); i++) {
		// m_shapeCount = 0;
		// Get child node
		CX3DNode *childNode = children->getNode(i);
		switch(childNode->getNodeType())
			{
			case OPENHRP_JOINT_NODE:
				{
					if (!addedJoint) {
						// Add joint
						m_rentity->addJoint(joint);
						addedJoint = true;
					}
					CX3DOpenHRPJointNode *child = (CX3DOpenHRPJointNode *)childNode;
					createRobotJoint(child, atrans, jname.c_str());
					break;
				}
			case OPENHRP_SEGMENT_NODE:
				{
					// Initialization of parts
					m_entity->initObjParts();

					m_shapeCount = 0;
					CX3DOpenHRPSegmentNode *child = ( CX3DOpenHRPSegmentNode *)childNode;
	    
					createRobotSegment(child, atrans);
					segmentNum++;

					if (segmentNum > 1) 
						LOG_ERR(("cannot set two segment at joint"));
	    
					// Get and configure the created parts
					SSimObjParts parts = m_entity->getObjParts();

					// Set parts to joint
					joint->robotParts.objParts.mass = parts.mass;
					joint->robotParts.objParts.body = parts.body;
					LOG_MSG(("added body %d", parts.body));
					joint->robotParts.objParts.pos  = parts.pos;

					// Set displacement from the joint
					SFVec3f *com = child->getCenterOfMass();
					joint->robotParts.com.set(com->x(), com->y(), com->z());

					// link name
					joint->robotParts.name = child->getName()->getValue();

					// mass
					joint->robotParts.objParts.mass = child->getMass()->getValue();
					//LOG_MSG(("body = %d", parts.body));

					int psize = parts.geoms.size();
					for (int i = 0; i < psize; i++) {
						joint->robotParts.objParts.geoms.push_back(parts.geoms[i]);
						ODEObjectContainer::getInstance()->
							m_allEntities.insert(std::map<dGeomID,SSimRobotEntity*>::value_type(parts.geoms[i],m_rentity));
								      
						LOG_MSG(("added geom %d", parts.geoms[i]));
					}
	    
					joint->has_geom = true;

					// Is the max number of segment is 1?
					if (!addedJoint) {
						// Add joint
						m_rentity->addJoint(joint);
						addedJoint = true;
					}
					break;
				}
			default:
				{
					//CX3DParser::printLog("Could not create this node type.");
					break;
				}
			}
	}
  
	// A joint that doesn't have segment, also does not have geometory.
	//if (segmentNum == 0)
	//joint.has_geom = false;
	return true;
}

bool    ShapeFileReader::createRobotSegment(CX3DOpenHRPSegmentNode *sNode, Vector3d trans)
{
	SFVec3f *com = sNode->getCenterOfMass();
	Vector3d vec(com->x(), com->y(), com->z());

	// Accumulation of translation 
	Vector3d atrans(trans.x() + com->x(), trans.y() + com->y(), trans.z() + com->z());

	// link name
	std::string name = sNode->getName()->getValue();

	// Get children field
	MFNode *children = sNode->getChildren();

	COPFromMFNode(children);

	int shapeNum = 0;
	// Assumption that no mixture of shape node and Transform node
	// In the case of no Transform node is used, the number of shape nodes is required.
	for (int i = 0; i < children->count(); i++) {
		// get child node
		CX3DNode *childNode = children->getNode(i);
		int type = childNode->getNodeType();
		if (type == SHAPE_NODE) {
			shapeNum++;
		}
	}
  
	for (int i = 0; i < children->count(); i++) {
		// get child node
		CX3DNode *childNode = children->getNode(i);
		switch(childNode->getNodeType())
			{
			case TRANSFORM_NODE:
				{
					CX3DTransformNode *child = (CX3DTransformNode *)childNode;
					createTransform(child, atrans);
					break;
				}
			case SHAPE_NODE:
				{
					CX3DShapeNode *child = (CX3DShapeNode *)childNode;
					// No translation and rotation
					Vector3d trans(0.0, 0.0, 0.0);
					dReal rot[4] = {0.0, 0.0, 0.0, 0.0};
					createShape(child, shapeNum, trans, rot);
					break;
				}
			default:
				{
					//CX3DParser::printLog("Could not create this node type. %d", childNode->getNodeType());
					break;
				}
			}
	}
	//m_rentity->addParts(name.c_str());
	return true;
}


bool ShapeFileReader::createObj(CX3DParser *parser)
{

	if (!computeCenterOfMass(parser)) {
		LOG_ERR(("failed to comute center of mass"));
	}

	// Finding Group node
	MFNode *groupNodes = parser->searchNodesFromDirectChildrenOfRoot("Group");
	int groupNum = groupNodes->count();

	// In case of Group node exists
	if (groupNum > 0) {
		// Loop for the number of Groups nodes (if Group nodes exist)
		for (int i = 0; i < groupNum; i++) {
			CX3DGroupNode *pGroup = (CX3DGroupNode *)(groupNodes->getNode(i));

			// Fail to get Group node
			if (!pGroup) {
				LOG_ERR(("failed to get group node"));
				break;
			}

			// Success to get Group node
			else {
				// Finding Transform node included in Group node
				MFNode *transformNodes = pGroup->searchNodesFromDirectChildren("Transform");
				int transNum = transformNodes->count();

				// In case of no Transform node
				if (transNum < 0) {
					LOG_ERR(("could not find Transform node"));
					break;
				}

				// In case of Transform nodes exist
				// Loop for each Transform node
				for (int i = 0;i < transNum; i++) {
					// Get i-th Transform node
					CX3DTransformNode *pTrans = (CX3DTransformNode *)transformNodes->getNode(i);

					// Now, there is notransform
					Vector3d vec(0.0, 0.0, 0.0);

					if (!createTransform(pTrans, vec)) {
						break;
					}
				}
			} // else
		}  // for (int i = 0; i < groupNodes->count(); i++)
	} // if (groupNodes)

	// Finding Transform node if there is noGroup node
	else {
		// Finding Transform node
		MFNode *transformNodes = parser->searchNodesFromDirectChildrenOfRoot("Transform");
		int transNum = transformNodes->count();

		// In case of Transform nodes exist
		if (transNum > 0) {
			// Loop for each Transform node
			for (int i = 0;i < transNum; i++) {
				// Creation of Transform node
				CX3DTransformNode *pTrans = (CX3DTransformNode *)transformNodes->getNode(i);

				// Now, no transform
				Vector3d vec(0.0, 0.0, 0.0);

				if (!createTransform(pTrans, vec)) {
					break; // For confirmation
				}


			}
		} 
		// Finding Shape node if there is no Transform node
		else {
			// Finding Shape node
			MFNode *shape = parser->searchNodesFromDirectChildrenOfRoot("Shape");
			int shapeNum = shape->count();

			// In case of Shape nodes exist
			if (shapeNum > 0) {
				// No translation and rotation
				Vector3d trans(0.0, 0.0, 0.0);
				dReal rot[4] = {0.0, 0.0, 0.0, 0.0};
				for (int i = 0; i < shapeNum; i++) {
					CX3DShapeNode *pShape = (CX3DShapeNode*)shape->getNode(i);

					if (!createShape(pShape, shapeNum, trans, rot)) {
						break; 
					}

				}
			}
			// Returen false if Shape nodes also don't exist
			else {
				return false;
			}
		}
	}
	return true;
}

bool ShapeFileReader::createTransform(CX3DTransformNode *trans, Vector3d utrans)
{
	// Finding Shape nodes which is included by Transform node
	MFNode *shape = trans->searchNodesFromDirectChildren("Shape");
  
	// Get Translation and Rotation of Transform node
	SFVec3f    *t = trans->getTranslation();

	Vector3d translation;
	//translation.set(t->x() + utrans.x(), t->y() + utrans.y(), t->z() + utrans.z());
	translation.set(t->x(), t->y(), t->z());

	SFRotation *rot   = trans->getRotation();
	dReal rotation[4];
	rotation[0] = rot->x();
	rotation[1] = rot->y();
	rotation[2] = rot->z();
	rotation[3] = rot->rot();

  
	//LOG_MSG(("rot->rot() = %f",rot->rot()));
	// In case of no shape node
	if (!shape) {
		LOG_ERR(("cannot find shape node"));
		return false;
	}

	// In case of shape nodes exist
	else {
		// Loop for each shape node
		for (int i = 0; i < shape->count(); i++) {
			CX3DShapeNode *pShape = (CX3DShapeNode *)shape->getNode(i);
			// Fail to get shape node
			if (!pShape) {
				LOG_ERR(("name: %s could not get shape node (%d th)\n",pShape->getNodeName(),i));
				break; 
			}
			// Success to get Shape node
			else {
				// Creation of Shape node
				if (!createShape(pShape, shape->count(), translation, rotation)) {
					break; 
				}
			}
		}
	}
	return true;
}


bool ShapeFileReader::createShape(CX3DShapeNode *shape, int shapeNum, Vector3d trans, dReal *rot)
{
	///////////////////////
	// Creation of shape
	///////////////////////
	// Get Geometry
	CX3DNode *geoNode = shape->getGeometry()->getNode();
	if (!geoNode) {
		LOG_ERR(("could not get geometry field"));
		return false;
	}
	CSimplifiedShape *sshape;

	int type = 0;
	// Simple shape (Assumption)
	int nodeType = geoNode->getNodeType();
	if (nodeType != INDEXED_FACE_SET_NODE) {
		//LOG_SYS(("Create simple shape %d [Box:2 Sphere:25]", nodeType));

		// [TODO] Creation of simple shape
		sshape = createSimpleShape(nodeType, &type, geoNode);
	}
	// Automatic Recognition of shape
	else {
		sshape = CSimplifiedShapeFactory::calcAutoFromShapeNode(shape, CSimplifiedShape::BOX);
	}      
	// Get ODE world
	dWorldID world = m_entity->getWorld();

	// Get ODESpace
	dSpaceID space = m_entity->getSpace();

	// Creation of ODEBody
	dBodyID newbody = dBodyCreate(world);

	// Set of body
	if (m_shapeCount == 0) {
		m_entity->setBody(newbody);
		m_shapeCount++;
	}
	// Set of initial position of body
	Vector3d entpos;
	m_entity->getInitPosition(entpos);
	//dBodySetPosition(newbody, entpos.x(), entpos.y(), entpos.z());

	// ODEGeom
	dGeomID newgeom = dCreateGeomTransform(space);
	dGeomTransformSetCleanup(newgeom, 1);

	// set of mass
	double allmass = m_entity->getMass();

	// Calculate of (average) mass by deviding the total mass by the number of shapes
	double massValue = allmass/shapeNum;

	dMass mass;
	dMassSetZero(&mass);

	// Get scale
	Vector3d scale = m_entity->getScale();

	// Position of parts (Displacement from the CoG)
	Vector3d pos;

	// Type is determined at this line if the shape is 'simple'
	if (type == 0) {
		type = sshape->getType();
	}
	// Creation of ODE geometry according to the result of automatic recognition of shape
	switch (type) {
	case CSimplifiedShape::SPHERE:
		{
			CSimplifiedSphere *sphere = (CSimplifiedSphere *)sshape;
	  
			// Relative position from CoG
			double cx = (sphere->x() - m_gx) * scale.x();
			double cy = (sphere->y() - m_gy) * scale.y();
			double cz = (sphere->z() - m_gz) * scale.z();
			//pos.set(cx, cy, cz);

			//float r = sphere->radius();
	  
			// Calculate average of scale
			double mean = (scale.x() + scale.y() + scale.z()) / 3.0;
			double r = sphere->radius() * mean;
	  
			// LOG_SYS(("Creating ODE shape \"sphere\""));
			// Creation of geometry
      
			dGeomID g = dCreateSphere(0, r);
      
			dGeomTransformSetGeom(newgeom, g);

			//dGeomSetPosition(g, cx + trans.x(), cy + trans.y(), cz + trans.z());
			double tx = trans.x();
			double ty = trans.y();
			double tz = trans.z();
			dGeomSetPosition(g, cx + tx, cy + ty, cz + tz);
			m_entity->addGeom(newgeom);
			break;
		} // case CSimplifiedShape::SPHERE:

	case CSimplifiedShape::CYLINDER:
		{
			CSimplifiedCylinder *cylinder = (CSimplifiedCylinder *)sshape;
			double cx = (cylinder->x() - m_gx) * scale.x();
			double cy = (cylinder->y() - m_gy) * scale.y();
			double cz = (cylinder->z() - m_gz) * scale.z();
			// LOG_MSG(("cylinder->y = %f, m_gy = %f, cy = %f", cylinder->y(), m_gy, cy));
			pos.set(cx, cy, cz);

			double h = cylinder->height();
			double r = cylinder->radius();

			// Scale for height
			h = h * scale.y();

			// Scale for horizontal axis
			double mean = (scale.x() + scale.y()) / 2;
			r = r * mean;

			//LOG_SYS(("Creating ODE shape \"cylinder\""));

			// Creation of geometry
			dGeomID g = dCreateCylinder(0, r, h);

			dGeomTransformSetGeom(newgeom, g);

			// A: fit the major axis to y-axis
			dReal offq[4] = {0.707, 0.707, 0.0, 0.0};

			// B: Calculation of Quaternion from rotational axis and angle
			dReal a = cos(rot[3]/2);
			dReal b = sin(rot[3]/2);
			dQuaternion qua = {-a, rot[0] * b, rot[1] * b, rot[2] * b};

			double tx = trans.x();
			double ty = trans.y();
			double tz = trans.z();
			dGeomSetPosition(g, cx + tx, cy + ty, cz + tz);

			// A*B
			dQuaternion tmp_q;
			dQSetIdentity(tmp_q);
			dQMultiply0(tmp_q, offq, qua);
			dGeomSetQuaternion(g, tmp_q);
			//dGeomSetPosition(g, cx, cy, cz);
			m_entity->addGeom(newgeom);

			break;
		} // case CSimplifiedShape::CYLINDER:
	case CSimplifiedShape::BOX:
		{

			CSimplifiedBox *bx = (CSimplifiedBox *)sshape;

			double cx = (bx->x() - m_gx) * scale.x();
			double cy = (bx->y() - m_gy) * scale.y();
			double cz = (bx->z() - m_gz) * scale.z();
			//LOG_MSG(("bx->y() = %f, m_gy = %f", bx->y(), m_gy));
			pos.set(cx, cy, cz);

			double sx = bx->sx();
			double sy = bx->sy();
			double sz = bx->sz();

			// Avoid 0
			if (sx == 0) sx = 0.001;
			if (sy == 0) sy = 0.001;
			if (sz == 0) sz = 0.001;

			// Apply the scale
			sx = sx * scale.x();
			sy = sy * scale.y();
			sz = sz * scale.z();

			//LOG_SYS(("Creating ODE shape \"box\""));

			Size size(sx, sy, sz);

			// Creation of geometry
			dGeomID g = dCreateBox(0, sx, sy, sz);
			dGeomTransformSetGeom(newgeom, g);

			double tx = trans.x();
			double ty = trans.y();
			double tz = trans.z();
			dGeomSetPosition(g, cx + tx, cy + ty, cz + tz);
			//dGeomSetPosition(g, cx, cy, cz );
			//LOG_MSG(("geom pos (%f, %f, %f)",cx+tx, cy+ty, cz+tz));	    


			// Calculation of Quaternion from rotational axis and angle
			LOG_MSG(("rot[3] = %f",rot[3]));
			dReal a = cos(rot[3]/2);
			dReal b = sin(rot[3]/2);
			dQuaternion qua = {a, rot[0] * b, rot[1] * b, rot[2] * b};
			LOG_MSG(("qua = (%f, %f, %f, %f)", qua[0], qua[1], qua[2], qua[3]));
			dGeomSetQuaternion(g,qua);

			m_entity->addGeom(newgeom);
      
      
		} // case CSimplifiedShape::BOX:
	} // switch (sshape->getType()) {
	delete sshape;
	return true;
}


CSimplifiedShape *ShapeFileReader::createSimpleShape(int ntype, int *stype, CX3DNode *geo)
{
	CSimplifiedShape *shape=NULL;

	//LOG_MSG(("test"));
	//int type = geo->getNodeType();
	switch(ntype)
		{
		case SPHERE_NODE:
			{
				CX3DSphereNode *sphereNode = (CX3DSphereNode*)geo;
				float r = sphereNode->getRadius()->getValue();
				CSimplifiedSphere *sphere = new CSimplifiedSphere();
				sphere->radius(r);

				// Set position to 0(temporal)
				sphere->x(0.0);
				sphere->y(0.0);
				sphere->z(0.0);

				*stype = CSimplifiedShape::SPHERE;
				return sphere;
				break;
			}
		case BOX_NODE:
			{
				CX3DBoxNode *boxNode = (CX3DBoxNode*)geo;

				float x, y, z;
				boxNode->getSize()->getValue(x, y, z);
				CSimplifiedBox *box = new CSimplifiedBox();
				box->sx(x);
				box->sy(y);
				box->sz(z);
				// Set position to 0(temporal)
				box->x(0.0);
				box->y(0.0);
				box->z(0.0);

				*stype = CSimplifiedShape::BOX;
				return box;
				break;
			}
		case CYLINDER_NODE:
			{
				CX3DCylinderNode *cylinderNode = (CX3DCylinderNode*)geo;
				float rad = cylinderNode->getRadius()->getValue();
				float height = cylinderNode->getHeight()->getValue();
				CSimplifiedCylinder *cylinder = new CSimplifiedCylinder();
				cylinder->radius(rad);
				cylinder->height(height);
				LOG_MSG(("height = %f, radius = %f",height, rad));

				cylinder->x(0.0);
				cylinder->y(0.0);
				cylinder->z(0.0);

				*stype = CSimplifiedShape::CYLINDER;
				return cylinder;
				break;
			}
		}

	return shape;
}


bool ShapeFileReader::computeCenterOfMass(CX3DParser *parser)
{
	MFNode *ind = parser->searchNodesFromAllChildrenOfRoot("IndexedFaceSet");
	int indsize = ind->count();
	if (indsize == 0) return true;
	int allPt = 0;

	double max_x = -10000.0;
	double max_y = -10000.0;
	double max_z = -10000.0;
  
	double min_x = 10000.0;
	double min_y = 10000.0;
	double min_z = 10000.0;
  
	// [ToDo] What should the system do in case of transform exist?
	for (int i = 0; i < indsize; i++) {
		CX3DIndexedFaceSetNode *index_node = (CX3DIndexedFaceSetNode *)ind->getNode(i);
		CX3DCoordinateNode *Coord = (CX3DCoordinateNode *)(index_node->getCoord()->getNode());

		// Get vertex coordinate of polygon
		MFVec3f *coord = Coord->getPoint();

		// Number of vertexes of polygon
		int numPt = coord->count();
		allPt += numPt;

		// In case of the average of vertex coordinate should be the CoG
		for (int i = 0; i < numPt; i++) {
			SFVec3f vec = coord->getValue(i);
			double x = vec.x();
			double y = vec.y();
			double z = vec.z();
			if      (max_x < x) max_x = x;
			else if (x < min_x) min_x = x;
			if      (max_y < y) max_y = y;
			else if (y < min_y) min_y = y;
			if      (max_z < z) max_z = z;
			else if (z < min_z) min_z = z;
		}

	}

	// Calculation of center
	m_gx = (min_x + max_x)/2;
	m_gy = (min_y + max_y)/2;
	m_gz = (min_z + max_z)/2;

	LOG_MSG(("center of mass (%f, %f, %f)",m_gx, m_gy, m_gz));

	///CX3DParser::printLog("position :%f, %f, %f", m_gx, m_gy, m_gz);
	return true;
}

bool ShapeFileReader::COPFromMFNode(MFNode *node)
{

	int size = node->count();
	if (size == 0) return true;
	int allPt = 0;

	double max_x = -10000.0;
	double max_y = -10000.0;
	double max_z = -10000.0;
  
	double min_x = 10000.0;
	double min_y = 10000.0;
	double min_z = 10000.0;

	/*
	 */
	MinMaxFromMFNode(node, &min_x, &min_y, &min_z, &max_x, &max_y, &max_z);

	// Calculation of center
	m_gx = (min_x + max_x)/2;
	m_gy = (min_y + max_y)/2;
	m_gz = (min_z + max_z)/2;

	LOG_MSG(("center of point(%f, %f, %f)",m_gx, m_gy, m_gz));
	return true;
}


bool ShapeFileReader::MinMaxFromIndexedNode(CX3DCoordinateNode* Coord,
                                            double *min_x, 
                                            double *min_y,
                                            double *min_z,
                                            double *max_x,
                                            double *max_y,
                                            double *max_z)
{
	// Get vertex coordinate of polygon
	MFVec3f *coord = Coord->getPoint();

	// Number of vertexes of polygon
	int numPt = coord->count();

	// Get max and min
	for (int i = 0; i < numPt; i++) {
		SFVec3f vec = coord->getValue(i);
		double x = vec.x();
		double y = vec.y();
		double z = vec.z();
		//LOG_MSG(("vec(%f, %f, %f)", x, y, z));
		if      (*max_x < x) *max_x = x;
		else if (x < *min_x) *min_x = x;
		if      (*max_y < y) *max_y = y;
		else if (y < *min_y) *min_y = y;
		if      (*max_z < z) *max_z = z;
		else if (z < *min_z) *min_z = z;
	}
	return true;
}

bool ShapeFileReader::MinMaxFromMFNode(MFNode *node,
                                       double *min_x, 
                                       double *min_y,
                                       double *min_z,
                                       double *max_x,
                                       double *max_y,
                                       double *max_z)
{
	int size = node->count();
		
	for (int i = 0; i < size; i++) {
		// Get child node
		CX3DNode *childNode = node->getNode(i);
		switch(childNode->getNodeType())
			{
			case TRANSFORM_NODE:
				{

					CX3DTransformNode *child = (CX3DTransformNode *)childNode;
					// Get Translation and Rotation of Transform node
					SFVec3f    *t = child->getTranslation();
					// Get children field
					MFNode *children = child->getChildren();

					MinMaxFromMFNode(children, min_x, min_y, min_z, max_x, max_y, max_z);

					break;
				}
			case SHAPE_NODE:
				{

					CX3DShapeNode *child = (CX3DShapeNode *)childNode;

					// Get Geometry
					CX3DNode *geoNode = child->getGeometry()->getNode();
					if (!geoNode)
						{
							LOG_ERR(("could not get geometry field"));
							return false;
						}
	  
					int nodeType = geoNode->getNodeType();
	  
					// simple shape
					if (nodeType != INDEXED_FACE_SET_NODE) {
						switch(nodeType)
							{
							case SPHERE_NODE:
								{
									CX3DSphereNode *sphereNode = (CX3DSphereNode*)geoNode;
									float r = sphereNode->getRadius()->getValue();
									*min_x = *min_y = *min_z = -r;
									*max_x = *max_y = *max_z = r;
									break;
								}
							case BOX_NODE:
								{
									CX3DBoxNode *boxNode = (CX3DBoxNode*)geoNode;
									float x, y, z;
									boxNode->getSize()->getValue(x, y, z);
									*min_x = -x;
									*min_y = -y;
									*min_z = -z;
									*max_x = x;
									*max_y = y;
									*max_z = z;
									break;
								}
							case CYLINDER_NODE:
								{
									CX3DCylinderNode *cylinderNode = (CX3DCylinderNode*)geoNode;
									float rad = cylinderNode->getRadius()->getValue();
									float height = cylinderNode->getHeight()->getValue();
									*min_x = *min_z = -rad;
									*max_x = *max_z = rad;
									*min_y  = -height/2;
									*max_y  = height/2;
								}
							default:
								break;
							}
					}
					else {
						// Get IndexedFaceSetNode
						CX3DIndexedFaceSetNode *index_node = (CX3DIndexedFaceSetNode *)geoNode;
						if (index_node) {
							// Get vertex coordinate (Shape cannot be created without this)
							CX3DCoordinateNode *Coord = (CX3DCoordinateNode *)(index_node->getCoord()->getNode());
							if (Coord) {
								MinMaxFromIndexedNode(Coord, min_x, min_y, min_z, max_x, max_y, max_z);
							}
						}
					}
					break;
				}
			default:
				break;
			}
	}
	return true;
}
