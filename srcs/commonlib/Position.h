/*
 * Written by Okamoto on 2011-03-31
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-29
 *    Modify indent, add English comments
 */
#ifndef Position_h
#define Position_h

#include "systemdef.h"

// added by sekikawa (2009/3/24)
// compile error occurred (memcpy() not found) when USE_ODE not defined.
#include <string.h>

#ifdef USE_ODE
#include "ode/ode.h"
#endif


/**
 * @brief 3D position class
 *
 * Represents position in the simulation world
 * Variable data type is based on ODE when using ODE, if not, data is data type is used
 */
class Position
{
private:
	enum { DIMENSION = 3,};
private:
	dReal m_values[DIMENSION];
public:
	//! Constructor
	Position() {
		set(0.0, 0.0, 0.0);
	}
	
	/**
	 * @brief Constructor
	 * @param x X-coordinate value
	 * @param y Y-coordinate value
	 * @param z Z-coordinate value
	 */
	Position(dReal x_, dReal y_, dReal z_) 
	{
		set(x_, y_, z_);
	}
	//! Copy constructor
	Position(const Position &o) 
	{
		copy(o);
	}
	/**
	 * @brief Sets each coordinate value
	 * @param x X-coordinate value
	 * @param y Y-coordinate value
	 * @param z Z-coordinate value
	 */
	void set(dReal x_, dReal y_, dReal z_)
	{
		m_values[0] = x_;
		m_values[1] = y_;
		m_values[2] = z_;
	}
	//! Gets coordinate values
	const dReal * values() { return m_values; }

	//! Gets X-coordinate value
	dReal x() const { return m_values[0]; }
	//! Gets Y-coordinate value
	dReal y() const { return m_values[1]; }
	//! Gets Z-coordinate value
	dReal z() const { return m_values[2]; }

	void x(dReal v) { m_values[0] = v; }
	void y(dReal v) { m_values[1] = v; }
	void z(dReal v) { m_values[2] = v; }

#ifdef IRWAS_TEST_CLIENT
	dReal glx() const { return x(); }
	dReal gly() const { return y(); }
	dReal glz() const { return z(); }
#endif

	//! Copy operator
	Position &operator=(const Position &o) {
		copy(o);
		return *this;
	}

private:
	void copy(const Position &o){
		memcpy(m_values, o.m_values, sizeof(m_values[0])*DIMENSION);
	}

public:
};


#endif // Position_h
 


