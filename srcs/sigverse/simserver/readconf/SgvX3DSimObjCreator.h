#ifndef _SGV_X3D_SIMOBJ_CREATOR_H_
#define _SGV_X3D_SIMOBJ_CREATOR_H_

#include <sigverse/x3d/parser/cpp/X3DParser/CX3DParser.h>
#include <sigverse/x3d/parser/cpp/X3DParser/CX3DShapeNode.h>
#include <sigverse/x3d/parser/cpp/X3DParser/CX3DTransformNode.h>
#include <sigverse/x3d/parser/cpp/X3DParser/CX3DIndexedFaceSetNode.h>
#include <sigverse/x3d/parser/cpp/X3DParser/CSimplifiedShape.h>
#include <sigverse/simserver/SSimObj.h>
#include <sigverse/simserver/SParts.h>
#include <sigverse/simserver/readconf/BodyFactory.h>

#include <sigverse/simserver/readconf/SimpleShapeDB.h>

namespace Sgv
{
	class TestNode;

	class Log {
	public:
		static void println(const char *fmt, ...);
	};

	class X3DSimObjCreator
	{
	public:
		X3DSimObjCreator(BodyFactory &f, SimpleShapeDB &ssdb);
		virtual ~X3DSimObjCreator();

		bool createSSimObjFromX3D(const char *x3dFileName, const char *entClassName, bool bGenSimpleShapeFromX3D);
		bool loadX3DFromFile(const char *x3dFileName);
		//bool loadOpenHRPFromFile(const char *x3dFileName);

		bool createSSimObjFromWrl(
			const std::string &objName,
			std::vector<std::string> &jointNames,
			std::vector<std::string> &partsNames,
			SSimObj *pObj,
			TestNode **ppNode);

	private:
		bool createSSimObjFromOpenHRP(
			std::vector<std::string> &jointNames,
			std::vector<std::string> &partsNames,
			TestNode **ppNode);

		Joint * createSSimObjFromOpenHRP_JointNode(
			TestNode *parent,
			SParts *parentParts,
			CX3DOpenHRPJointNode *pJointNode,
			std::vector<std::string> &jointNames,
			std::vector<std::string> &partsNames,
			int indent);

		SParts * createSSimObjFromOpenHRP_SegmentNode(
			TestNode *parent,
			CX3DOpenHRPSegmentNode *pSegmentNode,
			std::vector<std::string> &jointNames,
			std::vector<std::string> &partsNames,
			int indent);

		SParts * createSSimObjFromOpenHRP_TransformNode(
			TestNode *parent,
			TestNode *sparent,
			CX3DTransformNode *pTransNode,
			const std::string &parentName,
			std::vector<std::string> &jointNames,
			std::vector<std::string> &partsNames,
			int indent);

		SParts * createSSimObjFromOpenHRP_ShapeNode(
			TestNode *parent,
			CX3DShapeNode *pShapeNode,
			const std::string &parentName,
			std::vector<std::string> &partsNames,
			SFRotation *rot,
			int indent,
			SFVec3f *scal);

		SParts *genSPartsFromCX3DShapeNode(CX3DShapeNode *pShapeNode, const char *partsName, int indent);
		SParts *genSPartsFromSimplifiedShape(CSimplifiedShape *ss, const char *partsName, SFVec3f *scal);
		SParts *genSPartsFromSimpleShapeDB(const char *entClassName, const char *partsName);

		// ------------------------------------------
		//
		// ------------------------------------------
	public:
		static void test1(SSimObj *obj, const char *x3dOpenHRPFileName);
		static bool test1(X3DSimObjCreator &, const char *x3dOpenHRPFileName);

		BodyFactory &getBodyFactory() { return m_bodyF; }

	private:
		BodyFactory &m_bodyF;
		CX3DParser *m_pParser;
		MFNode *m_pHumanoidNodes;

		SimpleShapeDB &m_ssdb;
		MFNode *m_pStaticNodes;	
		std::string m_entClassName;
		bool m_bGenSimpleShapeFromX3D;
		FILE *m_fp;
	};

	enum NodeType {
		NODE_TYPE_NOT_SET = -1,
		NODE_TYPE_JOINT,
		NODE_TYPE_SEGMENT,
		NODE_TYPE_TRANSFORM,
		NODE_TYPE_SHAPE,
		NODE_TYPE_DUMMY_SEGMENT,
	};

	class TestNode
	{
	public:
		TestNode(TestNode *parent);
		~TestNode();

		void setType(NodeType type) { m_type = type; }

		NodeType getType() { return m_type; }

		// --------------------------------------
		// --------------------------------------
		void setName(const std::string &name) { m_name = name; }

		// --------------------------------------
		// --------------------------------------
		void setPosition(double x, double y, double z);

		void setPosition(SFVec3f *pos);

		// --------------------------------------
		// --------------------------------------
		void setOrientation(double deg, double ax, double ay, double az);

		void setOrientation(SFRotation *rot);

		void setCenterOfMass(SFVec3f *v)
		{
			if (v) {
				m_center.set(v->x(), v->y(), v->z());
			}
		}

		void setJointType(SFString *type)
		{
			if (type) {
				const char *v = type->getValue();
				if (v) {
					m_jointType = v;
				}
			}
		}

		void setJointAxis(SFVec3f *v)
		{
			if (v) {
				m_jointAxis.set(v->x(), v->y(), v->z());
			}
		}

		void addChild(TestNode *child) { m_children.push_back(child); }

		// --------------------------------------
		// --------------------------------------
		std::string getName() { return m_name; }

		TestNode *getParent() { return m_parent; }

		int count() { return m_children.size(); }

		TestNode *getChild(int i) { return m_children[i]; }

		Vector3d & getPosition() { return m_pos; }

		Rotation &getOrientation() { return m_rot; }

		const char * getJointType()
		{
			return m_jointType.length() > 0? m_jointType.c_str(): NULL;
		}

		void dump(int level=0);

		void calcPartsAbsPos(SSimObj *o, BodyFactory &f);

		void calcPartsAbsPos(SSimObj *o, BodyFactory &f, const Vector3d &basePos, const Rotation &r, int level);

		bool isRoot() { return m_parent == NULL? true: false; }

		SParts * createDummyParts(BodyFactory &f, const Position &pos);

	private:
		NodeType m_type;

		std::string m_name;
		TestNode *m_parent;
		std::vector<TestNode *> m_children;

		//Ogre::Vector3 m_pos;
		Vector3d m_pos;
		//Quaternion m_q;
		Rotation m_rot;
		Vector3d m_center;
		std::string m_jointType;
		Vector3d m_jointAxis;

		Joint  *m_joint;
		SParts *m_parts;
		SParts *m_dummyParts;
	public:
		Joint  *getJoint()  { return m_joint; }
		SParts *getSParts() { return m_parts; }
	};

	void printIndentSpace(int indent);
};

#endif

