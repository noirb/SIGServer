/*
 * Written by okamoto 2011-12-28
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Change Set/GetVelocity to Set/GetLinearVelocity
 * Added SetLinearVelocity by Tetsunari Inamura on 2014-01-06
 * Modified by Tetsunari Inamura on 2014-01-13
 *    Bug fixed on GetAngularVelocityRequest
 * Added addTorque by Tetsunari Inamura on 2014-02-26
 */

#ifndef Comm_Data_MoveControl_h
#define Comm_Data_MoveControl_h

#include "Encoder.h"

//#include "comm/data/JointTorqueData.h"

namespace CommData {

class AddForceRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	double m_x;
	double m_y;
	double m_z;
	bool m_relf;
public:
 AddForceRequest(const char *name, double x, double y, double z, bool relf) : Encoder(COMM_REQUEST_ADD_FORCE, BUFSIZE),m_name(name), m_x(x), m_y(y), m_z(z), m_relf(relf) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


class AddForceAtPosRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	double m_x;
	double m_y;
	double m_z;
	double m_px;
	double m_py;
	double m_pz;
	bool   m_rel; 
	bool   m_relf; 

public:
 AddForceAtPosRequest(const char *name, double x, double y, double z, double px, double py, double pz, bool rel, bool relf) : Encoder(COMM_REQUEST_ADD_FORCE_ATPOS, BUFSIZE),m_name(name), m_x(x), m_y(y), m_z(z), m_px(px), m_py(py), m_pz(pz), m_rel(rel), m_relf(relf) {}

	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


class AddTorqueRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };  //TODO: magic number
private:
	const char *m_name;
	double m_x;
	double m_y;
	double m_z;
	bool m_relf;
public:
 AddTorqueRequest(const char *name, double x, double y, double z, bool relf) : Encoder(COMM_REQUEST_ADD_TORQUE, BUFSIZE),m_name(name), m_x(x), m_y(y), m_z(z), m_relf(relf) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


class SetMassRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	double m_mass;
public:
 SetMassRequest(const char *name, double mass) : Encoder(COMM_REQUEST_SET_MASS, BUFSIZE),m_name(name), m_mass(mass) {}

	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


class GetAngularVelocityRequest : public Encoder
{
private:
	enum { BUFSIZE = 64, };
private:
	const char *m_name;
public:
 GetAngularVelocityRequest(const char *name) : Encoder(COMM_REQUEST_GET_ANGULAR_VELOCITY, BUFSIZE),m_name(name) {}

	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


// Modified by T.Inamura: m_name(name) was not be sent. Fixed on 2014-01-13
class GetAngularVelocityResult : public Encoder
{
private:
	enum { BUFSIZE = 128, };
	const char *m_name;
private:
	double m_x;
	double m_y;
	double m_z;
public:
 GetAngularVelocityResult(const char* name, double x, double y, double z) : Encoder(COMM_RESULT_GET_ANGULAR_VELOCITY, BUFSIZE), m_name(name), m_x(x), m_y(y), m_z(z) {}
	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


// Changed to LinearVelocity by Tetsunari Inamura on 2013-12-29
class GetLinearVelocityRequest : public Encoder
{
private:
	enum { BUFSIZE = 64, };
private:
	const char *m_name;
public:
 GetLinearVelocityRequest(const char *name) : Encoder(COMM_REQUEST_GET_LINEAR_VELOCITY, BUFSIZE),m_name(name) {}

	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


// Changed to LinearVelocity by Tetsunari Inamura on 2013-12-29
class GetLinearVelocityResult : public Encoder
{
 private:
	enum { BUFSIZE = 128, };
	const char *m_name;
 private:
	double m_x;
	double m_y;
	double m_z;
 public:
 GetLinearVelocityResult(const char* name, double x, double y, double z) : Encoder(COMM_RESULT_GET_LINEAR_VELOCITY, BUFSIZE), m_name(name), m_x(x), m_y(y), m_z(z) {}
  
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


// Added by Tetsunari Inamura on 2014-01-06
class SetLinearVelocityRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	double m_x;
	double m_y;
	double m_z;
public:
 SetLinearVelocityRequest(const char *name, double x, double y, double z) : Encoder(COMM_REQUEST_SET_LINEAR_VELOCITY, BUFSIZE),m_name(name), m_x(x), m_y(y), m_z(z) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};



class AddForceToPartsRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	const char *m_parts;
	double m_x;
	double m_y;
	double m_z;
public:
 AddForceToPartsRequest(const char *name, const char* parts,double x, double y, double z) : Encoder(COMM_REQUEST_ADD_FORCE_TOPARTS, BUFSIZE),m_name(name), m_parts(parts), m_x(x), m_y(y), m_z(z) {}

	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class SetGravityModeRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	bool m_gravity;
public:
 SetGravityModeRequest(const char *name, bool gravity) : Encoder(COMM_REQUEST_SET_GRAVITY_MODE, BUFSIZE),m_name(name), m_gravity(gravity) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class GetGravityModeRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
public:
 GetGravityModeRequest(const char *name) : Encoder(COMM_REQUEST_GET_GRAVITY_MODE, BUFSIZE),m_name(name) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class GetGravityModeResult : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	bool m_gravity;
public:
 GetGravityModeResult(const char *name, bool gravity) : Encoder(COMM_RESULT_GET_GRAVITY_MODE, BUFSIZE),m_name(name), m_gravity(gravity) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class SetDynamicsModeRequest : public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	const char *m_name;
	bool m_dynamics;
public:
 SetDynamicsModeRequest(const char *name, bool dynamics) : Encoder(COMM_REQUEST_SET_DYNAMICS_MODE, BUFSIZE),m_name(name), m_dynamics(dynamics) {}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};


}
typedef CommData::AddForceRequest           CommRequestAddForceEncoder;
typedef CommData::AddForceAtPosRequest      CommRequestAddForceAtPosEncoder;
typedef CommData::SetMassRequest            CommRequestSetMassEncoder;
typedef CommData::GetAngularVelocityRequest CommRequestGetAngularVelocityEncoder;
typedef CommData::GetAngularVelocityResult  CommResultGetAngularVelocityEncoder;
typedef CommData::GetLinearVelocityRequest  CommRequestGetLinearVelocityEncoder;
typedef CommData::GetLinearVelocityResult   CommResultGetLinearVelocityEncoder;
typedef CommData::SetLinearVelocityRequest  CommRequestSetLinearVelocityEncoder;
typedef CommData::AddForceToPartsRequest    CommRequestAddForceToPartsEncoder;
typedef CommData::AddTorqueRequest          CommRequestAddTorqueEncoder;
typedef CommData::SetGravityModeRequest     CommRequestSetGravityModeEncoder;
typedef CommData::GetGravityModeRequest     CommRequestGetGravityModeEncoder;
typedef CommData::GetGravityModeResult      CommResultGetGravityModeEncoder;
typedef CommData::SetDynamicsModeRequest    CommRequestSetDynamicsModeEncoder;

#endif // Comm_Encoder_JointTorque_h

 

