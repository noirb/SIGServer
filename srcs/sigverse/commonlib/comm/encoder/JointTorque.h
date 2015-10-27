/*
 * Written by okamoto on 2011-12-21
 * Modified by Tetsunari Inamura on 2013-12-30
 *    change JointVelocity  to AngularVelocityToJoint
 *    change ObjectVelocity to AngularVelocityToParts
 */

#ifndef Comm_Data_JointTorque_h
#define Comm_Data_JointTorque_h

#include <sigverse/commonlib/comm/encoder/Encoder.h>

#include <sigverse/commonlib/comm/data/JointTorqueData.h>

namespace CommData {

class AddJointTorqueRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	JointTorqueData m_data;
public:
	AddJointTorqueRequest(const char *aName, const char *jName, double torque) : Encoder(COMM_REQUEST_ADD_JOINT_TORQUE, BUFSIZE), m_data(aName, jName, torque) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


// modified by inamura on 2013-12-30
class SetAngularVelocityToJointRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	AngularVelocityToJointData m_data;
public:
	SetAngularVelocityToJointRequest(const char *aName, const char *jName, double v, double max) : Encoder(COMM_REQUEST_SET_ANGULAR_VELOCITY_JOINT, BUFSIZE), m_data(aName, jName, v, max) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


// modified by inamura on 2013-12-30
class SetAngularVelocityToPartsRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	AngularVelocityToPartsData m_data;
public:
	SetAngularVelocityToPartsRequest(const char *aName, const char *jName, double v, double max/*, double xaxis,double yaxis,double zaxis*/) : Encoder(COMM_REQUEST_SET_ANGULAR_VELOCITY_PARTS, BUFSIZE), m_data(aName, jName, v, max) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


class GetJointAngleRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	GetJointAngleData m_data;
public:
	GetJointAngleRequest(const char *aName, const char *jName) : Encoder(COMM_REQUEST_GET_JOINT_ANGLE, BUFSIZE), m_data(aName, jName) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


class GetJointAngleResult : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	ResultGetJointAngleData m_data;
public:
	GetJointAngleResult(double angle) : Encoder(COMM_RESULT_GET_JOINT_ANGLE, BUFSIZE), m_data(angle) {}

	void angle(double a) { m_data.angle(a);}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace

typedef CommData::AddJointTorqueRequest            CommRequestAddJointTorqueEncoder;
typedef CommData::GetJointAngleRequest             CommRequestGetJointAngleEncoder;
typedef CommData::GetJointAngleResult              CommResultGetJointAngleEncoder;

// modified by inamura on 2013-12-30
typedef CommData::SetAngularVelocityToJointRequest CommRequestSetAngularVelocityToJointEncoder;
typedef CommData::SetAngularVelocityToPartsRequest CommRequestSetAngularVelocityToPartsEncoder;

#endif // Comm_Encoder_JointTorque_h
