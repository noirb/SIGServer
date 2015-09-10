#ifndef _CX3D_PARSER_H_
#define _CX3D_PARSER_H_

#include "CX3DNode.h"
#include "CX3DBoxNode.h"
#include "CX3DColorNode.h"
#include "CX3DConeNode.h"
#include "CX3DCoordinateNode.h"
#include "CX3DCylinderNode.h"
#include "CX3DDirectionalLightNode.h"
#include "CX3DGroupNode.h"
#include "CX3DHAnimDisplacerNode.h"
#include "CX3DHAnimHumanoidNode.h"
#include "CX3DHAnimJointNode.h"
#include "CX3DHAnimSegmentNode.h"
#include "CX3DHAnimSiteNode.h"
#include "CX3DImageTextureNode.h"
#include "CX3DIndexedFaceSetNode.h"
#include "CX3DIndexedLineSetNode.h"
#include "CX3DMaterialNode.h"
#include "CX3DNormalNode.h"
#include "CX3DOpenHRPHumanoidNode.h"
#include "CX3DOpenHRPJointNode.h"
#include "CX3DOpenHRPSegmentNode.h"
#include "CX3DPointLightNode.h"
#include "CX3DShapeNode.h"
#include "CX3DSphereNode.h"
#include "CX3DSpotLightNode.h"
#include "CX3DTextureCoordinateNode.h"
#include "CX3DTextureTransformNode.h"
#include "CX3DTransformNode.h"
#include "CX3DViewpointNode.h"
#include "CJNIUtil.h"

#include <jni.h>
#include <vector>

class CX3DParser
{
public:
	CX3DParser();
	virtual ~CX3DParser();

	// =================================================
	//	X3D Parse Methods
	// =================================================

	// ---------------------------------------------
	// Parse X3D/VRML files
	// ---------------------------------------------
	bool parse(char *fname);

	// ---------------------------------------------
	// Print the parsing result as log
	// ---------------------------------------------
	void print();

	// Check the node type
	void printNodeTypeList();

	// ---------------------------------------------
	// Return child node of root node
	// ---------------------------------------------
	MFNode *getChildrenOfRootNode();

	// ---------------------------------------------
	// Find a node from child of root with specified node name 
	// grandchildren are not the finding target
	// ---------------------------------------------
	MFNode *searchNodesFromDirectChildrenOfRoot(char *nodeName);

	// ---------------------------------------------
	// Find all of nodes recursively from a starting node with specified node name 
	// ---------------------------------------------
	MFNode *searchNodesFromAllChildrenOfRoot(char *nodeName);

	// ---------------------------------------------
	// Get all of def node name
	// ---------------------------------------------
	std::vector<std::string> getDefNames();

	// ---------------------------------------------
	// Return def node
	// ---------------------------------------------
	CX3DNode *getDefNode(char *defName);


	// =================================================
	// Related debug log
	// =================================================

	// ---------------------------------------------
	// Open the log file
	// fname   : target file name to be output
	// bAppend : true means append mode
	// ---------------------------------------------
	static void openDebugLog(char *fname, bool bAppend=false);

	// ---------------------------------------------
	// Close the log file
	// ---------------------------------------------
	static void closeDebugLog();

	// ---------------------------------------------
	// Set the fp as output target of log
	// If another file is opened before calling this API, the file will be closed.
	// If fp = NULL, no log will be output
	// ---------------------------------------------
	static void setDebugLogFp(FILE *fp);

	// ---------------------------------------------
	// Set the log output destination as stderr
	// If another file is opened before calling this API, the file will be closed.
	// ---------------------------------------------
	static void resetDebugLogFp();

	// ---------------------------------------------
	// Refer the log output destination
	// ---------------------------------------------
	static FILE *getDebugLogFp();

	// ---------------------------------------------
	// Print indent of the log
	// ---------------------------------------------
	static void printIndent(int indent);

	// -----------------------------------------------------
	// Output log
	// Default log output destination is stderr
	// You can change the output destination by openDebugLog or setDebugLog
	// -----------------------------------------------------
	static void printLog(char *format, ...);

	// ---------------------------------------------
	// Output log with indent
	// ---------------------------------------------
	static void printIndentLog(int indentLevel, char *format, ...);

	// ---------------------------------------------
	// Flush all of the log in the buffer
	// ---------------------------------------------
	static void flushLog();

	// ----------------------------------------------------------
	// Set limit (maximum) number of display lines of MF field
	// 
	// For example, there is a MFInt32 which has 1000 elements and this API is called with n=5,
	// print() will display the head 5 lines from the 1000 elements.
	// (All of the MFNode will be displayed)
	// If you call with n=0, all of the elements will be displayed; this is default
	//
	// Counterplan for display of too many elements of MF field
	// ----------------------------------------------------------
	static void setMaxPrintElemsForMFField(int n);

	static int getMaxPrintElemsForMFField();

private:
	std::string m_fname;
	jobject m_X3DParser;
};

#endif

