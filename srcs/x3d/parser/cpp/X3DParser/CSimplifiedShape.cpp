/*
 * Modified by msi     on 2011-05-12
 * Modified by Okamoto on 2012-12-21
 * Added comments by Tetsunari Inamura on 2014-02-27
 */

#include "CSimplifiedShape.h"
#include "CX3DParser.h"
#include "CX3DField.h"
#include "CX3DNode.h"
#include "CX3DTransformNode.h"
#include "CX3DShapeNode.h"
#include "CX3DIndexedFaceSetNode.h"
#include "CX3DCoordinateNode.h"
#include <math.h>
#include <vector>

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// Calculation of simplified shape
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

// -----------------------------------------------------------------------------------------
// Create one simplified shape from tree in Transform node which has multiple Shape elements
// -----------------------------------------------------------------------------------------
CSimplifiedSphere *CSimplifiedShapeFactory::calcSphereFromTree(MFNode *tree)
{
	return (CSimplifiedSphere *)calcFromTree(tree, CSimplifiedShape::SPHERE);
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinderFromTree(MFNode *tree)
{
	return (CSimplifiedCylinder *)calcFromTree(tree, CSimplifiedShape::CYLINDER);
}

CSimplifiedBox *CSimplifiedShapeFactory::calcBoxFromTree(MFNode *tree)
{
	return (CSimplifiedBox *)calcFromTree(tree, CSimplifiedShape::BOX);
}


CSimplifiedShape *CSimplifiedShapeFactory::calcAutoFromTree(MFNode *tree)
{
	// Automatic selection of simplified shape
	// Selection from three candidates which has the smallest gap between ideal shape

	CSimplifiedShape *retValue = NULL;
	int i=-1;
	CSimplifiedSphere *sp = NULL;
	CSimplifiedCylinder *cy = NULL;
	CSimplifiedBox *bx = NULL;

	// Identify the shape which has the smallest gap --> i
	sp = (CSimplifiedSphere *)CSimplifiedShapeFactory::calcFromTree(tree, CSimplifiedShape::SPHERE);
	if (sp)	{
		cy = (CSimplifiedCylinder *)CSimplifiedShapeFactory::calcFromTree(tree, CSimplifiedShape::CYLINDER);
		if (cy)	{
			bx = (CSimplifiedBox *)CSimplifiedShapeFactory::calcFromTree(tree, CSimplifiedShape::BOX);
			if (bx)	{
				float minHizumi = sp->hizumi();
				i = 0;

				if (cy->hizumi() < minHizumi) {
					minHizumi = cy->hizumi();
					i = 1;
				}

				if (bx->hizumi() < minHizumi) {
					i = 2;
				}
			}
		}
	}
	// Preparation of return value
	switch (i) {
	case 0:
		retValue = (CSimplifiedShape *)sp;
		sp = NULL;
		break;

	case 1:
		retValue = (CSimplifiedShape *)cy;
		cy = NULL;
		break;

	case 2:
		retValue = (CSimplifiedShape *)bx;
		bx = NULL;
		break;
	}

	if (sp) delete sp;
	if (cy) delete cy;
	if (bx) delete bx;

	return retValue;
}


CSimplifiedShape *CSimplifiedShapeFactory::calcFromTree(MFNode *tree, CSimplifiedShape::SHAPE_TYPE hint)
{
	if (!tree || tree->count() == 0) return NULL;

	if (hint == CSimplifiedShape::NONE)	{
		// In case there is no hint, that is automatic selection
		return calcAutoFromTree(tree);
	}

	// In case there is hint
	std::vector<SFVec3f> vecPos;
	SFVec3f t;
	SFVec4f q;

	// Extract all of the vertex under the tree; store them in vecPos
	vecPos.clear();              // The result should be put in here
	t.setValue(0, 0, 0);
	q.setValue(1, 0, 0, 0);
	int nTrans = tree->count();
	for (int i=0; i<nTrans; i++) {
		CX3DTransformNode *pTrans = (CX3DTransformNode *)(tree->getNode(i));
		if (!pTrans) continue;

		extractPointsFromTree(pTrans, t, q, vecPos);
	}

	// Create simplified shape
	switch (hint) {
	case CSimplifiedShape::SPHERE:
		return (CSimplifiedShape *)calcSphere(vecPos);

	case CSimplifiedShape::CYLINDER:
		return (CSimplifiedShape *)calcCylinder(vecPos);

	case CSimplifiedShape::BOX:
		return (CSimplifiedShape *)calcBox(vecPos);

	default:
		return NULL;
	}
}


// -----------------------------------------------
// Calculate simplified shape from multiple shapes
// -----------------------------------------------
CSimplifiedSphere *CSimplifiedShapeFactory::calcSphereFromShapeNodes(MFNode *shapes)
{
	return (CSimplifiedSphere *)calcFromShapeNodes(shapes, CSimplifiedShape::SPHERE);
}


CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinderFromShapeNodes(MFNode *shapes)
{
	return (CSimplifiedCylinder *)calcFromShapeNodes(shapes, CSimplifiedShape::CYLINDER);
}


CSimplifiedBox *CSimplifiedShapeFactory::calcBoxFromShapeNodes(MFNode *shapes)
{
	return (CSimplifiedBox *)calcFromShapeNodes(shapes, CSimplifiedShape::BOX);
}


CSimplifiedShape *CSimplifiedShapeFactory::calcAutoFromShapeNodes(MFNode *shapes)
{
	// Selection from three candidates which has the smallest gap between ideal shape
	CSimplifiedShape *retValue = NULL;
	int i=-1;
	CSimplifiedSphere *sp = NULL;
	CSimplifiedCylinder *cy = NULL;
	CSimplifiedBox *bx = NULL;

	// Identify the shape which has the smallest gap --> i
	sp = (CSimplifiedSphere *)CSimplifiedShapeFactory::calcFromShapeNodes(shapes, CSimplifiedShape::SPHERE);
	if (sp)	{
		cy = (CSimplifiedCylinder *)CSimplifiedShapeFactory::calcFromShapeNodes(shapes, CSimplifiedShape::CYLINDER);
		if (cy)	{
			bx = (CSimplifiedBox *)CSimplifiedShapeFactory::calcFromShapeNodes(shapes, CSimplifiedShape::BOX);
			if (bx)	{
				float minHizumi = sp->hizumi();
				i = 0;

				if (cy->hizumi() < minHizumi) {
					minHizumi = cy->hizumi();
					i = 1;
				}

				if (bx->hizumi() < minHizumi) {
					i = 2;
				}
			}
		}
	}
	// Preparation of return value
	switch (i) {
	case 0:
		retValue = (CSimplifiedShape *)sp;
		sp = NULL;
		break;

	case 1:
		retValue = (CSimplifiedShape *)cy;
		cy = NULL;
		break;

	case 2:
		retValue = (CSimplifiedShape *)bx;
		bx = NULL;
		break;
	}

	if (sp) delete sp;
	if (cy) delete cy;
	if (bx) delete bx;

	return retValue;
}


CSimplifiedShape *CSimplifiedShapeFactory::calcFromShapeNodes(MFNode *shapes, CSimplifiedShape::SHAPE_TYPE hint)
{
	if (!shapes || shapes->count() == 0) return NULL;

	if (hint == CSimplifiedShape::NONE)	{
		// In case there is no hint, that is automatic selection
		return calcAutoFromShapeNodes(shapes);
	}

	// In case there is hint
	std::vector<SFVec3f> vecPos;

	extractPointsFromShapeNodes(shapes, vecPos);

	switch (hint) {
	case CSimplifiedShape::SPHERE:
		return calcSphere(vecPos);

	case CSimplifiedShape::CYLINDER:
		return calcCylinder(vecPos);

	case CSimplifiedShape::BOX:
		return calcBox(vecPos);

	default:
		return NULL;	
	}
}



// ------------------------------------------
// Calculate simplified shape from sole shape
// ------------------------------------------
CSimplifiedSphere *CSimplifiedShapeFactory::calcSphereFromShapeNode(CX3DShapeNode *pShape)
{
	return (CSimplifiedSphere *)calcFromShapeNode(pShape, CSimplifiedShape::SPHERE);
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinderFromShapeNode(CX3DShapeNode *pShape)
{
	return (CSimplifiedCylinder *)calcFromShapeNode(pShape, CSimplifiedShape::CYLINDER);
}

CSimplifiedBox *CSimplifiedShapeFactory::calcBoxFromShapeNode(CX3DShapeNode *pShape)
{
	return (CSimplifiedBox *)calcFromShapeNode(pShape, CSimplifiedShape::BOX);
}

CSimplifiedShape *CSimplifiedShapeFactory::calcAutoFromShapeNode(CX3DShapeNode *pShape, CSimplifiedShape::SHAPE_TYPE type)
{
  if (!pShape) return NULL;

  CSimplifiedShape *retValue = NULL;
  int i=-1;
  CSimplifiedSphere *sp = NULL;
  CSimplifiedCylinder *cy = NULL;
  CSimplifiedBox *bx = NULL;

  // Identify the shape which has the smallest gap --> i
  sp = (CSimplifiedSphere *)CSimplifiedShapeFactory::calcFromShapeNode(pShape, CSimplifiedShape::SPHERE);
  if (sp) {
      cy = (CSimplifiedCylinder *)CSimplifiedShapeFactory::calcFromShapeNode(pShape, CSimplifiedShape::CYLINDER);
      if (cy) {
		  bx = (CSimplifiedBox *)CSimplifiedShapeFactory::calcFromShapeNode(pShape, CSimplifiedShape::BOX);
		  if (bx) {
			  if(type != 0) {
				  i = type - 1;
			  }
			  else {
				  //printf("hizumi = (%f, %f, %f)\n", sp->hizumi(), cy->hizumi(), bx->hizumi());
				  float minHizumi = sp->hizumi();
				  i = 0;
				  
				  if (cy->hizumi() < minHizumi) {
					  minHizumi = cy->hizumi();
					  i = 1;
				  }
				  // If the values are the same, select box
				  if (bx->hizumi() <= minHizumi) {
					  i = 2; //TODO: Magic number, does it mean BOX?
				  }
			  }
		  }
	  }
  }
  // Preparation of return value
  switch (i) {
  case 0:
      retValue = (CSimplifiedShape *)sp;
      sp = NULL;
      break;
	  
  case 1:
      retValue = (CSimplifiedShape *)cy;
      cy = NULL;
      break;
	  
  case 2:
      retValue = (CSimplifiedShape *)bx;
      bx = NULL;
      break;
  }
  
  if (sp) delete sp;
  if (cy) delete cy;
  if (bx) delete bx;

  return retValue;
}


CSimplifiedShape *CSimplifiedShapeFactory::calcFromShapeNode(CX3DShapeNode *pShape, CSimplifiedShape::SHAPE_TYPE hint)
{
	if (!pShape) return NULL;

	if (hint == CSimplifiedShape::NONE)	{
		// There is hint
		return calcAutoFromShapeNode(pShape);
	}
	// There is no hint
	std::vector<SFVec3f> vecPos;

	extractPointsFromShapeNode(pShape, vecPos);

	switch (hint) {
	case CSimplifiedShape::SPHERE:
		return calcSphere(vecPos);
		
	case CSimplifiedShape::CYLINDER:
		return calcCylinder2(vecPos);
		
	case CSimplifiedShape::BOX:
		return calcBox(vecPos);
		
	default:
		return NULL;	
	}
}


// -----------------------------------------------
// Calculate simplified shape from sole IndexedFaceSetNode
// -----------------------------------------------

CSimplifiedSphere *CSimplifiedShapeFactory::calcSphere(CX3DIndexedFaceSetNode *pFaceSet)
{
	std::vector<SFVec3f> vecPos;

	extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);

	return calcSphere(vecPos);
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinder(CX3DIndexedFaceSetNode *pFaceSet)
{
	std::vector<SFVec3f> vecPos;

	extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);

	return calcCylinder(vecPos);
}

CSimplifiedBox *CSimplifiedShapeFactory::calcBox(CX3DIndexedFaceSetNode *pFaceSet)
{
	std::vector<SFVec3f> vecPos;

	extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);

	return calcBox(vecPos);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// Extract set of points from node
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

void CSimplifiedShapeFactory::extractPointsFromTree(CX3DTransformNode *pTrans, SFVec3f t, SFVec4f q, std::vector<SFVec3f>& vecPos)
{
	if (!pTrans) return;

	MFNode *transNodes = pTrans->searchNodesFromDirectChildren("Transform");
	if (transNodes)	{
		int nTrans = transNodes->count();
		for (int i=0; i<nTrans; i++) {
			CX3DTransformNode *pChildTrans = (CX3DTransformNode *)(transNodes->getNode(i));
			if (!pChildTrans) continue;

			// *********************
			// Correct at here
			// *********************
			SFVec3f newT = t;
			SFVec4f newQ = q;

			extractPointsFromTree(pChildTrans, newT, newQ, vecPos);
		}

		delete transNodes;  // This is required	
		transNodes = NULL;
	}

	MFNode *shapeNodes = pTrans->searchNodesFromDirectChildren("Shape");
	if (shapeNodes)	{
		int nShapes = shapeNodes->count();
		for (int i=0; i<nShapes; i++) {
			CX3DShapeNode *pChildShape = (CX3DShapeNode *)(shapeNodes->getNode(i));
			if (!pChildShape) continue;

			extractPointsFromShapeNode(pChildShape, vecPos);
		}

		delete shapeNodes;	
		shapeNodes = NULL;
	}

	return;
}


void CSimplifiedShapeFactory::extractPointsFromShapeNodes(MFNode *shapeNodes, std::vector<SFVec3f>& vecPos)
{
	if (!shapeNodes) return;

	// The number of nodes which are held by MFNode (=the number of Shape nodes)
	int nShape = shapeNodes->count();
	if (nShape <= 0) return;

	for (int iShape=0; iShape<nShape; iShape++)	{
		CX3DShapeNode *pShape = (CX3DShapeNode *)(shapeNodes->getNode(iShape));
		if (!pShape) continue;

		extractPointsFromShapeNode(pShape, vecPos);
	}
}

void CSimplifiedShapeFactory::extractPointsFromShapeNode(CX3DShapeNode *pShape, std::vector<SFVec3f>& vecPos)
{
	if (!pShape) return;

	// -------------------------------------------
	// Extract IndexedFaceSet node
	// -------------------------------------------
	CX3DNode *pNode = (CX3DIndexedFaceSetNode *)(pShape->getGeometry()->getNode());
	if (pNode && (pNode->getNodeType() == INDEXED_FACE_SET_NODE)) {

		CX3DIndexedFaceSetNode *pFaceSet = (CX3DIndexedFaceSetNode *)pNode;

		extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);
	}
}

void CSimplifiedShapeFactory::extractPointsFromIndexedFaceSetNode(CX3DIndexedFaceSetNode *pFaceSet, std::vector<SFVec3f>& vecPos)
{
	if (!pFaceSet) return;

	CX3DCoordinateNode *pCoord = (CX3DCoordinateNode *)(pFaceSet->getCoord()->getNode());
	if (pCoord)	{
		// Position of vertex
		MFVec3f *coords = pCoord->getPoint();

		// Index of vertex
		MFInt32 *coordIndex = pFaceSet->getCoordIndex();
		bool bHasCoordIndex = (coordIndex->count() > 0) ? true : false;

		int n = bHasCoordIndex ? coordIndex->count() : coords->count();

		// Extract vertex
		for (int i=0; i<n; i++)	{
			SFVec3f pos;

			// Position of vertex --> pos
			if (bHasCoordIndex)	{
				int ind = coordIndex->getValue(i);
				if (ind < 0) continue;
				pos = coords->getValue(ind);
			}
			else {
				pos = coords->getValue(i);
			}

			vecPos.push_back(pos);
		}
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// Core part of calculation of simplified shape
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

// ------------------------------------------
// Sphere
// ------------------------------------------

CSimplifiedSphere *CSimplifiedShapeFactory::calcSphere(std::vector<SFVec3f>& vecPos)
{
	CSimplifiedSphere *retValue = new CSimplifiedSphere();
	if (!retValue) return NULL;

	int n = (int)(vecPos.size());
	if (n <= 0) return NULL;

	// Calculate the center position
	float gx = 0;
	float gy = 0;
	float gz = 0;

	float maxx, maxy, maxz ,minx, miny, minz;
	for (int i=0; i<n; i++) {
		SFVec3f pos = vecPos[i];
		float x = pos.x();
		float y = pos.y();
		float z = pos.z();

		if(i == 0) {
			minx = x;	    maxx = x;	    
			miny = y;	    maxy = y;
			minz = z;	    maxz = z;
		}

		if     (maxx < x) maxx = x;
		else if(x < minx) minx = x; 
		if     (maxy < y) maxy = y;
		else if(y < miny) miny = y; 
		if     (maxz < z) maxz = z;
		else if(z < minz) minz = z; 
    }

	gx = (minx + maxx) / 2;
	gy = (miny + maxy) / 2;
	gz = (minz + maxz) / 2;

	/*
	  for (int i=0; i<n; i++) {
	    SFVec3f pos = vecPos[i];
	    gx += pos.x();
	    gy += pos.y();
	    gz += pos.z();
	  }
	  gx /= n;
	  gy /= n;
	  gz /= n;
	*/

	// Calculate distance between center of mass and each point
	float fr = 0;  // The furthest distance
	float nr = 0;  // The nearest  distance
	float ar = 0;  // The average  distance
	bool bFirst = true;

	for (int i=0; i<n; i++) {
		
		SFVec3f pos = vecPos[i];

		if (bFirst)	{
			fr = nr = sqrDist(gx, gy, gz, pos.x(), pos.y(), pos.z());
			bFirst = false;
		}
		else {
			float d = sqrDist(gx, gy, gz, pos.x(), pos.y(), pos.z());
			if (d > fr) fr = d;
			if (d < nr) nr = d;
		}
    }

	fr = sqrt(fr);
	nr = sqrt(nr);
	ar = (nr + fr)/2;

	// Calculate degree of distortion against radius r
	// degree of distortion = sum of gap^2 between (distance between each point and CoM)=pr and r
	// If all of the points are on the ideal sphere, the degree will be zero.
	// If the shape of the points differs from ideal sphere, the degree will be increased.
	float hizumi = (float) CSimplifiedShapeFactory::calcHizumiWithSphere(ar, gx, gy, gz, vecPos);

	// Set the return value
	retValue->x(gx);
	retValue->y(gy);
	retValue->z(gz);
	retValue->radius(ar);
	retValue->hizumi(hizumi);

	return retValue;
}


double CSimplifiedShapeFactory::calcHizumiWithSphere(float r, float gx, float gy, float gz, std::vector<SFVec3f>& vecPos)
{
	int n = (int)(vecPos.size());
	if (n>0) {
		double hizumiSum = 0;

		for (int i=0; i<n; i++)	{
			SFVec3f pos = vecPos[i];

			// Distance between CoM and pos
			float d = sqrt(sqrDist(gx, gy, gz, pos.x(), pos.y(), pos.z()));

			// Sum of squares of gap between radius and the above distance
			hizumiSum = hizumiSum + (d-r)*(d-r);
		}
		double hizumi = hizumiSum/n;

		// TODO: This algorithm output the distortion zero when the vertex shape just a cube
		// degree of distortion should be calculated by not by vertex position, but volume
		// The degree of distortion will be smaller when the number of vertex is smaller
		//
		// Therefore, if the number of vertex is smaller than threshold (=30),
		// the distortion value gained difference between the volume of sphere and an inscribed cube
		if(n <= 30)
			hizumi += 2.0/3.0*r*r*r*(2.0*M_PI - 4.0/sqrt(3.0));
		return hizumi;
	}
	else
	{
		return 0;
	}
}


// ------------------------------------------
// Calculation for Cylinder type
//
// Caution: coordinate of faceSet is:
// x-z plane for bottom
// y axis for height direction of the cylinder
// It's different from SIGVerse coordinate
// ------------------------------------------

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinder(std::vector<SFVec3f>& vecPos)
{
	CSimplifiedCylinder *retValue = new CSimplifiedCylinder();
	if (!retValue) return NULL;

	int n = (int)(vecPos.size());
	if (n <= 0) return NULL;

	// Calculate the center position
	float gx = 0;
	float gy = 0;
	float gz = 0;

	for (int i=0; i<n; i++)	{
		SFVec3f pos = vecPos[i];
		gx += pos.x();
		gy += pos.y();
		gz += pos.z();
	}
	gx /= n;
	gy /= n;
	gz /= n;

	// Init of variables before calculation of fr, nr and ar
	float fr = 0;  // The furthest distance between center axis and each point
	float nr = 0;  // The nearest  distance between center axis and each point
	float ar = 0;  // Radius of the cylinder
	bool bFirst = true;

	for (int i=0; i<n; i++)	{
		SFVec3f pos = vecPos[i];

		if (bFirst)	{
			fr = nr = sqrDist(gx, 0, gz, pos.x(), 0, pos.z());
			bFirst = false;
		}
		else {
			float d = sqrDist(gx, 0, gz, pos.x(), 0, pos.z());

			if (d > fr) fr = d;
			if (d < nr) nr = d;
		}
	}

	fr = sqrt(fr);
	nr = sqrt(nr);
	ar = (nr + fr)/2;

	// Height of cylinder = max of y - min of y
	float maxy, miny;
	bFirst = true;
	for (int i=0; i<n; i++)	{
		SFVec3f pos = vecPos[i];

		if (bFirst)	{
			maxy = miny = pos.y();
			bFirst = false;
		}
		else {
			float d = pos.y();

			if (d > maxy) maxy = d;
			if (d < miny) miny = d;
		}
	}

	//test
	//gy = (maxy + miny)/2 ;
	//gy = -100;
	// Calculation of distortion value
	float hizumi = (float)calcHizumiWithCylinder(ar, gx, gz, vecPos);

	// Set in return value
	retValue->x(gx);
	retValue->y(gy);
	retValue->z(gz);
	retValue->radius(ar);
	retValue->height(maxy - miny);
	retValue->hizumi(hizumi);

	return retValue;
}


CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinder(CX3DCylinderNode *pCylinderNode)
{
//	int i;
	printf("\t\t[Start calcCylinder]\n");

	if (!pCylinderNode) return NULL;

	CSimplifiedCylinder *retValue = new CSimplifiedCylinder();
	if (!retValue) return NULL;

	// Initialize the center of mass as origin
	float gx,gy,gz;
	gx = 0.0f;
	gy = 0.0f;
	gz = 0.0f;

	float radius = pCylinderNode->getRadius()->getValue();
	float height = pCylinderNode->getHeight()->getValue();
	// Initialize the distortion as zero
	float hizumi = 0.0f;

	// Set return value
	retValue->x(gx);
	retValue->y(gy);
	retValue->z(gz);
	retValue->radius(radius);
	retValue->height(height);
	retValue->hizumi(hizumi);

	return retValue;
}


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
bool CSimplifiedShapeFactory::isCylinder(
		CX3DIndexedFaceSetNode *pNodeData,
		float &radiusEstimated,
		float &heightEstimated,
		float xAxis[],
		float yAxis[],
		float zAxis[]
	)
{
	int i, n;
	bool result = false;

	if (!pNodeData) return false;

	CX3DCoordinateNode *pCoord = (CX3DCoordinateNode *)(pNodeData->getCoord()->getNode());

	if (!pCoord) return false;

	// Position of vertex
	MFVec3f *coords = pCoord->getPoint();

	// Position of CoM
	float gx,gy,gz;
	// The number of vertex
	n = coords->count();

	// If the number of vertex is less, it should be regarded as cube
	if (n < 14) return false; // TODO: Magic number

	// Calculate CoM
	getCenter(coords,gx,gy,gz);	

	//----------------------------------------
	// Calculation of axis of cylinder
	// If it is cylinder, the axis should consist of the two nearest points from CoM (by MSI)
	// However, it is wrong! (by Tetsunari Inamura); TODO
	//----------------------------------------
	float x_a[2],y_a[2],z_a[2];	 // The two nearest points from CoM
	getNearPoint(coords,gx,gy,gz,x_a,y_a,z_a,2);

	// Calculate from axis
	// nearer one of (x[0],y[0],z[0]) / (x[1],y[1],z[1]) should be the axis (by MSI) <-- Questionable by Tesunari Inamura
	float distance = 0.0f;
	int vertex = 0;	  // The number of vertex excepting axis

	for (i=0; i<n; i++) {
		SFVec3f pos;
		// Position of vertex -> pos
		pos = coords->getValue(i);

		float d1,d2,d;

		d1 = getDistance(pos.x(),pos.y(),pos.z(),x_a[0],y_a[0],z_a[0]);
		d2 = getDistance(pos.x(),pos.y(),pos.z(),x_a[1],y_a[1],z_a[1]);
		d = ( (d1 > d2) ? d2 : d1 );

		distance += d;

		if(d > SMALL_VALUE) {
			vertex++;
		}
	}

	// Rough distance value from CoM
	if(vertex > 0) {
		distance /= (float)vertex;
	}

	// Estimated radius of the cylinder
	float radius = distance;
	result = true;
	for (i=0; i<n; i++) {
		SFVec3f pos;
		// Position of vertex -> pos
		pos = coords->getValue(i);

		float d1,d2,d;

		d1 = getDistance(pos.x(),pos.y(),pos.z(),x_a[0],y_a[0],z_a[0]);
		d2 = getDistance(pos.x(),pos.y(),pos.z(),x_a[1],y_a[1],z_a[1]);
		d = ( (d1 > d2) ? d2 : d1 );

		// If the value quite differs from radius, it should not be cylinder
		// (Points on the axis itself should be removed from the judgement)
		if(d > SMALL_VALUE && (d - radius > SLIGHT_SMALL_VALUE)) {
			return false;
		}
	}

	// It is regarded as cylinder
	radiusEstimated = radius;
	heightEstimated = getDistance(x_a[0],y_a[0],z_a[0],x_a[1],y_a[1],z_a[1]);

	xAxis[0] = x_a[0];
	xAxis[1] = x_a[1];
	yAxis[0] = y_a[0];
	yAxis[1] = y_a[1];
	zAxis[0] = z_a[0];
	zAxis[1] = z_a[1];

	return result;
}


double CSimplifiedShapeFactory::calcHizumiWithCylinder(float r, float cx, float cz, std::vector<SFVec3f>& vecPos)
{
	int n = (int)(vecPos.size());

	if (n>0) {
		double hizumiSum = 0;

		for (int i=0; i<n; i++)	{
			SFVec3f pos = vecPos[i];

			// Distance between axis of cylinder and pos
			float d = sqrt(sqrDist(cx, 0, cz, pos.x(), 0, pos.z()));

			// sum of squares of gap between radius and the above distance
			hizumiSum = hizumiSum + (d-r)*(d-r);
		}
		// TODO: This algorithm output the distortion zero when the vertex shape just a cube
		// Distortion value should be calculated by not by vertex position, but volume
		//
		// Therefore, if the number of vertex is smaller than threshold (=30),
		// the distortion value gained difference between the volume of an ideal cylinder and an inscribed cube
		double hizumi = hizumiSum/n;
		if(n <= 30)
#ifndef WIN32
			hizumi += r*r*r*(1/sqrt(2)*M_PI - 8/3/sqrt(3));
#else
			hizumi += r*r*r*(1/sqrtf(2)*M_PI - 8/3/sqrtf(3));
#endif
		return hizumi;
	}
	else
		return 0.0;
}


// ------------------------------------------
// Box type
// ------------------------------------------
CSimplifiedBox *CSimplifiedShapeFactory::calcBox(std::vector<SFVec3f>& vecPos)
{
	CSimplifiedBox *retValue = new CSimplifiedBox();
	if (!retValue) return NULL;

	int n = (int)(vecPos.size());
	if (n <= 0) return NULL;

	// Calculate max/min value of each axis (x,y,z)
	// A position with min value -> (x1, y1, z1)
	// A position with max value -> (x2, y2, z2)
	// Then, a cube which has a diagonal line (x1, y1, z1)-(x2, y2, z2) should be bounding box
	float x1, y1, z1, x2, y2, z2;

	bool bFirst = true;
	for (int i=0; i<n; i++)	{
		SFVec3f pos = vecPos[i];

		if (bFirst)	{
			x1 = x2 = pos.x();
			y1 = y2 = pos.y();
			z1 = z2 = pos.z();

			bFirst = false;
		}
		else {
			float x = pos.x();
			float y = pos.y();
			float z = pos.z();

			if (x < x1) x1 = x;
			if (x > x2) x2 = x;

			if (y < y1) y1 = y;
			if (y > y2) y2 = y;

			if (z < z1) z1 = z;
			if (z > z2) z2 = z;
		}
	}

	float hizumi = (float)calcHizumiWithBox(x1, y1, z1, x2, y2, z2, vecPos);

	// The following lines are remained to deal with old version
	retValue->x1(x1);retValue->x2(x2);
	retValue->y1(y1);retValue->y2(y2);
	retValue->z1(z1);retValue->z2(z2);

	// Calculate length of a edge with subtract min from max
	retValue->sx(x2-x1);
	retValue->sy(y2-y1);
	retValue->sz(z2-z1);

	// return value should be an intermedeate point between max and min
	retValue->x((x1+x2)/2);
	retValue->y((y1+y2)/2);
	retValue->z((z1+z2)/2);
	
	retValue->hizumi(hizumi);

	return retValue;
}


double CSimplifiedShapeFactory::calcHizumiWithBox(float x1, float y1, float z1, float x2, float y2, float z2, std::vector<SFVec3f>& vecPos)
{
	float dist[6];
	float d;

	int n = (int)(vecPos.size());
	if (n>0) {
		double hizumiSum = 0;

		for (int i=0; i<n; i++)	{
			SFVec3f pos = vecPos[i];

			// Distance between six planes of bounding box
			dist[0] = pos.x() - x1;
			dist[1] = x2 - pos.x();
			dist[2] = pos.y() - y1;
			dist[3] = y2 - pos.y();
			dist[4] = pos.z() - z1;
			dist[5] = z2 - pos.z();

			// Calculate minimum distance from plane
			d = min6Float(dist);

			hizumiSum = hizumiSum + d*d;
		}

		return hizumiSum/n;
	}
	else
	{
		return 0;
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
// helper function
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

// Find the minimum value from six float values
// six values should be stored in f[0]--f[5]
float CSimplifiedShapeFactory::min6Float(float *f)
{
	float minValue;

	for (int i=0; i<6; i++)	{
		float x = f[i];

		if (i==0) {
			minValue = x;
		}
		else {
			if (x < minValue) minValue = x;
		}
	}
	return minValue;
}


float CSimplifiedShapeFactory::sqrDist(float px, float py, float pz, float qx, float qy, float qz)
{
	return (px-qx)*(px-qx) + (py-qy)*(py-qy) + (pz-qz)*(pz-qz);
}

float CSimplifiedShapeFactory::getDistance(float px, float py, float pz, float qx, float qy, float qz){
	return (float)sqrt(sqrDist(px,py,pz,qx,qy,qz));
}


/**
 * Calculate center of mass (CoM)
 * @param coords position of vertex
 * @param gx  x of CoM should be stored
 * @param gy  y of CoM should be stored
 * @param gz  z of CoM should be stored
 */
void CSimplifiedShapeFactory::getCenter(MFVec3f *coords,float &gx,float &gy,float &gz)
{
	int i, n;
	// The number of vertex
	n = coords->count();
	gx = 0.0f;
	gy = 0.0f;
	gz = 0.0f;

	bool bFirst = true;
	for (i=0; i<n; i++)	{
		SFVec3f pos;
		// Position of vertex --> pos
		pos = coords->getValue(i);

		gx += pos.x();
		gy += pos.y();
		gz += pos.z();
	}

	if(n > 0) {
		gx /= (float)n;
		gy /= (float)n;
		gz /= (float)n;
	}
	//printf("gx = %f,gy = %f,gz = %f\n",gx,gy,gz);

}


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
void CSimplifiedShapeFactory::getNearPoint(MFVec3f *coords,float gx,float gy,float gz,float destX[],float destY[],float destZ[],int num)
{
	// Calculation of axis of cylinder
	// If it is cylinder, the axis should consist of the two nearest points from CoM (by MSI)
	// However, it is wrong! (by Tetsunari Inamura); TODO
	float distMin = INVALID_VALUE;

	for(int n=0;n<num;n++) {
		destX[n] = INVALID_VALUE;
		destY[n] = INVALID_VALUE;
		destZ[n] = INVALID_VALUE;
	}

	for (int i=0; i < coords->count() ; i++) {
		SFVec3f pos;
		pos = coords->getValue(i);

		float d = getDistance(pos.x(),pos.y(),pos.z(),gx,gy,gz);
		if( distMin >= d ) {
			distMin = d;
			for(int n=num-1;n>0;n--) {
				destX[n] = destX[n-1];
				destY[n] = destY[n-1];
				destZ[n] = destZ[n-1];
			}
			destX[0] = pos.x();
			destY[0] = pos.y();
			destZ[0] = pos.z();
		}
	}
}




// Since the above cylinder calculation does not work well, this function 
// was added by okamoto on 2012-12-21
CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinder2(std::vector<SFVec3f>& vecPos)
{
	CSimplifiedCylinder *retValue = new CSimplifiedCylinder();
	if (!retValue) return NULL;

	int n = (int)(vecPos.size());
	if (n <= 0) return NULL;

	// Center of Mass
	float gx = 0;
	float gy = 0;
	float gz = 0;

	float maxx, maxy, maxz ,minx, miny, minz;
	for (int i=0; i<n; i++)
		{
			SFVec3f pos = vecPos[i];
			float x = pos.x();
			float y = pos.y();
			float z = pos.z();

			if(i == 0){
				minx = x;	    maxx = x;	    
				miny = y;	    maxy = y;
				minz = z;	    maxz = z;
			}

			if     (maxx < x) maxx = x;
			else if(x < minx) minx = x; 
			if     (maxy < y) maxy = y;
			else if(y < miny) miny = y; 
			if     (maxz < z) maxz = z;
			else if(z < minz) minz = z; 
		}
	
	gx = (minx + maxx) / 2;
	gy = (miny + maxy) / 2;
	gz = (minz + maxz) / 2;

	// The length of axis is difference between max and min
	float length = maxy - miny;

	float mxz = 0.0;
  
	for (int i = 0; i < n; i++) {
		SFVec3f pos = vecPos[i];
		// Vector from the center
		float dx = pos.x() - gx;
		float dz = pos.z() - gz;

		// Distance from y axis
		mxz += sqrt(dx*dx+dz*dz);
	}

	// Let the average of distance from y axis be the radius
	mxz /= n;
	float radius = mxz;

	double hizumiSum = 0.0;

	// Distortion calculation
	for (int i = 0; i < n; i++) {
		SFVec3f pos = vecPos[i];
    
		// Vector from the CoM
		float dx = pos.x() - gx;
		float dy = pos.y() - gy;
		float dz = pos.z() - gz;
    
		// Distance from y axis
		float dxz = sqrt(dx*dx+dz*dz);

		// Sum of squares of the difference, following to other APIs
		hizumiSum = hizumiSum + (dxz-radius)*(dxz-radius); 
	}

	double hizumi = hizumiSum / n;

	// Set return value
	retValue->x(gx);
	retValue->y(gy);
	retValue->z(gz);
	retValue->radius(radius);
	retValue->height(length);
	retValue->hizumi((float)hizumi);
  
	return retValue;
}


