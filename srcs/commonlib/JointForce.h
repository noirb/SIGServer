/*
 * Written by Okamoto on 2011-03-31
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-29
 *    Add English comments
 *
 */
#ifndef JointForce_h
#define JointForce_h

#include <assert.h>
#include <string>

#include "Vector3d.h"

/**
 * @brief Class for force acting on parts
 */
class JointForce
{
private:
	std::string m_partsName;
	Vector3d    m_f;
	Vector3d    m_tq;
public:
	//! Gets a part name
	const char *getPartsName() { return m_partsName.c_str(); }
	
	//! Gets force acting on a part
	const Vector3d &getForce() { return m_f; }

	//! Gets torque acting on a part
	const Vector3d &getTorque() { return m_tq; }
	
	//! Sets each value
	void set(const char *partsName, const Vector3d &f, const Vector3d &tq)
	{
		assert(partsName != NULL);
		m_partsName = partsName;
		m_f = f;
		m_tq = tq;
	}
	
	//! Copy operator
	JointForce & operator=(const JointForce &o)
	{
		m_partsName = o.m_partsName;
		m_f = o.m_f;
		m_tq = o.m_tq;
		return *this;
	}
};



#endif // JointForce_h
 


