/*
 * Modified by MSI on 2011-05-12
 */

#ifndef SSimObjBuilder_h
#define SSimObjBuilder_h

#include <sigverse/simserver/readconf/BodyFactory.h>
#include <sigverse/commonlib/Vector3d.h>

class SSimObj;
class ODEWorld;
class Joint;
class SParts;

class SSimObjBuilder : public BodyFactory
{
private:
	SSimObj  &m_obj;
	ODEWorld &m_world;
	Vector3d  m_scale;
public:
	SSimObjBuilder(SSimObj &o, ODEWorld &w);
	ODEWorld & getODEWorld(){return m_world;}
private:
	bool addJoint(Joint *j);

	bool addParts(SParts *parts);

	void connect(Joint *joint, SParts *p1, SParts *p2);

	void connect2(Joint *sibJ, Joint *childJ, SParts *sibP, SParts *childP);

	SSimObj & getSObj() { return m_obj; }

	Vector3d & applyScaleV(Vector3d &);
	Position & applyScaleP(Position &);
	Size & applyScaleS(Size &);
};

#endif // SSimObjBuilder_h


