/*
 * Added comments by Tetsunari Inamura on 2014-02-27
 */

#ifndef _CSIMPLIFIED_SHAPE_H_
#define _CSIMPLIFIED_SHAPE_H_

#include "CX3DNode.h"
#include "CX3DIndexedFaceSetNode.h"
#include "CX3DTransformNode.h"
#include "CX3DShapeNode.h"
#include "CX3DCylinderNode.h"
#include <float.h>
#include <vector>


//----------------------------------------------
// Base class to represent simplified shape
//----------------------------------------------
class CSimplifiedShape
{
 public:
	enum SHAPE_TYPE {
		NONE,
		SPHERE,
		CYLINDER,
		BOX
	};

	CSimplifiedShape() {}
	virtual ~CSimplifiedShape() {}

	// Return type of simplified shape (Shpere, Cylinder, Box)
	virtual SHAPE_TYPE getType() = 0;

	// Distortion value (=hizumi)
	float hizumi() { return m_hizumi; }
	void hizumi(float hizumi) { m_hizumi = hizumi; }

 protected:
	// Distortion value (=hizumi)
	float m_hizumi;
};


//----------------------------------------------
// Sphere
//----------------------------------------------
class CSimplifiedSphere : public CSimplifiedShape
{
public:
	CSimplifiedSphere() {}
	virtual ~CSimplifiedSphere() {}

	SHAPE_TYPE getType() { return SPHERE; }

	float radius() { return m_r; }
	void radius(float r) { m_r = r; }

	// Position of the center
	float x() { return m_x; }
	void x(float x) { m_x = x; }

	float y() { return m_y; }
	void y(float y) { m_y = y; }

	float z() { return m_z; }
	void z(float z) { m_z = z; }

private:
	// radius
	float m_r;

	// center position
	float m_x, m_y, m_z;
};


//----------------------------------------------
// Cylinder
//----------------------------------------------
class CSimplifiedCylinder : public CSimplifiedShape
{
 public:
	CSimplifiedCylinder() {}
	virtual ~CSimplifiedCylinder() {}

	SHAPE_TYPE getType() { return CYLINDER; }

	float radius() { return m_r; }
	void radius(float r) { m_r = r; }

	float height() { return m_h; }
	void height(float h) { m_h = h; }

	// Position of center
	float x() { return m_x; }
	void x(float x) { m_x = x; }
	float y() { return m_y; }
	void y(float y) { m_y = y; }
	float z() { return m_z; }
	void z(float z) { m_z = z; }

private:
	// radius
	float m_r;

	// height
	float m_h;

	// center of bottom face (bottom is (x,z) plane
	float m_x, m_y, m_z;
};


//----------------------------------------------
// Box
//----------------------------------------------
class CSimplifiedBox : public CSimplifiedShape
{
public:
	CSimplifiedBox() {}
	virtual ~CSimplifiedBox() {}

	SHAPE_TYPE getType() { return BOX; }

	float sx() { return m_sx; }
	void sx(float x) { m_sx = x; }

	float sy() { return m_sy; }
	void sy(float y) { m_sy = y; }

	float sz() { return m_sz; }
	void sz(float z) { m_sz = z; }

	float x() { return m_x; }
	void x(float x) { m_x = x; }

	float y() { return m_y; }
	void y(float y) { m_y = y; }

	float z() { return m_z; }
	void z(float z) { m_z = z; }

	// Position of a corner of box
	float x1() { return m_x1; }
	void x1(float x) { m_x1 = x; }

	float y1() { return m_y1; }
	void y1(float y) { m_y1 = y; }

	float z1() { return m_z1; }
	void z1(float z) { m_z1 = z; }

	// Position of another corner of box
	float x2() { return m_x2; }
	void x2(float x) { m_x2 = x; }

	float y2() { return m_y2; }
	void y2(float y) { m_y2 = y; }

	float z2() { return m_z2; }
	void z2(float z) { m_z2 = z; }

private:
	// positions
	float m_x1, m_y1, m_z1;
	float m_x2, m_y2, m_z2;

private:
	// Length of side of box
	float m_sx, m_sy, m_sz;
	// Gap from the center
	float m_x, m_y, m_z;
};


//--------------------------------------------------------------------
// Calculate simplified shape from Shape which includes IndexedFaceSet
//--------------------------------------------------------------------
class CSimplifiedShapeFactory
{
public:
	// ---------------------------------------------------------
	// Create simplified shape from tree structure consists of Transform node and Shape node
	//
	// MFNode should have the data according to the following structure 
	//
	//	+--T
	//	|  +--S
	//	|  :
	//	|  +--S
	//	|  +--T
	//	|  :
	//	|  +--T
	//	|	  +--S
	//	|	  :
	//	|	  +--S
	//	|	  +--T
	//	|	  :
	//	|	  +--T
	//	|  :
	//	+--T
	//
	//	(1) Tree has more than 0 Transform nodes (T)
	//	(2) Each Transform node has more than 0 Shape nodes (S) and more than 0 Transform nodes
	//	(3) Shape node does not have any nodes
	//	(4) Transform node can have recursive tree structure
	// ---------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromTree(MFNode *tree);
	static CSimplifiedCylinder *calcCylinderFromTree(MFNode *tree);
	static CSimplifiedBox *calcBoxFromTree(MFNode *tree);
	static CSimplifiedShape *calcAutoFromTree(MFNode *tree);
	static CSimplifiedShape *calcFromTree(MFNode *tree, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	// Create sole simplified shape from multiple Shapes
	// If hint in the second argument is not given, suitable simplified shape will be return automatically
	// If hint is given as desired shape, the same shape will be returned
	//
	// Processes that call this API have to clear the memory for object as return object
	// The returned object should be deleted by the process that call this API
	//
	//	(ex)
	//	CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromShapeNodes(shapes);
	//	if (ss) {
	//	   ....
	//     delete ss;   // <--- This is required
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromShapeNodes(MFNode *shapes);
	static CSimplifiedCylinder *calcCylinderFromShapeNodes(MFNode *shapes);
	static CSimplifiedBox *calcBoxFromShapeNodes(MFNode *shapes);
	static CSimplifiedShape *calcAutoFromShapeNodes(MFNode *shapes);
	static CSimplifiedShape *calcFromShapeNodes(MFNode *shapes, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	// Create sole simplified shape from a Shape
	//
	// Processes that call this API have to clear the memory for object as return object
	// The returned object should be deleted by the process that call this API
	//
	//	(ex)
	//	CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromShapeNode(pShape);
	//	if (ss) {
	//	   ....
	//     delete ss;   // <--- This is required
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedCylinder *calcCylinderFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedBox *calcBoxFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedShape *calcAutoFromShapeNode(CX3DShapeNode *pShape, CSimplifiedShape::SHAPE_TYPE type = CSimplifiedShape::NONE);
	static CSimplifiedShape *calcFromShapeNode(CX3DShapeNode *pShape, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	// Create sole simplified shape from an IndexedFaceSet
	//
	// Processes that call this API have to clear the memory for object as return object
	// The returned object should be deleted by the process that call this API
	//
	//	(ex)
	//	CSimplifiedSphere *ss = CSimplifiedShapeFactory::calcSphere(pFaceSet);
	//	if (ss) {
	//	   ....
	//     delete ss;   // <--- This is required
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphere(CX3DIndexedFaceSetNode *pFaceSet);
	static CSimplifiedCylinder *calcCylinder(CX3DIndexedFaceSetNode *pFaceSet);
	static CSimplifiedBox *calcBox(CX3DIndexedFaceSetNode *pFaceSet);

private:
	static void extractPointsFromTree(CX3DTransformNode *pTrans, SFVec3f t, SFVec4f q, std::vector<SFVec3f>& vecPos);

	static void extractPointsFromShapeNodes(MFNode *shapeNodes, std::vector<SFVec3f>& vecPos);

	static void extractPointsFromShapeNode(CX3DShapeNode *pShape, std::vector<SFVec3f>& vecPos);

	static void extractPointsFromIndexedFaceSetNode(CX3DIndexedFaceSetNode *pFaceSet, std::vector<SFVec3f>& vecPos);

	// Core parts of each simplified shape calculation
	static CSimplifiedSphere   *calcSphere   (std::vector<SFVec3f>& vecPos);
	static CSimplifiedCylinder *calcCylinder (std::vector<SFVec3f>& vecPos);
	static CSimplifiedCylinder *calcCylinder2(std::vector<SFVec3f>& vecPos);
	static CSimplifiedBox      *calcBox      (std::vector<SFVec3f>& vecPos);

	// Calculate distortion value (=hizumi); TODO
	static double calcHizumiWithSphere(float r, float gx, float gy, float gz, std::vector<SFVec3f>& vecPos);
	static double calcHizumiWithCylinder(float r, float cx, float cz, std::vector<SFVec3f>& vecPos);
	static double calcHizumiWithBox(float x1, float y1, float z1, float x2, float y2, float z2, std::vector<SFVec3f>& vecPos);

public:
	static CSimplifiedCylinder *calcCylinder(CX3DCylinderNode *pCylinderNode);

	/**
	 * Judge whether the candidate shape can be regarded as cylinder
	 * @param pNodeData
	 * @param radiusEstimated radius will be stored if it is cylinder
	 * @param heightEstimated height will be stored if it is cylinder
	 * @param xAxis[]		  coordinate of cylinder's axis (xAxis[0]:head,xAxis[1]:tail)
	 * @param yAxis[]		  coordinate of cylinder's axis (yAxis[0]:head,yAxis[1]:tail)
	 * @param zAxis[]		  coordinate of cylinder's axis (zAxis[0]:head,zAxis[1]:tail)
	 * @return  true when the candidate should be regarded as cylinder
	 */
	static bool isCylinder(
			CX3DIndexedFaceSetNode *pNodeData,
			float &radiusEstimated,
			float &heightEstimated,
			float xAxis[],
			float yAxis[],
			float zAxis[]
		);

private:
	#define INVALID_VALUE (FLT_MAX)
	#define SMALL_VALUE (0.001f)
	#define SLIGHT_SMALL_VALUE (0.01f)

public:
	// Find the minimum value from six float values
	// six values should be stored in f[0]--f[5]
	static float min6Float(float *f);

	// Calculate square of distance between two points
	static float sqrDist(float px, float py, float pz, float qx, float qy, float qz);

	static float getDistance(float px, float py, float pz, float qx, float qy, float qz);

	/**
	 * Calculate center of mass (CoM)
	 * @param coords position of vertex
	 * @param gx  x of CoM should be stored
	 * @param gy  y of CoM should be stored
	 * @param gz  z of CoM should be stored
	 */
	static void getCenter(MFVec3f *coords,float &gx,float &gy,float &gz);

	/**
	 * Find the nearest point from the base point (gx,gy,gz)
	 * @param coords  List of position of vertex
	 * @param gx	  x of the base point
	 * @param gy	  y of the base point
	 * @param gz	  z of the base point
	 * @param destX[] x of the nearest points should be stored as array
	 * @param destY[] y of the nearest points should be stored as array
	 * @param destZ[] z of the nearest points should be stored as array
	 * @param num	  The number of points to be stored in the above array destX,Y,Z
	 */
	static void getNearPoint(MFVec3f *coords,float gx,float gy,float gz,float destX[],float destY[],float destZ[],int num);
};

#endif


