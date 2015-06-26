/*
 * Written by okamoto on 2011-12-28
 * Modified by Tetsunari Inamura on 2013-12-29
 *   Add English comments (translation is not finished yet)
 * Modified by Tetsunari Inamura on 2013-12-30
 *   change addJointVelocity to setAngularVelocityToJoint
 */

#ifndef SSimObj_h
#define SSimObj_h

#include "systemdef.h"
#include "SimObjBase.h"
#include "SParts.h"
#include "Vector3d.h"
#include "Rotation.h"

class Joint;

struct Camera
{
	Camera(std::string l, Vector3d p, Vector3d d,double a, double f) : link(l), pos(p), dir(d), aspectRatio(a), FOV(f)
	{}

	std::string link;
	Vector3d pos;
	Vector3d dir;
	double aspectRatio;   // aspect ratio
	double FOV;           // field of view
};


class SSimObj : public SimObjBase
{
private:
	typedef SimObjBase Super;
public:
	typedef std::map<std::string, SParts*> PartsM;
	typedef std::map<std::string, Joint*>  JointM;
private:
	PartsM                m_parts;
	JointM                m_joints;
	dSpaceID              m_space;
	bool                  m_gmode;
	Vector3d              m_vpos;
	Rotation              m_vrot;
	std::map<int, Camera> m_vcams;
	std::string           m_currShape;
	int                   m_ID;
	std::map<std::string, double> m_jvel;
	bool                  m_grasped;
	
protected:
	int                   m_type;
public:
	SSimObj(dSpaceID parent);
	virtual ~SSimObj();

	dSpaceID space()
	{
		assert(m_space);
		return m_space;
	}

	void setAttached(bool b)
	{
		m_attached = b;
	}

	void addId();
	void push(Joint *p);

	void addCamera(int id, Camera cam)
	{
		m_vcams.insert( std::map<int, Camera>::value_type(id, cam));
	}

	void setCamera(int id, Camera cam)
	{
		std::map<int, Camera>::iterator it;
		it = m_vcams.find(id);

		if(it != m_vcams.end()) {
			(*it).second = cam;
		}
		else {
			LOG_ERR(("Cannot find camera id [%d]", id));
		}
	}

	std::map<int, Camera> getCamera()
	{
		return m_vcams;
	}

	void setShape(std::string shape)
	{
		m_currShape = shape;
	}

	std::string getShape()
	{
		return m_currShape;
	}

	void setIsGrasped(bool grasp){m_grasped = grasp;}

	bool getIsGrasped(){return m_grasped;}

	void push(SParts *p)
	{
		m_parts[std::string(p->name())] = p;
		p->setParent(this);
	}

	void push(Attribute *attr)
	{
		Super::push(attr);
	}

	SParts * getSBody() { return getSParts("body"); }
	Parts  * getBody()  { return getSParts("body"); }

	SParts* getSParts(const char *name)
	{
		PartsM::iterator i = m_parts.find(name);

		return i != m_parts.end()? i->second: NULL;
	}

	/**
	 * Get SParts data from dGeomID
	 */
//	SParts* getSPartsFromODE_dGeomID(dGeomID geomId) {
//
//		PartsM::iterator i = m_parts.begin();
//	}

	// Set parameters to ODE Object
	void applyParams(bool init, double step);
	// Load parameters from ODE Object
	void loadParams();

	int getJointSize();

	PartsM getAllParts()
	{
		return m_parts;
	}

	void setGMode(bool g){ m_gmode = g;}
	bool getGMode(){return m_gmode;}

	void setVPosition(Vector3d pos)
	{
		m_vpos.x(pos.x());
		m_vpos.y(pos.y());
		m_vpos.z(pos.z());
	}

	void setVPosition(double x, double y, double z)
	{
		m_vpos.x(x);
		m_vpos.y(y);
		m_vpos.z(z);
	}

	void getVPosition(Vector3d *pos)
	{
		pos->x(m_vpos.x());
		pos->y(m_vpos.y());
		pos->z(m_vpos.z());
	}

	void setVRotation(Rotation rot)
	{
		m_vrot.setQuaternion(rot.qw(), rot.qx(), rot.qy(), rot.qz());
	}

	void setVRotation(double qw, double qx, double qy, double qz)
	{
		m_vrot.setQuaternion(qw, qx, qy, qz);
	}

	void getVRotation(Rotation *rot)
	{
		rot->setQuaternion(m_vrot.qw(), m_vrot.qx(), m_vrot.qy(), m_vrot.qz());
	}

	JointM* getJointMap()
	{
		return &m_joints;
	}

	Joint * getJoint(const char *s);

	bool removeJoint(Joint *j);

	//! Set angular velocity in dynamics mode on
	// modified by inamura: changed from addJointVelocity
	void setAngularVelocityToJoint(std::string jname, double vel)
	{
		typedef std::map<std::string, double> JMAP;
		JMAP::iterator it = m_jvel.find(jname);
		
		// if the target joint name is existed
		if(it != m_jvel.end()) {
			// TODO: why if is required? by inamura, not so important
			if(vel == 0.0)
				m_jvel.erase(it);
			else
				(*it).second = vel;
		}
		// if the target joint name is not regisered
		else {
			m_jvel.insert(JMAP::value_type(jname, vel));
		}
	}

#ifdef _DUBUG
	void dump();
#else
	void dump() {}
#endif
 private:
	class Iterator : public PartsIterator
	{
	private:
		std::map<std::string, SParts*> &m_map;
		std::map<std::string, SParts*>::iterator m_i;
	public:
		Iterator(std::map<std::string, SParts*> &map) : m_map(map)
		{
			m_i = m_map.begin();
		}
	private:
		Parts * next() {
			if (m_i == m_map.end()) { return NULL; }
			SParts *p = m_i->second;
			m_i++;
			return p;
		}
	};

	PartsIterator * getPartsIterator() {
		return new Iterator(m_parts);
	}
private:
	static unsigned s_cnt;
public:
	static void initCounter() { s_cnt = 0; }
};

class SRobotObj : public SSimObj
{
public:
	//SRobotObj(){}
	SRobotObj(dSpaceID parent) : SSimObj(parent), m_onMove(false)
	{
		m_type = 1;
	}

	bool setWheel(double wheelRadius, double wheelDistance)
	{
		m_wheelRadius   = wheelRadius;
		m_wheelDistance = wheelDistance;
		return true;
	}

	bool setWheelVelocity(double left, double right)
	{
		m_leftWheelVel  = left;
		m_rightWheelVel = right;
		if(m_leftWheelVel != 0.0 && m_rightWheelVel != 0.0) {
			m_onMove = true;
		}
		else m_onMove = false;

		return m_onMove;
	}

	bool   getOnMove(){return m_onMove;}
	double getWheelRadius(){return m_wheelRadius;}
	double getWheelDistance(){return m_wheelDistance;}

	void   getWheelVelocity(double &left, double &right)
	{
		left  = m_leftWheelVel;
		right = m_rightWheelVel;
	}

 private:
	double m_wheelRadius;
	double m_wheelDistance;
	double m_leftWheelVel;
	double m_rightWheelVel;
	bool   m_onMove;
};


#endif // SSimObj_h


