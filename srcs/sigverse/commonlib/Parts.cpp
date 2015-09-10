/* $Id: Parts.cpp,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */
#include <sigverse/commonlib/Parts.h>
#include <sigverse/commonlib/Position.h>
#include <sigverse/commonlib/Size.h>
#include <assert.h>

Parts::Id Parts::s_cnt = 0;

void Parts::free_()
{
	delete [] m_buf; m_buf = 0;
}


const dReal *Parts::getPosition()
{
	return m_pos.values();
}

const dReal * Parts::getRotation()
{
#ifdef USE_ODE  // taku
	return m_rot.matrix();
#else
	return NULL;
#endif
}

const dReal * Parts::getQuaternion()
{
	return m_rot.q();
}

// begin(add)(sekikawa)(2007/11/30)
void Parts::givePosition(double &x, double &y, double &z)
{
	x = (double)m_pos.x();
	y = (double)m_pos.y();
	z = (double)m_pos.z();
}

void Parts::giveQuaternion(double &qw, double &qx, double &qy, double &qz)
{
	const dReal *q = m_rot.q();
	
	qw = (double)(q[0]);
	qx = (double)(q[1]);
	qy = (double)(q[2]);
	qz = (double)(q[3]);
}
// end


void Parts::dump()
{
	char * typestrs[] = { "box", "cylinder", "sphere", };
	printf("\tparts(%d): %s(%s) (%f, %f, %f)\n", id(), name(), typestrs[m_type],  m_pos.x(), m_pos.y(), m_pos.z());
}
