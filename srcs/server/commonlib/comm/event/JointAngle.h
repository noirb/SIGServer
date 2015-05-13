/* $Id: JointAngle.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef JointAngle_h
#define JointAngle_h

#include "comm/data/JointAngleData.h"


class RequestSetJointAngleEvent 
{
private:
	JointAngleData m_data;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	const char * getAgentName() { return m_data.agentName(); }
	const char * getJointName() { return m_data.jointName(); }
	double getAngle() { return m_data.angle(); }
};


#endif // JointAngle_h
 

