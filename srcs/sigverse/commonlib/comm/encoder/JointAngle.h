/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Comm_Data_JointAgnle_h
#define Comm_Data_JointAgnle_h

#include <sigverse/commonlib/comm/encoder/Encoder.h>

#include <sigverse/commonlib/comm/data/JointAngleData.h>

namespace CommData {

class SetJointAngleRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, }; //TODO: Magic number
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
 

