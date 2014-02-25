/* $Id: JointAngle.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Data_JointAgnle_h
#define Comm_Data_JointAgnle_h

#include "Encoder.h"

#include "comm/data/JointAngleData.h"

namespace CommData {

class SetJointAngleRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	JointAngleData m_data;
public:
	SetJointAngleRequest(const char *aName, const char *jName, double angle) : Encoder(COMM_REQUEST_SET_JOINT_ANGLE, BUFSIZE), m_data(aName, jName, angle) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace

typedef CommData::SetJointAngleRequest CommRequestSetJointAngleEncoder;

#endif // Comm_Encoder_JointAgnle_h
 

