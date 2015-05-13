#ifndef SSimEntity_h
#define SSimEntity_h

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <ode/ode.h>
#include "Vector3d.h"
#include "Logger.h"

//! Object parts that have more than one ODE parts.
//! Not Robot entity (that doesn't have joint) have only one this structure.
struct SSimObjParts
{
	double               mass;   // parts number.
	std::vector<dGeomID> geoms;  // all geometries
	dBodyID              body;   // body ID
	Vector3d             pos;    // gap from SIGVerse position.(ode position is center of gravity.)
};

//! Robot parts
struct SSimRobotParts
{
	SSimObjParts objParts;
	std::string  name;
	bool         root; // parts attendant on root, or not.
	Vector3d     com;  // center of gravity position from Joint position
};

//Joint
struct SSimJoint
{
	std::string     name;
	dJointID        joint;
	std::string     parent_joint;
	unsigned int    ID;
	Vector3d        posFromParent; // initial gap from parent joints
	Vector3d        posFromRoot;   // initial gap from root joints
	Vector3d        iniAxis;       // initial axis of rotation
	bool            has_geom;
	//dBodyID         body;  //dummyBody
	SSimRobotParts  robotParts;
	bool            isRoot;
};


/**
 * @brief Entity class
 */
class SSimEntity
{
public:
	SSimEntity(dWorldID world, dSpaceID space, const char *name)
		:m_name(name),
		 m_world(world),
		 m_space(space),
		 m_agent(false),
		 m_robot(false),
		 m_collision(false),
		 m_ID(-1),
		 m_qw(1.0),
		 m_qx(0.0),
		 m_qy(0.0),
		 m_qz(0.0)
	{
		m_parts.mass    = 1.0;
		m_parts.body    = false;
		m_parts.pos.set(0.0, 0.0, 0.0);
		m_scale.set(1.0, 1.0, 1.0);
	}

	//! get ODE world
	dWorldID getWorld(){return m_world;}

	//! get ODESpace
	dSpaceID getSpace(){return m_space;}

	std::string name(){return m_name;}

	void setIsAgent(bool flag){m_agent = flag;}
	bool isAgent(){return m_agent;}

	void setScale(Vector3d vec);
	Vector3d getScale(){return m_scale;}

	void setIsRobot(bool flag){m_robot = flag;} // Robot have Joints
	bool isRobot(){return m_robot;}

	void setInitPosition(Vector3d pos);
	void getInitPosition(Vector3d &pos){pos.set(m_inipos.x(), m_inipos.y(), m_inipos.z());}

	void setMass(double mass);
	double getMass(){return m_parts.mass;}

	//! get Geometry number
	int getGeomNum(){return m_parts.geoms.size();}

	int getPartsType(int n){return dGeomGetClass(m_parts.geoms[n]);}

	//! get Geometry of n-th parts.
	dGeomID getPartsGeom(int n){return m_parts.geoms[n];}

	Vector3d getCenterOfMass(){return m_parts.pos;}

	//! get body
	dBodyID getPartsBody(){return m_parts.body;}

	//! set body
	void setBody(dBodyID body){m_parts.body = body;}

	//! add geometry (set gap from body)
	void addGeom(dGeomID geom);

	//! do collision detection, or not.
	void setCollision(bool col);

	//! set shape file name
	void setShapeFile(std::string shape){m_x3dfile = shape;}

	//! get Entity position
	//! if no change from before, return false.
	//! if 'pre' is true, update motion detection.
	bool getPosition(Vector3d &pos, bool pre = false);

	//! get Entity rotation
	//! if no change from before, return false.
	bool getQuaternion(dQuaternion q, bool pre = false);

	//! get shape file name
	std::string getShapeFile(){return m_x3dfile;}

	//! set Entity ID
	void setID(int id){m_ID = id;}
	//! get Entity ID
	int  getID(){return m_ID;}

	//! get parts
	SSimObjParts getObjParts(){return m_parts;}

	//! initialize parts
	void initObjParts()
	{
		m_parts.mass    = 1.0;
		m_parts.body    = false;
		m_parts.pos.set(0.0, 0.0, 0.0);
		m_parts.geoms.clear();
	}

protected:
	dWorldID      m_world;  // ODEWorld
	dSpaceID      m_space;  // ODESpace
	int           m_ID;     // Entity ID
	std::string   m_name;   // entity name
	bool          m_agent;  // agent flag
	Vector3d      m_scale;  // shape scale
	Vector3d      m_inipos; // init position
	bool          m_robot;  // robot flag

	bool          m_collision; // collision detection flag
	std::string   m_x3dfile;   // shape file name
	dReal         m_px, m_py, m_pz; // position x, y, z
	dReal         m_qw, m_qx, m_qy, m_qz; // quaternion w, x, y, z
	SSimObjParts  m_parts;     // include all ode parts
};

// Robot Entity class
class SSimRobotEntity : public SSimEntity
{
public:
	SSimRobotEntity(dWorldID world, dSpaceID space, const char *name)
		: SSimEntity(world, space, name)
	{
	};

	void setRootBody(dBodyID body){m_rootBody = body;}

	dBodyID getRootBody(){return m_rootBody;}

	//void addParts(const char *name);
	///void addParts(std::vector<SSimJoint> joint, const char *parent_name, const char *name);

	void addJoint(SSimJoint *joint);

	//! get parts number
	int getPartsNum(){return m_allParts.size();}

	//! do collision detection, or not.
	void setCollision(bool col);

	//! get Joint
	SSimJoint *getJoint(std::string jname)
	{
		std::vector<SSimJoint*>::iterator it;
		it = m_allJoints.begin();

		while(it != m_allJoints.end()){
			if((*it)->name == jname){
				return (*it);
			}
			it++;
		}
		LOG_ERR(("cannot find joint %d",jname.c_str()));
	}

	void setMass(SSimObjParts *parts, double mass);
	void setInitPosition(Vector3d pos);
	std::vector<SSimJoint*> getAllJoints(){return m_allJoints;}


private:
	std::vector<SSimRobotParts> m_allParts; // all parts that have geometry
	std::vector<SSimJoint*> m_allJoints;
	dBodyID   m_rootBody;
	int       m_jointNum;
};
#endif
