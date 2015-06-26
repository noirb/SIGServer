/* 
 * Written by okamoto on 2011-12-28
 * Modified by Tetsunari Inamura on 2013-12-30
 *    change GetVelocity to GetLinearVelocity
 *    no need to translate comments
 * Added SetLinearVelocity by Tetsunari Inamura on 2014-01-06
 * Added addTorque         by Tetsunari Inamura on 2014-02-26
 */

#ifndef MoveControl_h
#define MoveControl_h

#include "Logger.h"

class RequestAddForceEvent 
{
private:
	const char* m_name;
	double      m_x;
	double      m_y;
	double      m_z;
	bool        m_relf;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestAddForceEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}

	double x() { return m_x; }
	double y() { return m_y; }
	double z() { return m_z; }
	bool rel() { return m_relf;}
};


class RequestAddForceAtPosEvent 
{
private:
	const char* m_name;
	double m_x;
	double m_y;
	double m_z;
	double m_px;
	double m_py;
	double m_pz;
	bool   m_rel;
	bool   m_relf;
public:
	bool   set(int packetNum, int seq, char *data, int n);
public:
	~RequestAddForceAtPosEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}
	double x() { return m_x; }
	double y() { return m_y; }
	double z() { return m_z; }
	
	double px() { return m_px; }
	double py() { return m_py; }
	double pz() { return m_pz; }
	
	bool rel()  { return m_rel; }
	bool relf() { return m_relf; }
};


// Added by Tetsunari Inamura on 2014-02-26
class RequestAddTorqueEvent 
{
private:
	const char* m_name;
	double      m_x;
	double      m_y;
	double      m_z;
	bool        m_relf;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestAddTorqueEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}
	double x() { return m_x; }
	double y() { return m_y; }
	double z() { return m_z; }
	bool rel() { return m_relf;}
};


class RequestSetMassEvent 
{
private:
	const char* m_name;
	double m_mass;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestSetMassEvent() {
		delete m_name;
	}
	const char * getAgentName() { 
		return m_name;
	}
	double mass() { return m_mass; }
};


class RequestGetAngularVelocityEvent 
{
private:
	const char* m_name;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestGetAngularVelocityEvent() {
		delete m_name;
	}
	const char * getAgentName() { 
		return m_name;
	}
};


class ResultGetAngularVelocityEvent 
{
private:
	const char* m_name;
	double      m_x;
	double      m_y;
	double      m_z;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~ResultGetAngularVelocityEvent() {
		delete m_name;
    }
	const char * getAgentName() { 
		return m_name;
	}
	double x(){return m_x;}
	double y(){return m_y;}
	double z(){return m_z;}
};


// Modified by Tetsunari Inamura: change GetVelocity to GetLinearVelocity
class RequestGetLinearVelocityEvent 
{
private:
	const char* m_name;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestGetLinearVelocityEvent() {
		delete m_name;
	}

	const char * getAgentName() { 
		return m_name;
	}
};


// Modified by Tetsunari Inamura: change GetVelocity to GetLinearVelocity
class ResultGetLinearVelocityEvent 
{
private:
	const char* m_name;
	double      m_x;
	double      m_y;
	double      m_z;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~ResultGetLinearVelocityEvent() {
		delete m_name;
	}
	const char * getAgentName() { 
		return m_name;
	}
	double x(){return m_x;}
	double y(){return m_y;}
	double z(){return m_z;}
};


// Add by inamura on 2014-01-06
class RequestSetLinearVelocityEvent 
{
private:
	const char* m_name;
	double      m_x;
	double      m_y;
	double      m_z;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestSetLinearVelocityEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}
	double x() { return m_x; }
	double y() { return m_y; }
	double z() { return m_z; }
};




class RequestAddForceToPartsEvent 
{
private:
	const char* m_name;
	const char* m_parts;
	double      m_x;
	double      m_y;
	double      m_z;

public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestAddForceToPartsEvent() {
		delete m_name;
		delete m_parts;
	}
	
	const char * getAgentName() { 
		return m_name;
	}
	
	const char * getPartsName() { 
		return m_parts;
	}
	double x() { return m_x; }
	double y() { return m_y; }
	double z() { return m_z; }
};


class RequestSetGravityModeEvent 
{
private:
	const char* m_name;
	bool m_gravity;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestSetGravityModeEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}
	
	bool grav() { return m_gravity; }
};


class RequestGetGravityModeEvent 
{
private:
	const char* m_name;
	bool m_gravity;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestGetGravityModeEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}
	
	bool grav() { return m_gravity; }
};


class ResultGetGravityModeEvent 
{
private:
	const char* m_name;
	bool m_gravity;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~ResultGetGravityModeEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}

	bool grav() { return m_gravity;}
};


class RequestSetDynamicsModeEvent 
{
private:
	const char* m_name;
	bool m_dynamics;
public:
	bool set(int packetNum, int seq, char *data, int n);
public:
	~RequestSetDynamicsModeEvent() {
		delete m_name;
	}
	
	const char * getAgentName() { 
		return m_name;
	}
	bool dynamics() { return m_dynamics;}
};


#endif // MoveControl_h
 

