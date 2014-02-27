/*
 * Added comments by Tetsunari Inamura on 2014-02-27
 */

#ifndef _CJNIUTIL_H_
#define _CJNIUTIL_H_

#include <jni.h>

class CJNIUtil
{
public:
	// ========================================================
	// Methods for JNI use
	// ========================================================

	// -----------------------------------------------
	// Execute the Java VM according to confFile
	// Create the sole CJNIUtil object
	// -----------------------------------------------
	static bool init(char *confFile);

	// -----------------------------------------------
	// Finalize Java VM and destroy CJNIUtil object
	// -----------------------------------------------
	static void destroy();

	// -----------------------------------------------
	// Refer the CJNIUtil object
	// -----------------------------------------------
	static CJNIUtil *getUtil();

	// -----------------------------------------------
	// Return pointer to the JNIEnv
	// Return NULL when the CJNIUtil is not initialized
	// -----------------------------------------------
	static JNIEnv *getEnv();

	// -----------------------------------------------
	// Execute Java VM
	// -----------------------------------------------
	bool createJavaVM(char *confFile);

	// -----------------------------------------------
	// Finalize Java VM
	// -----------------------------------------------
	void destroyJavaVM();

	JNIEnv *env() { return m_env; }
	JavaVM *jvm() { return m_jvm; }

	jclass getClass(char *className);

	// -----------------------------------------------
	// Create (new) object
	// -----------------------------------------------
	jobject newInstance(char *className);
	jobject newInstance(jclass c);

	jmethodID getMethodID(char *className, char *methodName, char *methodSig);
	jmethodID getMethodID(jclass c, char *methodName, char *methodSig);


	// ========================================================
	// Method for Java X3DParser and related class
	// ========================================================

	// -----------------------------------------------
	// Get class ID to be used frequentry
	// -----------------------------------------------
	jclass getClassOfVRMLNode();

	// -----------------------------------------------
	// Check class type
	// -----------------------------------------------
	bool isInstanceOfVRMLNode(jobject obj);

	// -----------------------------------------------
	// Wrapper for methods of X3DParser class
	// -----------------------------------------------
	bool X3DParser_parse(jobject x3dParser, char *fname);
	jobjectArray X3DParser_getChildrenOfRootNode(jobject x3dParser);
	jobjectArray X3DParser_getDefNames(jobject x3dParser);
	jobject X3DParser_getDefNode(jobject x3dParser, char *defName);

	// -----------------------------------------------
	// Wrapper for methods of VRMLNode class
	// -----------------------------------------------
	char *VRMLNode_getNodeName(jobject vrmlNode);
	int VRMLNode_getNumFields(jobject vrmlNode);
	jobject VRMLNode_getFieldDeclaration(jobject vrmlNode, int i);
	int VRMLNode_getFieldIndex(jobject vrmlNode, char *fieldName);

	// -----------------------------------------------
	// Wrapper for methods of VRMLNodeType class
	// -----------------------------------------------
	jobject VRMLNodeType_getFieldValue(jobject vrmlNode, int i);

	// -----------------------------------------------
	// Wrapper for methods of VRMLFieldDeclaration class
	// -----------------------------------------------
	char *VRMLFieldDeclaration_getName(jobject vrmlDecl);
	int VRMLFieldDeclaration_getFieldType(jobject vrmlDecl);
	char *VRMLFieldDeclaration_getFieldTypeString(jobject vrmlDecl);

private:
	JNIEnv *m_env;
	JavaVM *m_jvm;

	jclass m_class_VRMLNode;

	// ------------------------------------------------
	// This class is not created by constructor
	// Static method named CJNIUtil::init() should be used for the initialization
	// ------------------------------------------------
	CJNIUtil();
	virtual ~CJNIUtil();
};

#endif

