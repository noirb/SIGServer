/* $Id: ResultGetJointForceData.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef ResultGetJointForceData_h
#define ResultGetJointForceData_h

#include <assert.h>
#include <sigverse/commonlib/JointForce.h>

class ResultGetJointForceData
{
public:
	enum { JOINT_FORCE_NUM = 2, };
private:
	bool       m_success;
	JointForce m_jf[JOINT_FORCE_NUM];
public:
	ResultGetJointForceData() : m_success(false) {}
	
	void success(bool b) { m_success = b; }
	bool success()       { return m_success; }

	JointForce * getJointForces() {
		return m_jf;
	}
};


#endif // ResultGetJointForceData_h
