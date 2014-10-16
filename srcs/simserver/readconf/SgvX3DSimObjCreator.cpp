/*
 * Modified by Okamoto on 2012-02-14
 */

#include "SgvX3DSimObjCreator.h"
#include "SSimWorld.h"
#include "ODEWorld.h"
#include "Joint.h"
#include "Rotation.h"
//#include "SgvLog.h"
//#include "CX3DNodeType.h"
#include <stdarg.h>

//#define ENABLE_DUMP // if you want to enable DUMP, you should remove the comment of this line
#include "ParserDump.h"


// This definition should be used in a case that the simple shape information calculated automatically
// should be output as XML file for debug
//#define PRINT_OUT_GENERATED_SIMPLE_SHAPE

namespace Sgv
{
	////////////////////////////////////////////////////////////////
	//
	//	X3DSimObjCreator
	//
	X3DSimObjCreator::X3DSimObjCreator(BodyFactory &f, SimpleShapeDB &ssdb)
		: m_bodyF(f), m_ssdb(ssdb)
	{
		m_pParser = new CX3DParser();

		m_pHumanoidNodes = NULL;
		m_pStaticNodes = NULL;

		m_fp = NULL;
	}

	X3DSimObjCreator::~X3DSimObjCreator()
	{
		if (m_pStaticNodes)	{
			delete m_pStaticNodes;
			m_pStaticNodes = NULL;
		}
		if (m_pHumanoidNodes) {
			delete m_pHumanoidNodes;
			m_pHumanoidNodes = NULL;
		}

		if (m_pParser) {
			delete m_pParser;
			m_pParser = NULL;
		}
	}

#if 1
// FIX20110421(ExpSS)
	bool X3DSimObjCreator::createSSimObjFromX3D(const char *x3dFileName, const char *entClassName, bool bGenSimpleShapeFromX3D)
	{
		DUMP(("loading %s ... (entClassName=%s)", x3dFileName, entClassName));

		// Loading X3D file
		if (!loadX3DFromFile(x3dFileName)) {
			return false;
		}

		if (m_pStaticNodes && m_pStaticNodes->count() > 0) {

			// Static parts is loaded
			SParts *parts = NULL;

			if (bGenSimpleShapeFromX3D)	{
				CX3DTransformNode *tNode = (CX3DTransformNode *)m_pStaticNodes->getNode(0);
				MFNode *shape = tNode->searchNodesFromDirectChildren("Shape");
				if(shape) {
					// Refer the type of shape node
					CX3DShapeNode *sh = (CX3DShapeNode*)shape->getNode(0);
					CX3DNode *pG = sh->getGeometry()->getNode();
					int type = pG->getNodeType();
					
					// Create simple ODE geometry from X3D shape information : added by okamoto on 2011-10-11
					switch(type) {
					case CYLINDER_NODE: {
						// Reference of size data
						CVRMLFieldData *data  = pG->createFieldValue("radius");
						CVRMLFieldData *data2 = pG->createFieldValue("height");
						CVRMLFloatData *dat   = (CVRMLFloatData*)data;
						CVRMLFloatData *dat2  = (CVRMLFloatData*)data2;
						double rad    = dat ->getValue();
						double height = dat2->getValue();
						Position pos(0, 0, 0);

						// Create ODE Parts
						parts = (SParts *)(new SCylinderParts("body", pos, rad, height));
						m_bodyF.applyScaleP(pos); // pos is both of input and output (value will be changed)
						break;
					}

					case BOX_NODE: {
						// Reference of size data
						CVRMLFieldData *data = pG->createFieldValue("size");
						// Cast to Data type
						CVRMLFloatArrayData *dat = (CVRMLFloatArrayData*)data;
						// Reference of data
						double x = dat->getValue(0);
						double y = dat->getValue(1);
						double z = dat->getValue(2);
						Position pos(0, 0, 0);
						Size     size(x,y,z);
						parts = (SParts *)(new SBoxParts("body", pos, size));
						break;
					}
					case SPHERE_NODE: {
						// Reference of radius
						CVRMLFieldData *data = pG->createFieldValue("radius");
						CVRMLFloatData *dat = (CVRMLFloatData*)data;
						double rad = dat->getValue();
						Position pos(0, 0, 0);
						// Create ODE Parts
						parts = (SParts *)(new SSphereParts("body", pos, rad));
						break;
					}
					// Create ODE object based on estimated shape in a case of IndexedFaceSet node
					case INDEXED_FACE_SET_NODE: {
						DUMP(("*** X3DType=Static, SimpleShape=Auto ***\n"));
						// ------------------------------------
						// Automatic generation of simple shape
						// ------------------------------------
						CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromTree(m_pStaticNodes);
						parts = genSPartsFromSimplifiedShape(ss, "body"); // Name of the parts is fixed
						break;
					}
					}
			    }
				else {
					LOG_ERR(("there is no shape node."));
				}
			}
			else {
				DUMP(("*** X3DType=Static, SimpleShape=Manual ***"));
				// ----------------------------------------------
				// Use simple shape which is specified explicitly
				// ----------------------------------------------
			    parts = genSPartsFromSimpleShapeDB(entClassName, "body");	
			    if (!parts)
					{
						LOG_ERR(("no SimpleShape found (className=(%s) partsName=(%s))", entClassName, "body"));
						exit(1);
					}
			}
			
			if (parts)
				{
					DUMP(("*** calling addParts [%s:%d] ***", __FILE__, __LINE__));
					// Add the parts to SSimObj
					m_bodyF.addParts(parts);
				}
		}
		else if (m_pHumanoidNodes && m_pHumanoidNodes->count() > 0)
		{

			std::vector<std::string> jointNames;

			std::vector<std::string> partsNames;

			Sgv::TestNode *root = NULL;

			m_entClassName = entClassName;
			m_bGenSimpleShapeFromX3D = bGenSimpleShapeFromX3D;

			if (createSSimObjFromOpenHRP(jointNames, partsNames, &root))
			{
				SSimObj &obj = getBodyFactory().getSObj();

				Sgv::Log::println("*** jointNames ***");
				for (unsigned int i=0; i<jointNames.size(); i++)
				{
					Sgv::Log::println("[%d] %s", i, jointNames[i].c_str());
				}

				Sgv::Log::println("*** partsNames ***");
				for (unsigned int i=0; i<partsNames.size(); i++)
				{
					Sgv::Log::println("[%d] %s", i, partsNames[i].c_str());
				}

				Sgv::Log::println("*** calling calcPartsAbsPos() [%s:%d] ***", __FILE__, __LINE__);
				root->calcPartsAbsPos(&obj, getBodyFactory());

// konao
				Sgv::Log::println("*** root node dump [%s:%d] ***", __FILE__, __LINE__);
				root->dump();

				SParts *bodyParts = obj.getSBody();
				if (bodyParts)
				{
					Sgv::Log::println("*** bodyParts connection dump [%s:%d] ***", __FILE__, __LINE__);
#ifdef SIMPLE_SHAPE_DEBUG_ON
					bodyParts->dumpConnectionInfo(0);

					Sgv::Log::println("*** bodyParts ODE dump [%s:%d] ***", __FILE__, __LINE__);
					bodyParts->dumpODEInfo(0);
#endif
				}

				Sgv::Log::println("*** obj dump start [%s:%d] ***", __FILE__, __LINE__);
				obj.dump();
				Sgv::Log::println("*** obj dump end [%s:%d] ***", __FILE__, __LINE__);
			}
		}

		DUMP(("all done.\n"));
		return true;
	}
#else
// orig
	bool X3DSimObjCreator::test1(X3DSimObjCreator &creator, const char *x3dOpenHRPFileName)
	{
		//X3DSimObjCreator creator;

		bool isHumanoid = creator.loadOpenHRPFromFile(x3dOpenHRPFileName);

		if (!isHumanoid)
		{
q
!q
		std::vector<std::string> jointNames;
		std::vector<std::string> partsNames;
		Sgv::TestNode *node = NULL;

			//creator.createSSimObjFromWrl("testSSimObj", jointNames, partsNames, NULL, &node);
			DUMP(("all done."));

			return false;
		}
		else
		{
			std::vector<std::string> jointNames;
			std::vector<std::string> partsNames;
			Sgv::TestNode *node = NULL;

			if (creator.createSSimObjFromOpenHRP("testSSimObj", jointNames, partsNames, NULL, &node))
			{
				SSimObj &obj = creator.getBodyFactory().getSObj();
				printf("jointSize = %d\n",obj.getJointSize());
				printf("*** jointNames ***\n");
				for (int i=0; i<jointNames.size(); i++)
				{
					printf("[%d] %s\n", i, jointNames[i].c_str());
				}

				printf("*** partsNames ***\n");
				for (int i=0; i<partsNames.size(); i++)
				{
					printf("[%d] %s\n", i, partsNames[i].c_str());
				}

				printf("*** node dump ***\n");
				node->dump();

				printf("*** parts abs info ***\n");
				node->calcPartsAbsPos(&obj, creator.getBodyFactory());
			}

			DUMP(("all done."));
			return true;
		}
	}
#endif

#if 1
// FIX20110421(ExpSS)
	bool X3DSimObjCreator::loadX3DFromFile(const char *x3dFileName)
	{
		if (!m_pParser->parse((char *)x3dFileName))
		{
			DUMP(("failed to parse (%s)\n", x3dFileName));
			//MessageBox(NULL, msg, "error", MB_OK | MB_ICONEXCLAMATION);

			return false;
		}

		//m_pParser->print();

		if (m_pStaticNodes)
		{
			delete m_pStaticNodes;
			m_pStaticNodes = NULL;
		}
		if (m_pHumanoidNodes)
		{
			delete m_pHumanoidNodes;
			m_pHumanoidNodes = NULL;
		}

		m_pHumanoidNodes = m_pParser->searchNodesFromAllChildrenOfRoot("Humanoid");

		if (!m_pHumanoidNodes || m_pHumanoidNodes->count() == 0)
		{
			//
			//
			//	Group
			//	  Transform
			//		Shape
			//

			MFNode *groupNodes = m_pParser->searchNodesFromDirectChildrenOfRoot("Group");
			if (!groupNodes)
			{
				DUMP(("no group nodes\n"));
				return false;
			}

			CX3DGroupNode *pGroup = (CX3DGroupNode *)(groupNodes->getNode(0));
			if (!pGroup)
			{
				DUMP(("root group is NULL\n"));
				delete groupNodes;
				return false;
			}

			m_pStaticNodes = pGroup->searchNodesFromDirectChildren("Transform");
		}

		return true;
	}
#else
// orig
	bool X3DSimObjCreator::loadOpenHRPFromFile(const char *x3dFileName)
	{
		char msg[256];

		if (!m_pParser->parse((char *)x3dFileName))
		{
			DUMP(("failed to parse (%s)", x3dFileName));
			//MessageBox(NULL, msg, "error", MB_OK | MB_ICONEXCLAMATION);

			return false;
		}

		//m_pParser->print();

		if (m_pHumanoidNodes)
		{
			delete m_pHumanoidNodes;
			m_pHumanoidNodes = NULL;
		}

		m_pHumanoidNodes = m_pParser->searchNodesFromAllChildrenOfRoot("Humanoid");

		// bugfix(sekikawa)(2009/4/10)
		if (!m_pHumanoidNodes) {
			DUMP( ("[Error @X3DSimObjCreator::loadOpenHRPFromFile()]\n") );
			return false;
		}
		else if (m_pHumanoidNodes->count() > 0) {
			return true;
		}
		else {
			DUMP(("%s contains no OpenHRP Shapes\n", x3dFileName));
			return false;
		}

	}
#endif

	bool X3DSimObjCreator::createSSimObjFromWrl(
		const std::string &objName,
		std::vector<std::string> &jointNames,
		std::vector<std::string> &partsNames,
		SSimObj *pObj,
		TestNode **ppNode) {
		//	[TODO]
		m_pParser->printNodeTypeList();

		TestNode *root = new TestNode(NULL);
		jointNames.clear();

		partsNames.clear();

		m_pHumanoidNodes = m_pParser->getChildrenOfRootNode();


		int parentNodeNum = m_pHumanoidNodes->count();
		for(int n=0;n<parentNodeNum;n++) {
			CX3DNode *pNode = m_pHumanoidNodes->getNode(n);
			int fieldNum = pNode->countFields();

			if (pNode) {

				if( pNode->getNodeType()== SHAPE_NODE ) {
				}

				MFNode *children = pNode->searchNodesFromAllChildren(pNode->getNodeName());
				if (children) {
					int m = children->count();
					for (int j=0; j<m; j++) {
						CX3DNode *child = children->releaseNode(j);
						int childNodeType = child->getNodeType();
					}

					delete children;
					children = NULL;
				}

			}
		}

		*ppNode = root;

		return true;
	}

	bool X3DSimObjCreator::createSSimObjFromOpenHRP(
//		const std::string &objName,
		std::vector<std::string> &jointNames,
		std::vector<std::string> &partsNames,
//		SSimObj *,
		TestNode **ppNode)
	{
		if (!ppNode) return false;

		if (!m_pHumanoidNodes)
		{
			Sgv::Log::println("[OutOfMemoryError @X3DSimObjCreator::createSSimObjFromOpenHRP]");
			return false;
		}
		else if (m_pHumanoidNodes->count() == 0) {
			Sgv::Log::println("[No Humanoid node is loaded.]");
			return false;
		}

		Sgv::Log::println("***** createSSimObjFromOpenHRP() started *****");

#ifdef PRINT_OUT_GENERATED_SIMPLE_SHAPE
		m_fp = fopen("ssAutoGen.xml", "w");
#else
		m_fp = NULL;
#endif
		if (m_fp)
		{
			fprintf(m_fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n");
			fprintf(m_fp, "\n");
			fprintf(m_fp, "<simpleShapeDesc>\n");
		}

		TestNode *root = new TestNode(NULL);

		jointNames.clear();

		partsNames.clear();

		CX3DOpenHRPHumanoidNode *pHumanoid = (CX3DOpenHRPHumanoidNode *)(m_pHumanoidNodes->getNode(0));

		if (pHumanoid)
		{
			// -----------------------------------------
			// -----------------------------------------
			MFNode *humanoidBody = pHumanoid->getHumanoidBody();

			// -----------------------------------------
			// -----------------------------------------
			int nBody = humanoidBody->count();

			for (int i=0; i<nBody; i++)
			{
				// -----------------------------------------
				// -----------------------------------------
				CX3DNode *pNode = humanoidBody->getNode(i);
				if (pNode)
				{
					// -----------------------------------------
					// -----------------------------------------
					switch (pNode->getNodeType())
					{
					case OPENHRP_JOINT_NODE:
						{
							// -----------------------------------------
							// -----------------------------------------
							CX3DOpenHRPJointNode *pJointNode = (CX3DOpenHRPJointNode *)pNode;
							createSSimObjFromOpenHRP_JointNode(root, NULL, pJointNode, /* NULL,*/ jointNames, partsNames, 0);
						}
						break;
					default:
						break;
					}
				}
			}
		}

		*ppNode = root;

		if (m_fp)
		{
			fprintf(m_fp, "</simpleShapeDesc>\n");
			fclose(m_fp);
			m_fp = NULL;
		}

		Sgv::Log::println("***** createSSimObjFromOpenHRP() finished *****");

		return true;
	}

	Joint * X3DSimObjCreator::createSSimObjFromOpenHRP_JointNode(
		TestNode *parent,
		SParts *parentParts,
		CX3DOpenHRPJointNode *pJointNode,
//		SSimObj *,
		std::vector<std::string> &jointNames,
		std::vector<std::string> &partsNames,
		int indent)
	{
		//if (!parent || !pJointNode || !o) return;
		if (!parent || !pJointNode) return NULL;

		// -----------------------------------------
		// -----------------------------------------
		SFString *name = pJointNode->getName();
		std::string jointName = name->getValue();

		// -----------------------------------------
		// -----------------------------------------
		SFVec3f *trans = pJointNode->getTranslation();

		// -----------------------------------------
		// -----------------------------------------
		SFRotation *rot = pJointNode->getRotation();

		// -----------------------------------------
		// -----------------------------------------
		SFVec3f *jointAxis = pJointNode->getJointAxis();

		// -----------------------------------------
		//
		// -----------------------------------------
		TestNode *node = new TestNode(parent);

		node->setName(jointName);
		node->setType(NODE_TYPE_JOINT);
		node->setPosition(trans);
		node->setOrientation(rot);
		node->setJointType(pJointNode->getJointType());
		node->setJointAxis(jointAxis);

		parent->addChild(node);

		// -----------------------------------------
		// -----------------------------------------
		jointNames.push_back(jointName);

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("JOINT (%s)", jointName.c_str());

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("translation (%f %f %f)", trans->x(), trans->y(), trans->z());

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("rotation    angle(%f) vec(%f %f %f)", rot->rot(), rot->x(), rot->y(), rot->z());
		SFString *type = pJointNode->getJointType();
		Joint *joint = NULL;

		// -----------------------------------------
		//
		// -----------------------------------------
		MFNode *children = pJointNode->getChildren();

		// -------------------------------------------
		// -------------------------------------------
		int n = children->count();

		// -----------------------------------------
		// -----------------------------------------
		int partsCnt = 0;	
		for (int i=0; i<n; i++)
		{
			// -------------------------------------------
			// -------------------------------------------
			CX3DNode *pNode = children->getNode(i);

			// -------------------------------------------
			// -------------------------------------------
			if (pNode->getNodeType() == OPENHRP_SEGMENT_NODE) {
				CX3DOpenHRPSegmentNode *pSegNode = (CX3DOpenHRPSegmentNode *)pNode;
				SParts *p = createSSimObjFromOpenHRP_SegmentNode(node, pSegNode, /* NULL,*/ jointNames, partsNames, indent+1);
				partsCnt++;	
			}
		}
#if 1
		if (parent->isRoot()) {
			TestNode *dummy = new TestNode(node);
			dummy->setName("body");
			dummy->setType(NODE_TYPE_DUMMY_SEGMENT);
			node->addChild(dummy);
		}
		else if (partsCnt == 0)
#else
		if (partsCnt == 0)
#endif
		{
			char buf[128];
			sprintf(buf, "%s_dummy_seg", jointName.c_str());
			TestNode *dummy = new TestNode(node);
			dummy->setName(buf);
			dummy->setType(NODE_TYPE_DUMMY_SEGMENT);
			node->addChild(dummy);
		}
		// -----------------------------------------
		// -----------------------------------------
		for (int i=0; i<n; i++)
		{
		  // -------------------------------------------
		  // -------------------------------------------
		  CX3DNode *pNode = children->getNode(i);
		  
		  // -------------------------------------------
		  // -------------------------------------------
		  if (pNode->getNodeType() == OPENHRP_JOINT_NODE)
		    {
		      // -------------------------------------------
		      // -------------------------------------------
		      CX3DOpenHRPJointNode *pJointNode = (CX3DOpenHRPJointNode *)pNode;
		      createSSimObjFromOpenHRP_JointNode(node, NULL, pJointNode, /* NULL,*/ jointNames, partsNames, indent+1);
		    }
		}
		return joint;
	}

	SParts * X3DSimObjCreator::createSSimObjFromOpenHRP_SegmentNode(
		TestNode *parent,
		CX3DOpenHRPSegmentNode *pSegmentNode,
//		SSimObj *,
		std::vector<std::string> &jointNames,
		std::vector<std::string> &partsNames,
		int indent)
	{
		//if (!parent || !pSegmentNode || !o) return;
		if (!parent || !pSegmentNode) return NULL;

		// -----------------------------------------
		// -----------------------------------------
		SFString *name = pSegmentNode->getName();
		std::string segmentName = name->getValue();

		SFFloat *mass = pSegmentNode->getMass();
		float ms = mass->getValue();

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("SEGMENT (%s)", segmentName.c_str());

		TestNode *node = new TestNode(parent);	
		node->setType(NODE_TYPE_SEGMENT);
		node->setName(segmentName);
		node->setCenterOfMass(pSegmentNode->getCenterOfMass());


		// -----------------------------------------
		//
		// -----------------------------------------
		MFNode *children = pSegmentNode->getChildren();

		// -------------------------------------------
		// -------------------------------------------
		int n = children->count();
		bool createS = false;

		for (int i=0; i<n; i++)
		{
			// -------------------------------------------
			// -------------------------------------------
			CX3DNode *pNode = children->getNode(i);

			// -------------------------------------------
			// -------------------------------------------
			switch (pNode->getNodeType())
			{
			case TRANSFORM_NODE:
				{

					// -------------------------------------------
					// -------------------------------------------
					CX3DTransformNode *pTransNode = (CX3DTransformNode *)pNode;

				        if(createS) break;
					
					SParts *p = createSSimObjFromOpenHRP_TransformNode(parent, node, pTransNode, /* NULL,*/ segmentName, jointNames, partsNames, indent+1);	
					createS = true;
					
				break;
				}
			default:
				break;

			}
		}

		SParts *parts = getBodyFactory().getSObj().getSParts(segmentName.c_str());

		if (!parts) {
			//
			//

			delete node;	

			TestNode *newNode = new TestNode(parent);	
			newNode->setName(segmentName);
			newNode->setType(NODE_TYPE_DUMMY_SEGMENT);
			newNode->setCenterOfMass(pSegmentNode->getCenterOfMass());
			parent->addChild(newNode);
		} else {
			parent->addChild(node);
			parts->setMass(ms);
		}

		return parts;
	}

	SParts * X3DSimObjCreator::createSSimObjFromOpenHRP_TransformNode(
		TestNode *parent,
		TestNode *sparent,
		CX3DTransformNode *pTransNode,
//		SSimObj *,
		const std::string &parentName,
		std::vector<std::string> &jointNames,
		std::vector<std::string> &partsNames,
		int indent)
	{
		//if (!parent || !pTransNode || !o) return;
		if (!parent || !pTransNode) return NULL;

		// -----------------------------------------
		// -----------------------------------------
		SFVec3f *trans = pTransNode->getTranslation();

		// -----------------------------------------
		// -----------------------------------------
		SFRotation *rot = pTransNode->getRotation();

		// -----------------------------------------
		//
		// -----------------------------------------
		TestNode *node = new TestNode(parent);

		node->setName(parentName);
		node->setType(NODE_TYPE_TRANSFORM);
		node->setPosition(trans);
		//node->setOrientation(rot);

		//okamoto (2011/1/28)
		//sparent->setPosition(trans);

		//okamoto (2010/12/6)
		//sparent->setOrientation(rot);

		/*
		if(sparent->getJointType() == "fixed"){
		  LOG_MSG(("fixed"));
		  sparent->setOrientation(rot);
		}
		*/

		parent->addChild(node);

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("TRANSFORM");

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("translation (%f %f %f)", trans->x(), trans->y(), trans->z());

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("rotation    angle(%f) vec(%f %f %f)", rot->rot(), rot->x(), rot->y(), rot->z());

		// -----------------------------------------
		//
		// -----------------------------------------
		MFNode *children = pTransNode->getChildren();

		// -------------------------------------------
		// -------------------------------------------
		int n = children->count();

		SParts *parts = NULL;
		for (int i=0; i<n; i++)
		{
			// -------------------------------------------
			// -------------------------------------------
			CX3DNode *pNode = children->getNode(i);

			// -------------------------------------------
			// -------------------------------------------
			switch (pNode->getNodeType())
			{
			case TRANSFORM_NODE:
				{
					// -------------------------------------------
					// -------------------------------------------
					CX3DTransformNode *pTransNode = (CX3DTransformNode *)pNode;
					SParts *p = createSSimObjFromOpenHRP_TransformNode(parent, node, pTransNode, /* NULL,*/ parentName, jointNames, partsNames, indent+1);
					if (p) {
						//assert(!parts);
						parts = p;
					}
				}
				break;

			case SHAPE_NODE:
				{
					// -------------------------------------------
					// -------------------------------------------
					CX3DShapeNode *pShapeNode = (CX3DShapeNode *)pNode;
					SParts *p = createSSimObjFromOpenHRP_ShapeNode(node, pShapeNode, /* NULL, */ parentName, partsNames, rot,indent+1);
					if (p) {
						parts = p;
						//assert(!parts);
					}
				}
				break;
			default:
				break;
			}
		}
		return parts;
	}

	SParts* X3DSimObjCreator::createSSimObjFromOpenHRP_ShapeNode(
		TestNode *parent,
		CX3DShapeNode *pShapeNode,
//		SSimObj *,
		const std::string &parentName,
		std::vector<std::string> &partsNames,
		SFRotation *rot,
		int indent)
	{

		if (!parent || !pShapeNode) return NULL;

		Sgv::printIndentSpace(indent);
		Sgv::Log::println("SHAPE");

		std::string partsName = parentName;

		TestNode *node = new TestNode(parent);
		node->setName(partsName.c_str());
		node->setType(NODE_TYPE_SHAPE);
		parent->addChild(node);

		// -----------------------------------------
		// -----------------------------------------
		partsNames.push_back(partsName);

		SParts *parts = NULL;

		if (m_bGenSimpleShapeFromX3D)
		{
// konao
//////////////////////

		  //CX3DShapeNode *sh = pShapeNode->getNode(0);
		  CX3DNode *pG = pShapeNode->getGeometry()->getNode();
		  int type = pG->getNodeType();

		  switch(type) {
		    
		  case CYLINDER_NODE:{
		    
		    CVRMLFieldData *data = pG->createFieldValue("radius");
		    CVRMLFieldData *data2 = pG->createFieldValue("height");
		    CVRMLFloatData *dat = (CVRMLFloatData*)data;
		    CVRMLFloatData *dat2 = (CVRMLFloatData*)data2;
		    double rad = dat->getValue();
		    double height = dat2->getValue();
		    //Position pos(10, 10, 10);
		    Position pos(0, 0, 0);

		    Rotation rotation;		    
		    rotation.setAxisAndAngle(rot->x(), rot->y(), rot->z(), rot->rot());
		    parts = (SParts *)(new SCylinderParts(partsName.c_str(), pos, rad, height, rotation));

		    break;
		  }
		    
		  case BOX_NODE:{
		    
		    CVRMLFieldData *data = pG->createFieldValue("size");
		    
		    //int dattype = data->getFieldType();
		    //LOG_MSG(("dattype = %d",dattype));
		    
		    CVRMLFloatArrayData *dat = (CVRMLFloatArrayData*)data;
		    
		    double x = dat->getValue(0);
		    double y = dat->getValue(1);
		    double z = dat->getValue(2);
		    
		    Position pos(0, 0, 0);
		    Size     size(x,y,z);

		    Rotation rotation;		    
		    rotation.setAxisAndAngle(rot->x(), rot->y(), rot->z(), rot->rot());

		    parts = (SParts *)(new SBoxParts(partsName.c_str(), pos, size, rotation));
		    break;
		  }
		    
		  case SPHERE_NODE:{

		    CVRMLFieldData *data = pG->createFieldValue("radius");
		    CVRMLFloatData *dat = (CVRMLFloatData*)data;
		    double rad = dat->getValue();
		    Position pos(0, 0, 0);

		    parts = (SParts *)(new SSphereParts(partsName.c_str(), pos, rad));
		    break;
		  }
		    
		  case INDEXED_FACE_SET_NODE:{	
		    ///////////////
		    DUMP(("*** SimpleShape: Auto generate mode ***\n"));
		    
#define SIMPLE_SHAPE_GEN_MSI
#ifdef SIMPLE_SHAPE_GEN_MSI
		    // ----------------------------------
		    // ----------------------------------
		    CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromShapeNode(pShapeNode, CSimplifiedShape::BOX);
		    parts = genSPartsFromSimplifiedShape(ss, partsName.c_str());

#else
		    parts = genSPartsFromCX3DShapeNode(pShapeNode, partsName.c_str(), indent);
#endif
		    break;
		  }
		  }
		}
		else
		  {
		    // konao
		    DUMP(("*** SimpleShape: Manual setting mode ***\n"));
		    
		    // ----------------------------------------
		    // ----------------------------------------
		    parts = genSPartsFromSimpleShapeDB(m_entClassName.c_str(), partsName.c_str());
		    if (!parts)
		      {
			LOG_ERR(("no SimpleShape found (className=(%s) partsName=(%s))\n", m_entClassName.c_str(), partsName.c_str()));
			exit(1);
		      }
		  }

		if (parts)
		  {
		    // konao
		    DUMP(("*** calling addParts (%s) [%s:%d] ***\n", partsName.c_str(), __FILE__, __LINE__));
		    getBodyFactory().addParts(parts);	
		    parts = NULL;
		  }
		
		return parts;
	}

#if 1
// FIX20110421(ExpSS)
	SParts *X3DSimObjCreator::genSPartsFromCX3DShapeNode(CX3DShapeNode *pShapeNode, const char *partsName, int indent)
	{
		SParts *parts = NULL;

		if (!pShapeNode) return parts;

		// -----------------------------------------
		// -----------------------------------------
		SFNode *geometry = pShapeNode->getGeometry();

		if (geometry)
		{

			// -----------------------------------------
			// -----------------------------------------
			CX3DNode *pNodeData = geometry->getNode();

			//CSimplifiedShapeFactory::isCylinder(pNodeData);

			if (pNodeData)
			{
				//
				//
				int nodeType = pNodeData->getNodeType();
				switch(nodeType) {
				case INDEXED_FACE_SET_NODE:
				{
					Sgv::printIndentSpace(indent);
					Sgv::Log::println("INDEXED_FACE_SET");
					//
					//
					CSimplifiedBox *bx = CSimplifiedShapeFactory::calcBox( (CX3DIndexedFaceSetNode*)pNodeData );

					float x1 = bx->x1();
					float y1 = bx->y1();
					float z1 = bx->z1();
					float x2 = bx->x2();
					float y2 = bx->y2();
					float z2 = bx->z2();

					float cx = (x1+x2)/2;
					float cy = (y1+y2)/2;
					float cz = (z1+z2)/2;
					float sx = (float)fabs(x2-x1);
					float sy = (float)fabs(y2-y1);
					float sz = (float)fabs(z2-z1);

					Position pos(cx, cy, cz);
					getBodyFactory().applyScaleP(pos);

					Size sz_(sx, sy, sz);
					getBodyFactory().applyScaleS(sz_);

					float radius;
					float height;
					float xAxis[2];
					float yAxis[2];
					float zAxis[2];
					bool isCylinder = CSimplifiedShapeFactory::isCylinder(
												(CX3DIndexedFaceSetNode*)pNodeData,
												radius,
												height,
												xAxis,
												yAxis,
												zAxis
											);

					if(isCylinder) {
						pos.x( ( (xAxis[0] + xAxis[1]) / 2.0 ) );
						pos.y( ( (yAxis[0] + yAxis[1]) / 2.0 ) );
						pos.z( ( (zAxis[0] + zAxis[1]) / 2.0 ) );

						parts = (SParts *)( new SCylinderParts(partsName, pos, radius,height) );
						double vecX = xAxis[1] - xAxis[0];
						double vecY = yAxis[1] - yAxis[0];
						double vecZ = zAxis[1] - zAxis[0];
						((SCylinderParts*)parts)->initializeAngle(vecX,vecY,vecZ);

					}
					else {
						parts = (SParts *)(new SBoxParts(partsName, pos, sz_));
					}

					break;
				}
				case CYLINDER_NODE:
				{
					//
					//
					CSimplifiedCylinder *cy = CSimplifiedShapeFactory::calcCylinder( (CX3DCylinderNode *)pNodeData );

					const char *partsCylinderName = partsName;
					Position pos(cy->x(),cy->y(),cy->z());
					getBodyFactory().applyScaleP(pos);

					Size sz_(cy->radius(), cy->radius(), cy->height());
					getBodyFactory().applyScaleS(sz_);

					float radius = cy->radius();
					float height = cy->height();

					parts = (SParts *)( new SCylinderParts(partsName, pos, radius,height) );
					((SCylinderParts*)parts)->initializeAngle(0.0,0.0,1.0);
					break;
				}
				default:
					break;
				}
			}
		}

		return parts;
	}

	SParts *X3DSimObjCreator::genSPartsFromSimplifiedShape(CSimplifiedShape *ss, const char *partsName)
	{
		if (!ss) return NULL;

		SParts *parts = NULL;

		if (m_fp)
		{
			fprintf(m_fp, "\t<parts name=\"%s\" ", partsName ? partsName : "");
		}

		switch (ss->getType())
		{
			case CSimplifiedShape::SPHERE:
			{
				CSimplifiedSphere *sphere = (CSimplifiedSphere *)ss;

				float cx = sphere->x();
				float cy = sphere->y();
				float cz = sphere->z();
				float r = sphere->radius();

				Position pos(cx, cy, cz);
				m_bodyF.applyScaleP(pos);	

				if (m_fp)
				{
					fprintf(m_fp, "type=\"sphere\">\n");
					fprintf(m_fp, "\t\t<position x=\"%f\" y=\"%f\" z=\"%f\"/>\n", pos.x(), pos.y(), pos.z());
//					fprintf(m_fp, "\t\t<size r=\"%f\"/>\n", r);
					fprintf(m_fp, "\t\t<size r=\"%f\"/>\n", r/5);
				}
// konao
#if 1
DUMP(("\tshapeType=Sphere\n"));
DUMP(("\tpos (x=\"%f\" y=\"%f\" z=\"%f\")\n", pos.x(), pos.y(), pos.z()));
DUMP(("\tr=\"%f\"\n", r));
#endif

				parts = (SParts *)(new SSphereParts(partsName, pos, r));
			}
			break;

			case CSimplifiedShape::CYLINDER:
			{
				CSimplifiedCylinder *cylinder = (CSimplifiedCylinder *)ss;

				float cx = cylinder->x();
				float cy = cylinder->y();
				float cz = cylinder->z();
				float h = cylinder->height();
				float r = cylinder->radius();

				Position pos(cx, cy, cz);
				m_bodyF.applyScaleP(pos);	

				if (m_fp)
				{
					fprintf(m_fp, "type=\"cylinder\">\n");
					fprintf(m_fp, "\t\t<position x=\"%f\" y=\"%f\" z=\"%f\"/>\n", pos.x(), pos.y(), pos.z());
//					fprintf(m_fp, "\t\t<size r=\"%f\" h=\"%f\"/>\n", r, h);
					fprintf(m_fp, "\t\t<size r=\"%f\" h=\"%f\"/>\n", r/5, h/5);
				}
// konao
#if 1
DUMP(("\tshapeType=Sylinder\n"));
DUMP(("\tpos (x=\"%f\" y=\"%f\" z=\"%f\")\n", pos.x(), pos.y(), pos.z()));
DUMP(("\tr=\"%f\", h=\"%f\"\n", r, h));
#endif

				parts = (SParts *)(new SCylinderParts(partsName, pos, r, h));
			}
			break;

			case CSimplifiedShape::BOX:
			{
				CSimplifiedBox *bx = (CSimplifiedBox *)ss;

				float x1 = bx->x1();
				float y1 = bx->y1();
				float z1 = bx->z1();
				float x2 = bx->x2();
				float y2 = bx->y2();
				float z2 = bx->z2();
				// printf("estimate box1(%f, %f, %f)\n", x1, y1, z1);
				// printf("estimate box2(%f, %f, %f)\n", x2, y2, z2);

				float cx = (x1+x2)/2;
				float cy = (y1+y2)/2;
				float cz = (z1+z2)/2;
				float sx = (float)fabs(x2-x1);
				float sy = (float)fabs(y2-y1);
				float sz = (float)fabs(z2-z1);

				Position pos(cx, cy, cz);
				m_bodyF.applyScaleP(pos);	

				Size size(sx, sy, sz);
				m_bodyF.applyScaleS(size);	

				if (m_fp)
				{
					fprintf(m_fp, "type=\"box\">\n");
					fprintf(m_fp, "\t\t<position x=\"%f\" y=\"%f\" z=\"%f\"/>\n", pos.x(), pos.y(), pos.z());
//					fprintf(m_fp, "\t\t<size sx=\"%f\" sy=\"%f\" sz=\"%f\"/>\n", size.x(), size.y(), size.z());
					fprintf(m_fp, "\t\t<size sx=\"%f\" sy=\"%f\" sz=\"%f\"/>\n", size.x()/50, size.y()/50, size.z()/50);
				}
// konao
#if 1
DUMP(("\tshapeType=Box\n"));
DUMP(("\tpos (x=\"%f\" y=\"%f\" z=\"%f\")\n", pos.x(), pos.y(), pos.z()));
DUMP(("\tsize (sx=\"%f\" sy=\"%f\" sz=\"%f\")\n", size.x(), size.y(), size.z()));
#endif

				parts = (SParts *)(new SBoxParts(partsName, pos, size));
			}
			break;
		}

		if (m_fp)
		{
			fprintf(m_fp, "\t</parts>\n");
		}

		return parts;
	}

	SParts *X3DSimObjCreator::genSPartsFromSimpleShapeDB(const char *entClassName, const char *partsName)
	{
		SParts *parts = NULL;

		SimpleShape *ss = m_ssdb.get(entClassName);
		if (ss)
		{
			SimpleShapeElem *sse = ss->get(partsName);
			if (sse)
			{
				switch (sse->getType())
				{
					case SimpleShapeElem::SS_TYPE_SPHERE:
					{
// konao
#if 1
DUMP(("genSPartsFromSimpleShapeDB() (type=sphere) [%s:%d]\n", __FILE__, __LINE__));
DUMP(("\tpos (x=\"%f\" y=\"%f\" z=\"%f\")\n", sse->x(), sse->y(), sse->z()));
DUMP(("\tr=\"%f\"\n", sse->r()));
#endif
						Position pos(sse->x(), sse->y(), sse->z());
						parts = (SParts *)(new SSphereParts(partsName, pos, sse->r()));
						parts->setODEPos(sse->x(), sse->y(), sse->z());
					}
					break;

					case SimpleShapeElem::SS_TYPE_CYLINDER:
					{
// konao
#if 1
DUMP(("genSPartsFromSimpleShapeDB() (type=cylinder) [%s:%d]\n", __FILE__, __LINE__));
DUMP(("\tpos (x=\"%f\" y=\"%f\" z=\"%f\")\n", sse->x(), sse->y(), sse->z()));
DUMP(("\tr=\"%f\", h=\"%f\"\n", sse->r(), sse->h()));
#endif
						Position pos(sse->x(), sse->y(), sse->z());
						parts = (SParts *)(new SCylinderParts(partsName, pos, sse->r(), sse->h()));
						parts->setODEPos(sse->x(), sse->y(), sse->z());
					}
					break;

					case SimpleShapeElem::SS_TYPE_BOX:
					{
// konao
#if 1
DUMP(("genSPartsFromSimpleShapeDB() (type=box) [%s:%d]\n", __FILE__, __LINE__));
DUMP(("\tpos (x=\"%f\" y=\"%f\" z=\"%f\")\n", sse->x(), sse->y(), sse->z()));
DUMP(("\tsize (sx=\"%f\" sy=\"%f\" sz=\"%f\")\n", sse->sx(), sse->sy(), sse->sz()));
#endif
						Position pos(sse->x(), sse->y(), sse->z());
						Size size(sse->sx(), sse->sy(), sse->sz());
						parts = (SParts *)(new SBoxParts(partsName, pos, size));
						parts->setODEPos(sse->x(), sse->y(), sse->z());
					}
					break;
				}
			}
		}

		return parts;
	}
#endif

	////////////////////////////////////////////////////////////////
	//
	//	TestNode
	//
	TestNode::TestNode(TestNode *parent)
		: m_type(NODE_TYPE_NOT_SET), m_parent(parent), m_name(""),
		  m_joint(NULL), m_parts(NULL), m_dummyParts(NULL)
	{
		m_pos.set(0, 0, 0);
		//m_rot = Ogre::Quaternion(Ogre::Radian(0), Ogre::Vector3(1, 0, 0));
		m_rot.setAxisAndAngle(1, 0, 0, 0);

		m_children.clear();
	}

	TestNode::~TestNode() {

	}

	void TestNode::setPosition(double x, double y, double z)
	{
		m_pos.set(x, y, z);
		/*
		m_pos.x = x;
		m_pos.y = y;
		m_pos.z = z;
		*/
	}

	void TestNode::setPosition(SFVec3f *pos)
	{
		if (pos)
		{
			/*
			m_pos.x = pos->x();
			m_pos.y = pos->y();
			m_pos.z = pos->z();
			*/
			m_pos.set(pos->x(), pos->y(), pos->z());
		}
	}

	/*
	void TestNode::setOrientation(double deg, double ax, double ay, double az)
	{
		//Ogre::Vector3 axis(ax, ay, az);
		Vector3d axis(ax, ay, az);
		axis.normalize();

		//Ogre::Radian rot(3.14159265/180*deg);
		double rad = (3.14159265*deg/180);

		//m_rot = Ogre::Quaternion(rot, axis);
		m_rot.setAxisAndAngle(axis.x(), axis.y(), axis.z(), rad);
	}
	*/

	void TestNode::setOrientation(SFRotation *rot)
	{
		Vector3d axis(rot->x(), rot->y(), rot->z());
		axis.normalize();
		double rad = rot->rot();
		m_rot.setAxisAndAngle(axis.x(), axis.y(), axis.z(), rad);
	}

	void TestNode::dump(int level)
	{
		char buf[256];

		if (level == 0)
		{
			Sgv::Log::println("J=Joint, G=Segment, T=Transform, S=Shape, D=Dummy");
		}

		switch (m_type)
		{
			case NODE_TYPE_JOINT:
				strcpy(buf, "J");
				break;

			case NODE_TYPE_SEGMENT:
				strcpy(buf, "G");
				break;

			case NODE_TYPE_TRANSFORM:
				strcpy(buf, "T");
				break;

			case NODE_TYPE_SHAPE:
				strcpy(buf, "S");
				break;

			case NODE_TYPE_DUMMY_SEGMENT:
				strcpy(buf, "D");
				break;
			default:
				strcpy(buf, "U");	// unknown
		}

		// name & type
		Sgv::printIndentSpace(level);
		Sgv::Log::println("[%s](%s)", buf, getName().c_str());

		// pos
		Vector3d &pos = getPosition();
//		Sgv::printIndentSpace(level);
//		Sgv::Log::println("pos (%f %f %f)", pos.x(), pos.y(), pos.z());

		// orientation
		const dReal *q = getOrientation().q();
//		Sgv::printIndentSpace(level);
//		Sgv::Log::println("q (%f %f %f %f)", q[0], q[1], q[2], q[3]);

		int n = m_children.size();

		for (int i=0; i<n; i++)
		{
			TestNode *child = m_children[i];

			if (child)
			{
				child->dump(level+1);
			}
		}
	}

	void TestNode::calcPartsAbsPos(SSimObj *o, BodyFactory &f)
	{
		for (int i=0; i<count(); i++)
		{
			TestNode *child = getChild(i);
			if (child)
			{
				child->calcPartsAbsPos(o, f, m_pos, m_rot, 0);
			}
		}
	}

	void TestNode::calcPartsAbsPos(SSimObj *o, BodyFactory &f, const Vector3d &basePos, const Rotation &baseR, int level)
	{
		char * nodeName[] = {
			"Joint", "Segment", "Transform", "Shape", "DummySegment"};

		Sgv::Log::println("");
		Sgv::printIndentSpace(level);
		Sgv::Log::println("*** %s (%s) ***", nodeName[getType()], m_name.c_str());

		Vector3d pos(m_pos);

		Rotation r(baseR);
		r *= m_rot;

		{
			baseR.apply(pos);

			pos += basePos;
		}


		Sgv::printIndentSpace(level);
		Sgv::Log::println("m_pos = (%f, %f, %f)", m_pos.x(), m_pos.y(), m_pos.z());
		Sgv::printIndentSpace(level);
		Sgv::Log::println("pos(%f %f %f)", pos.x(), pos.y(), pos.z());

		Sgv::printIndentSpace(level);
		const dReal *q = r.q();
		Sgv::Log::println("q(%f %f %f %f)", q[0], q[1], q[2], q[3]);


		{
			Vector3d z(0, 0, 1);
			r.apply(z);
			Sgv::printIndentSpace(level);
			Sgv::Log::println("z-axis(rot axis) -> (%f %f %f)", z.x(), z.y(), z.z());
		}

		{
			Vector3d x(1, 0, 0);
			r.apply(x);
			Sgv::printIndentSpace(level);
			Sgv::Log::println("x-axis(zero-angle) -> (%f %f %f)", x.x(), x.y(), x.z());
		}


		NodeType type = getType();
		switch(type)
		{
		case NODE_TYPE_JOINT:
			if (const char *jointType = getJointType()) {

				Sgv::printIndentSpace(level);
				Sgv::Log::println("jointType = %s",jointType);
				if (strcmp(jointType, "fixed") == 0) {
					m_joint = new FixedJoint(m_name.c_str());
				} else if (strcmp(jointType, "rotate") == 0) {
					Vector3d &axis = m_jointAxis;
					Sgv::printIndentSpace(level);
					Sgv::Log::println("jointAxis = (%f, %f, %f)",
							  axis.x(), axis.y(), axis.z());
					m_joint = new HingeJoint(m_name.c_str(), axis);
				} else if (strcmp(jointType, "sphere") == 0) {
					m_joint = new BallJoint(m_name.c_str());
				}

				if (m_joint) {
					f.addJoint(m_joint);
					Vector3d v(pos);
					f.applyScaleV(v);
					m_joint->setAnchor(v.x(), v.y(), v.z());
				}
			}
			break;

		case NODE_TYPE_SEGMENT:
			Sgv::printIndentSpace(level);
			Sgv::Log::println("centerOfMass = (%f, %f, %f)",
					  m_center.x(), m_center.y(), m_center.z());

			if (SParts *parts = o->getSParts(m_name.c_str())) {	
				Sgv::printIndentSpace(level);
				Sgv::Log::println("parts(%s) exists", m_name.c_str());

				Vector3d c(pos);
				c += m_center;

				Sgv::printIndentSpace(level);
				Sgv::Log::println("Parts pos = (%f, %f, %f)",
						  c.x(), c.y(), c.z());
				f.applyScaleV(c);
				parts->setInitialPosition(c);
				parts->setInitialQuaternion(m_rot);
				m_parts = parts;
			}
			break;

		case NODE_TYPE_DUMMY_SEGMENT:
			{
				Sgv::printIndentSpace(level);
				Sgv::Log::println("creating dummy = (%s)", m_name.c_str());

				Position pos_;
				/*
				pos_.set(pos.x() + m_center.x(),
					 pos.y() + m_center.y(),
					 pos.z() + m_center.z());
				*/

				pos_.set(pos.x(), pos.y(), pos.z());
				f.applyScaleP(pos_);

				if (strcmp(m_name.c_str(), "body") == 0)
				{
// begin(FIX20110421(ExpSS))
					m_parts = new SSphereParts(m_name.c_str(), pos_, 1.0);	
//					m_parts = new SBlindParts(m_name.c_str(), pos_);
// end(FIX20110421(ExpSS))
				}
				else
				{
					m_parts = new SBlindParts(m_name.c_str(), pos_);
				}

// konao
DUMP(("*** calling addParts (dummy) (%s) [%s:%d] ***\n", m_name.c_str(), __FILE__, __LINE__));
				f.addParts(m_parts);
			}
			break;
		default:
			break;
		}

		for (int i=0; i<count(); i++)
		{
			TestNode *child = getChild(i);
			if (child)
			{
				child->calcPartsAbsPos(o, f, pos, r, level+1);
			}
		}

		if (m_joint && m_parent) {

			TestNode *sibPartsNode = NULL;

			for (int i=0; i<m_parent->count(); i++) {
				TestNode *p = m_parent->getChild(i);
				if (p && p->getSParts()) {
					sibPartsNode = p;
					break;
				}
			}

			TestNode *childPartsNode = NULL;

			for (int i=0; i<count(); i++) {
				TestNode *p = getChild(i);
				if (p && p->getSParts()) {
					childPartsNode = p;
					break;
				}
			}

			f.connect(m_joint,
				   sibPartsNode->getSParts(),
				   childPartsNode->getSParts());

			bool b = false;
			for (int i=0; i<count(); i++) {
				TestNode *p = getChild(i);
				if (p && p->getJoint()) {
					b =true;
					f.connect2(m_joint, p->getJoint(),
						   sibPartsNode->getSParts(),
						   childPartsNode->getSParts());
				}
			}

			if (!b) {
				f.connect2(m_joint, NULL,
					   sibPartsNode->getSParts(),
					   childPartsNode->getSParts());
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//
	void printIndentSpace(int indent)
	{
#ifdef SIMPLE_SHAPE_DEBUG_ON
		for (int i=0; i<indent; i++) printf("  ");
#endif
	}

	void Log::println(const char *fmt, ...)
	{
#ifdef SIMPLE_SHAPE_DEBUG_ON
		va_list args;
		va_start(args, fmt);
		char buf[256];
		vsprintf(buf, fmt, args);
		printf(buf);
		printf("\n");
		va_end(args);
#endif
	}
};


