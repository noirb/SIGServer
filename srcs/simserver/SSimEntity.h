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

//! ODEパーツを複数持つobjectのパーツ
//! Robotではない(関節を持たない)エンティティはこれを1つだけ持つ
struct SSimObjParts
{
  // パーツの数
  double                mass;
  std::vector<dGeomID> geoms;  //全ジオメトリー
  dBodyID               body;
  // ODEにおけるパーツの位置は重心の位置となる
  // SIGVerseにおける位置とのずれ
  Vector3d               pos;    
};

//! 関節があるRobotが持つパーツ
struct SSimRobotParts
{
  // 自パーツ
  SSimObjParts objParts;

  // 自パーツの名前
  std::string  name;

  // rootジョイントに付属するパーツかどうか
  bool         root;

  // Joint位置からの重心の位置
  Vector3d      com;
};

//ジョイント
struct SSimJoint
{
  // 自ジョイント名
  std::string     name;

  // 自ジョイント
  dJointID        joint;
  
  // 親ジョイントの名前
  std::string     parent_joint;

  // ID番号
  unsigned int    ID;

  // 親ジョイントからのずれ(初期)
  Vector3d        posFromParent;

  // rootジョイントからのずれ(初期)
  Vector3d        posFromRoot;

  // 設定時の回転軸
  Vector3d        iniAxis;

  // ジオメトリを持つかどうか（ダミーかどうか）
  bool            has_geom;

  // ジオメトリを持たない場合のダミーボディ
  //dBodyID         body;

  // ジョイントが持つロボットパーツ
  SSimRobotParts  robotParts;

  // rootジョイントかどうか
  bool            isRoot;


};



/**
 * @brief エンティティクラス
 *
 * 各エンティティの属性・状態を保持する。
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
  
  //! ODE世界を取得する
  dWorldID getWorld(){return m_world;}

  //! ODESpaceを取得する
  dSpaceID getSpace(){return m_space;}

  //! 名前を取得する
  std::string name(){return m_name;}

  //! エージェントかどうか設定する
  void setIsAgent(bool flag){m_agent = flag;}

  //! エージェントかどうか取得する
  bool isAgent(){return m_agent;}

  //! scaleを設定する
  void setScale(Vector3d vec);

  //! scaleを取得
  Vector3d getScale(){return m_scale;}   
  
  //! 関節を持つロボットかどうか設定
  void setIsRobot(bool flag){m_robot = flag;}
  
  //! 関節を持つロボットかどうか取得
  bool isRobot(){return m_robot;}
  
  //! 初期位置設定
  void setInitPosition(Vector3d pos);

  //! 初期位置取得
  void getInitPosition(Vector3d &pos){pos.set(m_inipos.x(), m_inipos.y(), m_inipos.z());}
  
  //! 質量設定
  void setMass(double mass);

  //! 質量取得
  double getMass(){return m_parts.mass;}

  //! Geometryの数取得
  int getGeomNum(){return m_parts.geoms.size();}
  
  //! パーツのタイプ取得
  int getPartsType(int n){return dGeomGetClass(m_parts.geoms[n]);}
  
  //! n番目のパーツのgeometry取得
  dGeomID getPartsGeom(int n){return m_parts.geoms[n];}
  
  //! パーツ全体のエンティティの位置を原点とする重心位置を取得する
  Vector3d getCenterOfMass(){return m_parts.pos;}
  
  //! body取得
  dBodyID getPartsBody(){return m_parts.body;}
  
  //! bodyの設定
  void setBody(dBodyID body){m_parts.body = body;}

  //! geometry設定(pos)でbodyとのずれを設定
  void addGeom(dGeomID geom);

  //! 衝突判定を行うか設定
  void setCollision(bool col);

  //! 形状ファイル名の設定
  void setShapeFile(std::string shape){m_x3dfile = shape;}

  //! エンティティの位置を取得
  //! 前回と変更がなければ戻り値はfalse
  //! 引数preがtrueの場合は移動判定パラメータを更新
  bool getPosition(Vector3d &pos, bool pre = false);

  //! エンティティの位置を取得
  //! 前回取得時と変更がなければ戻り値はfalse
  //! 引数preがtrueの場合は回転判定パラメータを更新
  bool getQuaternion(dQuaternion q, bool pre = false);

  //! 形状ファイル名の取得
  std::string getShapeFile(){return m_x3dfile;}

  //! Entity ID番号設定
  void setID(int id){m_ID = id;}

  //! Entity ID番号設定
  int  getID(){return m_ID;}

  //! パーツ取得
  SSimObjParts getObjParts(){return m_parts;}

  //! パーツを初期化
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

  bool            m_collision; // 衝突判定を行うかどうか
  std::string       m_x3dfile; // shapeファイル名
  dReal         m_px, m_py, m_pz; // 移動判定用
  dReal   m_qw, m_qx, m_qy, m_qz; // 回転判定用     
  SSimObjParts      m_parts; // include all ode parts
};

// 関節をもつロボットクラス
class SSimRobotEntity : public SSimEntity
{
 public:
 SSimRobotEntity(dWorldID world, dSpaceID space, const char *name) 
   : SSimEntity(world, space, name)
    {
    };


  void setRootBody(dBodyID body){m_rootBody = body;}

  dBodyID getRootBody(){return m_rootBody;}

  //! パーツを追加
  //void addParts(const char *name);
  
  //! パーツを追加しジョイントで接続
  ///void addParts(std::vector<SSimJoint> joint, const char *parent_name, const char *name);
  void addJoint(SSimJoint *joint);

  //! パーツの数取得
  int  getPartsNum(){return m_allParts.size();}

  //! 衝突判定を行うか設定
  void setCollision(bool col);

  //! Joint取得
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

  //! 質量設定
  void setMass(SSimObjParts *parts, double mass);
  void setInitPosition(Vector3d pos);
  std::vector<SSimJoint*> getAllJoints(){return m_allJoints;}


  /*
  void setJointNum(int num){m_jointNum = num;}
  int  getJointNum(){return m_jointNum;}
  */

 private:
  std::vector<SSimRobotParts> m_allParts; // geomtryをもつ全パーツ
  std::vector<SSimJoint*> m_allJoints;
  dBodyID   m_rootBody;
  int       m_jointNum;
};
#endif
