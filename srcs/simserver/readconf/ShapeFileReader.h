/*
 * Modified by MSI on 2011-05-19
 * Modified by Tetsunari Inamura on 2014-02-25
 *    Translated Japanese comments into English
 */

#ifndef ShapeFileReader_h
#define ShapeFileReader_h

#include "Logger.h"
#include "SSimEntity.h"
#include "CX3DParser.h"
#include "SParts.h"
#include "CSimplifiedShape.h"
#include "Vector3d.h"

class ShapeFileReader
{
public:
	ShapeFileReader(SSimEntity *entity);
	ShapeFileReader(SSimRobotEntity *entity);
	bool createRobotObj(CX3DParser *parser);
	// Specify the name of parent joint by the last argument
	bool createRobotJoint(CX3DOpenHRPJointNode *jNode, Vector3d vec, const char *jname);
	bool createRobotSegment(CX3DOpenHRPSegmentNode *sNode, Vector3d vec);
	bool createObj(CX3DParser *parser);
	bool createTransform(CX3DTransformNode *trans, Vector3d vec);

	//! Create ODEShape
	bool createShape(CX3DShapeNode *shape, int partsNum, Vector3d vec, dReal *rot);
	CSimplifiedShape *createSimpleShape(int ntype, int *type, CX3DNode *geo);

	//! Calculation of the center position
	bool computeCenterOfMass(CX3DParser *parser);

	//! Calculate the Center of Parts which belong to a segment
	bool COPFromMFNode(MFNode *node);

	//! Calculate max/min number of vertex of polygon from Indexednode
	bool MinMaxFromIndexedNode(CX3DCoordinateNode* coord,
			double *minx,
			double *miny,
			double *minz,
			double *maxx,
			double *maxy,
			double *maxz);

	//! Calculate max/min number of vertex of polygon from MFNode
	bool MinMaxFromMFNode(MFNode *node,
			double *minx, 
			double *miny,
			double *minz,
			double *maxx,
			double *maxy,
			double *maxz);

private:
	SSimEntity      *m_entity;  // Entity which does not have joints
	SSimRobotEntity *m_rentity; // Entity which have joints
	CX3DParser      *m_pParser;

	// not used now
	//std::vector<SSimJoint> tmp_joint;

	double m_gx;
	double m_gy;
	double m_gz;
	int    m_shapeCount;
	bool   m_isRobot;
};

#endif
