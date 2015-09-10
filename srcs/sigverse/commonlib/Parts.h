/*
 * Written on 2011-05-12
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-29
 *    Modify indent, add English comments
 */
#ifndef Parts_h
#define Parts_h

#include "systemdef.h"

#ifdef USE_ODE
#include <ode/ode.h>
#endif

#include <string>
#include <stdio.h>

#include "Position.h"
#include "Size.h"
#include "Rotation.h"

class ODEObj;
class ODEWorld;
class SimObjBase;

enum {
	PARTS_TYPE_NOT_SET = -1,
	PARTS_TYPE_BOX,
	PARTS_TYPE_CYLINDER,
	PARTS_TYPE_SPHERE,
	PARTS_TYPE_NUM,
};

typedef short PartsType;

/**
 * @brief Interface of component parts
 */
class PartsCmpnt
{
public:
	//! Destructor
	virtual ~PartsCmpnt() {};

	//! Gets data length when binary translation
	virtual int datalen() = 0;

	//! Gets binary data
	virtual char *dataBinary() = 0;

	//! Shows holding information (data size)
	virtual void dump() = 0;    // FIX20110421(ExpSS)
};

/**
 * @brief Parts class constituting an entity
 */
class Parts
{
public:
	typedef unsigned Id;
private:
	static Id s_cnt;
public:
	static void initCounter() { s_cnt = 0; }
protected:
	PartsType   m_type;
	std::string m_name;

	Position m_pos;
	Rotation m_rot;

	Id   m_id;
	bool m_blind;

	char * m_buf;
	int    m_bufsize;
private:
	void free_();

protected:
	Parts(PartsType t, const char *name, const Position &pos)
		: m_type(t), m_name(name), m_pos(pos),
		  m_id(0), m_blind(false), m_buf(NULL), m_bufsize(0) {}

	Parts(const Parts &o) :
		m_type(o.m_type), m_name(o.m_name),
		m_pos(o.m_pos), m_rot(o.m_rot),
		m_id(o.m_id), m_blind(o.m_blind),
		m_buf(NULL), m_bufsize(0) {}
public:
	//! Destructor
	virtual ~Parts() {	free_(); }
public:
	/**
	 * @brief Sets ID
	 *
	 * ID is unique number for parts
	 */
	void setId(Id id) { m_id = id; }

	/**
	 * @brief Sets ID automatically
	 *
	 * ID is unique number for parts
	 */
	void addId() {
		s_cnt++;
		m_id = s_cnt;
	}
	//! Gets ID
	Id id() { return m_id; }

	//! Gets a part type
	PartsType getType() { return m_type; }

	//! Gets a part name
	const char *name(){ return m_name.c_str(); }

	//! Returns whether a part is body (a component that constitutes the center of an entity)
	bool isBody() const {
		return strcmp(m_name.c_str(), "body") == 0? true: false;
	}

	//! Returns whether a part is invisible
	bool isBlind() const { return m_blind; }

protected:
	void setBlind(bool b) { m_blind = b; }

public:
	//! Sets quatanion
	virtual void setQuaternion(dReal q0, dReal q1, dReal q2, dReal q3) = 0;

	//! Gets position
	virtual const dReal * getPosition() = 0;

	//! Gets rotation matrix
	virtual const dReal * getRotation() = 0;

	//! Gets quatanion
	virtual const dReal * getQuaternion() = 0;

	//! Sets position
	void givePosition(double &x, double &y, double &z);

	//! Sets quatanion
	void giveQuaternion(double &qw, double &qx, double &qy, double &qz);

	/**
	 * @brief Binary translation of parts objects
	 *
	 * @param n binary data size
	 * @return  binary data
	 */
	char * toBinary(int &n);

public:
	virtual PartsCmpnt * extdata() = 0;

	// Dump method (for debug)
	virtual void	dump();

#ifdef IMPLEMENT_DRAWER
	virtual void draw(DrawContext &c) = 0;
#endif

};

#endif // Parts_h



