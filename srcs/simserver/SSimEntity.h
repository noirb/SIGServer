#ifndef SSimEntity_h
#define SSimEntity_h

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <ode/ode.h>
#include "Vector3d.h"
#include "Logger.h"

//struct SSimRobotParts;

//! Parts of object which has multiple ODE parts
//! An entity, which does not have any joints (it means it is not robot), should have only one element of this part
struct SSimObjParts
{
	// number of parts
	double                mass;
	std::vector<dGeomID> geoms;  // All of the geometry
	dBodyID               body;
	// Position of the parts in ODE should be center of gravity
	// Gap between position in SIGVerse and position in ODE
	Vector3d               pos;    
};


//! Parts that should be had by a robot, which has joints
struct SSimRobotParts
{
	// Self parts
	SSimObjParts objParts;

	// Name of the self parts
	std::string  name;

	// Whether this part belongs to root joint
	bool         root;

	// Displacement of CoG from Joint position
	Vector3d      com;
};


//! Joint
struct SSimJoint
{
	// Name of self joint
	std::string     name;

	// Self joint
	dJointID        joint;
  
	// Name of parent's joint
	std::string     parent_joint;

	unsigned int    ID;

	// Gap from parent's joint (initial value)
	Vector3d        posFromParent;

	// Gap from root joint (initial value)
	Vector3d        posFromRoot;

	// rotational axis at the initial(?) moment  (original comments was vague)
	Vector3d        iniAxis;

	// Whether the joint has geometry or not (dummy or not)
	bool            has_geom;

	// Dummy body when the joint does not have geometry
	//dBodyID         body;

	// Robot part belongs to the joint
	SSimRobotParts  robotParts;

	// Whether root joint or not
	bool            isRoot;
};



/**
 * @brief Entity class
 *
 * Attribution and status of each entity is managed
 */
class SSimEntity
{
public:	SSimEntity(dWorldID world, dSpaceID space, const char *name)
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
  
	//! Reference of ODE world
	dWorldID getWorld() {return m_world;}

	//! Reference of ODESpace
	dSpaceID getSpace() {return m_space;}

	//! Reference of name
	std::string name() {return m_name;}

	//! Set whether agent or not
	void setIsAgent(bool flag) {m_agent = flag;}

	//! Reference of agent flag
	bool isAgent() {return m_agent;}

	//! Set of scale
	void setScale(Vector3d vec);

	//! Reference of scaleを
	Vector3d getScale() {return m_scale;}   
  
	//! Set whether this is robot, which has joints, or not
	void setIsRobot(bool flag) {m_robot = flag;}
  
	//! Reference of robot flag
	bool isRobot() {return m_robot;}
  
	//! Set of initial position
	void setInitPosition(Vector3d pos);

	//! Reference of initial position
	void getInitPosition(Vector3d &pos) {pos.set(m_inipos.x(), m_inipos.y(), m_inipos.z());}
  
	//! Set of mass
	void setMass(double mass);

	//! Reference of mass
	double getMass() {return m_parts.mass;}

	//! Reference of number of Geometries
	int getGeomNum() {return m_parts.geoms.size();}
  
	//! Reference of type of part
	int getPartsType(int n) {return dGeomGetClass(m_parts.geoms[n]);}
  
	//! Reference of n-th part's geometry
	dGeomID getPartsGeom(int n) {return m_parts.geoms[n];}
  
	//! Reference of CoG (in a coordinate of whole entity
	Vector3d getCenterOfMass() {return m_parts.pos;}
  
	//! reference of body
	dBodyID getPartsBody() {return m_parts.body;}
  
	//! settnig of body
	void setBody(dBodyID body) {m_parts.body = body;}

	//! Set of geometry // vague comments in Japanese -> (pos)でbodyとのずれを設定 <- What's this?
	void addGeom(dGeomID geom);

	//! Set of collision flag
	void setCollision(bool col);

	//! Set of shape file name
	void setShapeFile(std::string shape) {m_x3dfile = shape;}

	//! Reference of position of entity
	//! Returns false if the value is the same as previous value
	//! If pre is true, update parameters for movement judgement
	bool getPosition(Vector3d &pos, bool pre = false);

	//! Reference of orientation of entity
	//! Returns false if the value is the same as previous value
	//! If pre is true, update parameters for rotation judgement
	bool getQuaternion(dQuaternion q, bool pre = false);

	//! Reference of shape file name
	std::string getShapeFile(){return m_x3dfile;}

	//! Set of Entity ID
	void setID(int id) {m_ID = id;}

	//! Reference of Entity ID
	int  getID() {return m_ID;}

	//! Reference of Parts
	SSimObjParts getObjParts() {return m_parts;}

	//! Initialization of parts
	void initObjParts(){
		m_parts.mass    = 1.0;
		m_parts.body    = false;
		m_parts.pos.set(0.0, 0.0, 0.0);
		m_parts.geoms.clear();
	}    

 protected:
	dWorldID      m_world;     // ODEWorld
	dSpaceID      m_space;     // ODESpace
	int           m_ID;        // Entity ID
	std::string   m_name;      // entity name
	bool          m_agent;     // agent flag
	Vector3d      m_scale;     // shape scale
	Vector3d      m_inipos;    // init position
	bool          m_robot;     // robot flag

	bool          m_collision; // Whether collision detection should be executed
	std::string   m_x3dfile;   // shape file name
	dReal         m_px, m_py, m_pz;       // parameters for movement judgement
	dReal         m_qw, m_qx, m_qy, m_qz; // parameters for rotation judgement
	SSimObjParts  m_parts;     // include all ode parts
};


// Robot class which has joints
class SSimRobotEntity : public SSimEntity
{
public:
	SSimRobotEntity(dWorldID world, dSpaceID space, const char *name) 
		: SSimEntity(world, space, name)
	{
	};

	void setRootBody(dBodyID body){m_rootBody = body;}

	dBodyID getRootBody(){return m_rootBody;}

	//! Add a part
	//void addParts(const char *name);
  
	//! Add a part and connect by joint
	///void addParts(std::vector<SSimJoint> joint, const char *parent_name, const char *name);
	void addJoint(SSimJoint *joint);

	//! Reference of number of parts
	int getPartsNum(){return m_allParts.size();}

	//! Set of collision flag
	void setCollision(bool col);

	//! Reference of Joint
	SSimJoint *getJoint(std::string jname){
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

	//! Set of mass
	void setMass(SSimObjParts *parts, double mass);

	void setInitPosition(Vector3d pos);
	std::vector<SSimJoint*> getAllJoints(){return m_allJoints;}


	/*
	  void setJointNum(int num){m_jointNum = num;}
	  int  getJointNum(){return m_jointNum;}
	*/

private:
	std::vector<SSimRobotParts> m_allParts; // whole parts which has geomtry
	std::vector<SSimJoint*> m_allJoints;
	dBodyID   m_rootBody;
	int       m_jointNum;
};
#endif
