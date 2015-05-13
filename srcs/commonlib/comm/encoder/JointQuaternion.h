/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Comm_Data_JointQuaternion_h
#define Comm_Data_JointQuaternion_h

#include "Encoder.h"

#include "comm/data/JointQuaternionData.h"

namespace CommData {

class SetJointQuaternionRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, }; //TODO: Magic number
private:
	JointQuaternionData m_data;
public:
	SetJointQuaternionRequest(const char *aName, const char *jName, double qw, double qx, double qy, double qz, bool b) : Encoder(COMM_REQUEST_SET_JOINT_QUATERNION, BUFSIZE)
	{
		m_data.agentName(aName);
		m_data.jointName(jName);
		m_data.qw(qw);
		m_data.qx(qx);
		m_data.qy(qy);
		m_data.qz(qz);
		m_data.offset(b);
	}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace

typedef CommData::SetJointQuaternionRequest CommRequestSetJointQuaternionEncoder;

#endif // Comm_Encoder_JointQuaternion_h
 

