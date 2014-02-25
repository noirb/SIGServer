/* $Id: JointQuaternion.h,v 1.2 2011-12-14 08:13:55 okamoto Exp $ */ 
#ifndef JointQuaternion_h
#define JointQuaternion_h

#include "comm/data/JointQuaternionData.h"


class RequestSetJointQuaternionEvent 
{
private:
	JointQuaternionData m_data;
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	const char * getAgentName() { return m_data.agentName(); }
	const char * getJointName() { return m_data.jointName(); }
	double getqw() { return m_data.qw(); }
	double getqx() { return m_data.qx(); }
	double getqy() { return m_data.qy(); }
	double getqz() { return m_data.qz(); }
	double getoffset() { return m_data.offset(); }
};


#endif // JointQuaternion_h
 

