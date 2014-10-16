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

//! ODE$B%Q!<%D$rJ#?t;}$D(Bobject$B$N%Q!<%D(B
//! Robot$B$G$O$J$$(B($B4X@a$r;}$?$J$$(B)$B%(%s%F%#%F%#$O$3$l$r(B1$B$D$@$1;}$D(B
struct SSimObjParts
{
  // $B%Q!<%D$N?t(B
  double                mass;
  std::vector<dGeomID> geoms;  //$BA4%8%*%a%H%j!<(B
  dBodyID               body;
  // ODE$B$K$*$1$k%Q!<%D$N0LCV$O=E?4$N0LCV$H$J$k(B
  // SIGVerse$B$K$*$1$k0LCV$H$N$:$l(B
  Vector3d               pos;    
};

//! $B4X@a$,$"$k(BRobot$B$,;}$D%Q!<%D(B
struct SSimRobotParts
{
  // $B<+%Q!<%D(B
  SSimObjParts objParts;

  // $B<+%Q!<%D$NL>A0(B
  std::string  name;

  // root$B%8%g%$%s%H$KIUB0$9$k%Q!<%D$+$I$&$+(B
  bool         root;

  // Joint$B0LCV$+$i$N=E?4$N0LCV(B
  Vector3d      com;
};

//$B%8%g%$%s%H(B
struct SSimJoint
{
  // $B<+%8%g%$%s%HL>(B
  std::string     name;

  // $B<+%8%g%$%s%H(B
  dJointID        joint;
  
  // $B?F%8%g%$%s%H$NL>A0(B
  std::string     parent_joint;

  // ID$BHV9f(B
  unsigned int    ID;

  // $B?F%8%g%$%s%H$+$i$N$:$l(B($B=i4|(B)
  Vector3d        posFromParent;

  // root$B%8%g%$%s%H$+$i$N$:$l(B($B=i4|(B)
  Vector3d        posFromRoot;

  // $B@_Dj;~$N2sE><4(B
  Vector3d        iniAxis;

  // $B%8%*%a%H%j$r;}$D$+$I$&$+!J%@%_!<$+$I$&$+!K(B
  bool            has_geom;

  // $B%8%*%a%H%j$r;}$?$J$$>l9g$N%@%_!<%\%G%#(B
  //dBodyID         body;

  // $B%8%g%$%s%H$,;}$D%m%\%C%H%Q!<%D(B
  SSimRobotParts  robotParts;

  // root$B%8%g%$%s%H$+$I$&$+(B
  bool            isRoot;


};



/**
 * @brief $B%(%s%F%#%F%#%/%i%9(B
 *
 * $B3F%(%s%F%#%F%#$NB0@-!&>uBV$rJ];}$9$k!#(B
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
  
  //! ODE$B@$3&$r<hF@$9$k(B
  dWorldID getWorld(){return m_world;}

  //! ODESpace$B$r<hF@$9$k(B
  dSpaceID getSpace(){return m_space;}

  //! $BL>A0$r<hF@$9$k(B
  std::string name(){return m_name;}

  //! $B%(!<%8%'%s%H$+$I$&$+@_Dj$9$k(B
  void setIsAgent(bool flag){m_agent = flag;}

  //! $B%(!<%8%'%s%H$+$I$&$+<hF@$9$k(B
  bool isAgent(){return m_agent;}

  //! scale$B$r@_Dj$9$k(B
  void setScale(Vector3d vec);

  //! scale$B$r<hF@(B
  Vector3d getScale(){return m_scale;}   
  
  //! $B4X@a$r;}$D%m%\%C%H$+$I$&$+@_Dj(B
  void setIsRobot(bool flag){m_robot = flag;}
  
  //! $B4X@a$r;}$D%m%\%C%H$+$I$&$+<hF@(B
  bool isRobot(){return m_robot;}
  
  //! $B=i4|0LCV@_Dj(B
  void setInitPosition(Vector3d pos);

  //! $B=i4|0LCV<hF@(B
  void getInitPosition(Vector3d &pos){pos.set(m_inipos.x(), m_inipos.y(), m_inipos.z());}
  
  //! $B<ANL@_Dj(B
  void setMass(double mass);

  //! $B<ANL<hF@(B
  double getMass(){return m_parts.mass;}

  //! Geometry$B$N?t<hF@(B
  int getGeomNum(){return m_parts.geoms.size();}
  
  //! $B%Q!<%D$N%?%$%W<hF@(B
  int getPartsType(int n){return dGeomGetClass(m_parts.geoms[n]);}
  
  //! n$BHVL\$N%Q!<%D$N(Bgeometry$B<hF@(B
  dGeomID getPartsGeom(int n){return m_parts.geoms[n];}
  
  //! $B%Q!<%DA4BN$N%(%s%F%#%F%#$N0LCV$r86E@$H$9$k=E?40LCV$r<hF@$9$k(B
  Vector3d getCenterOfMass(){return m_parts.pos;}
  
  //! body$B<hF@(B
  dBodyID getPartsBody(){return m_parts.body;}
  
  //! body$B$N@_Dj(B
  void setBody(dBodyID body){m_parts.body = body;}

  //! geometry$B@_Dj(B(pos)$B$G(Bbody$B$H$N$:$l$r@_Dj(B
  void addGeom(dGeomID geom);

  //! $B>WFMH=Dj$r9T$&$+@_Dj(B
  void setCollision(bool col);

  //! $B7A>u%U%!%$%kL>$N@_Dj(B
  void setShapeFile(std::string shape){m_x3dfile = shape;}

  //! $B%(%s%F%#%F%#$N0LCV$r<hF@(B
  //! $BA02s$HJQ99$,$J$1$l$PLa$jCM$O(Bfalse
  //! $B0z?t(Bpre$B$,(Btrue$B$N>l9g$O0\F0H=Dj%Q%i%a!<%?$r99?7(B
  bool getPosition(Vector3d &pos, bool pre = false);

  //! $B%(%s%F%#%F%#$N0LCV$r<hF@(B
  //! $BA02s<hF@;~$HJQ99$,$J$1$l$PLa$jCM$O(Bfalse
  //! $B0z?t(Bpre$B$,(Btrue$B$N>l9g$O2sE>H=Dj%Q%i%a!<%?$r99?7(B
  bool getQuaternion(dQuaternion q, bool pre = false);

  //! $B7A>u%U%!%$%kL>$N<hF@(B
  std::string getShapeFile(){return m_x3dfile;}

  //! Entity ID$BHV9f@_Dj(B
  void setID(int id){m_ID = id;}

  //! Entity ID$BHV9f@_Dj(B
  int  getID(){return m_ID;}

  //! $B%Q!<%D<hF@(B
  SSimObjParts getObjParts(){return m_parts;}

  //! $B%Q!<%D$r=i4|2=(B
  void initObjParts(){
    m_parts.mass    = 1.0;
    m_parts.body    = false;
    m_parts.pos.set(0.0, 0.0, 0.0);
    m_parts.geoms.clear();
  }    

 protected:
  dWorldID            m_world; // ODEWorld
  dSpaceID            m_space; // ODESpace
  int                    m_ID; // Entity ID
  std::string          m_name; // entity name
  bool                m_agent; // agent flag
  Vector3d            m_scale; // shape scale
  Vector3d           m_inipos; // init position
  bool                m_robot; // robot flag

  bool            m_collision; // $B>WFMH=Dj$r9T$&$+$I$&$+(B
  std::string       m_x3dfile; // shape$B%U%!%$%kL>(B
  dReal         m_px, m_py, m_pz; // $B0\F0H=DjMQ(B
  dReal   m_qw, m_qx, m_qy, m_qz; // $B2sE>H=DjMQ(B     
  SSimObjParts      m_parts; // include all ode parts
};

// $B4X@a$r$b$D%m%\%C%H%/%i%9(B
class SSimRobotEntity : public SSimEntity
{
 public:
 SSimRobotEntity(dWorldID world, dSpaceID space, const char *name) 
   : SSimEntity(world, space, name)
    {
    };


  void setRootBody(dBodyID body){m_rootBody = body;}

  dBodyID getRootBody(){return m_rootBody;}

  //! $B%Q!<%D$rDI2C(B
  //void addParts(const char *name);
  
  //! $B%Q!<%D$rDI2C$7%8%g%$%s%H$G@\B3(B
  ///void addParts(std::vector<SSimJoint> joint, const char *parent_name, const char *name);
  void addJoint(SSimJoint *joint);

  //! $B%Q!<%D$N?t<hF@(B
  int  getPartsNum(){return m_allParts.size();}

  //! $B>WFMH=Dj$r9T$&$+@_Dj(B
  void setCollision(bool col);

  //! Joint$B<hF@(B
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
	return NULL;
  }

  //! $B<ANL@_Dj(B
  void setMass(SSimObjParts *parts, double mass);
  void setInitPosition(Vector3d pos);
  std::vector<SSimJoint*> getAllJoints(){return m_allJoints;}


  /*
  void setJointNum(int num){m_jointNum = num;}
  int  getJointNum(){return m_jointNum;}
  */

 private:
  std::vector<SSimRobotParts> m_allParts; // geomtry$B$r$b$DA4%Q!<%D(B
  std::vector<SSimJoint*> m_allJoints;
  dBodyID   m_rootBody;
  int       m_jointNum;
};
#endif
