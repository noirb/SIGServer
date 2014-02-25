/* $Id: Rotation.cpp,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */
#include "Rotation.h"
#include <math.h>

Rotation::Rotation()
{
#ifdef USE_ODE
	dQSetIdentity(m_q);
#endif	
	makeMatrix();
}


Rotation::Rotation(const Rotation &o)
{
	for (int i=0; i<4; i++) {
		m_q[i] = o.m_q[i];
	}
	makeMatrix();
}


#define W 0
#define X 1
#define Y 2
#define Z 3

#ifdef USE_ODE	// taku
Rotation & Rotation::operator*=(const Rotation &o)
{
#if 1
	const dReal *rq = q();
	const dReal *oq = o.q();
	const dReal w = rq[W]*oq[W] - rq[X]*oq[X] - rq[Y]*oq[Y] - rq[Z]*oq[Z];
	const dReal x = rq[W]*oq[X] + rq[X]*oq[W] + rq[Y]*oq[Z] - rq[Z]*oq[Y];
	const dReal y = rq[W]*oq[Y] - rq[X]*oq[Z] + rq[Y]*oq[W] + rq[Z]*oq[X];
	const dReal z = rq[W]*oq[Z] + rq[X]*oq[Y] - rq[Y]*oq[X] + rq[Z]*oq[W];
	setQuaternion(w, x, y, z);
#else
	dQuaternion q_;
	dQMultiply0(q_, q(), o.q());
	setQuaternion(q_[0], q_[1], q_[2], q_[3]);
#endif

	return *this;
}

Rotation & Rotation::operator=(const Rotation &o)
{
	const dReal *q = o.q();
	setQuaternion(q);
	return *this;
}
#endif
