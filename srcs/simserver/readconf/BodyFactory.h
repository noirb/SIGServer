/*
 * Modified by MSI on 2011-05-12
 */


#ifndef BodyFactory_h
#define BodyFactory_h
#include "ODEWorld.h"

class Joint;
class SParts;
class SSimObj;
class ODEWorld;

class Position;
class Vector3d;
class Size;

class BodyFactory
{
public:
	virtual ~BodyFactory() {;}

	virtual bool addJoint(Joint *) = 0;

	virtual bool addParts(SParts *) = 0;

	virtual void connect(Joint *joint, SParts *p1, SParts *p2) = 0;

	virtual void connect2(Joint *parentJ, Joint *sibJ, SParts *sibP, SParts *childP) = 0;

	virtual SSimObj & getSObj() = 0;

	virtual ODEWorld & getODEWorld() = 0;

	virtual Vector3d & applyScaleV(Vector3d &) = 0;

	virtual Position & applyScaleP(Position &) = 0;

	virtual Size & applyScaleS(Size &) = 0;
};


#endif // BodyFactory_h


