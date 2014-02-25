/*
 * Modified by Okamoto on 2011-03-25
 */

#ifndef Transform_h
#define Transform_h

#include "Rotation.h"
#include "Vector3d.h"

class Transform {
private:
	Rotation m_rot;
	Vector3d m_v;
public:
	Transform() : m_v(0.0, 0.0, 0.0) { }
	Transform(double x, double y, double z) : m_v(x, y, z) { }
	Transform(const Transform &o) : m_rot(o.m_rot), m_v(o.m_v) {;}

	void	push(const Vector3d &v) {
		m_v.shift(v);
	}
		
	void 	push(const Rotation &r)
	{
		Rotation rr(r);
		rr *= m_rot;
		m_rot = rr;
	}

	const Vector3d & translation() const { return m_v; }
	const Rotation & rotation() const { return m_rot; }

	Vector3d &apply(Vector3d &v)
	{
		v.rotate(m_rot);
		v += m_v;
		return v;
	}
};

#endif // Transform_h
 

