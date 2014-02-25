/* $Id: SimViewPoint.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "SimViewPoint.h"
#include "systemdef.h"

#ifdef USE_OPENGL

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static double inRange(double v)
{
	return v;
}

void SimViewPoint::draw(GLContext &c)
{
	//	printf("viewpoint : (%f, %f, %f)\n", m_pos.x(), m_pos.y(), m_pos.z());
	glRotated(m_roty, 0.0, 1.0, 0.0);
	glRotated(m_rotz, 1.0, 0.0, 0.0);
	glTranslated(m_pos.glx(), m_pos.gly(), m_pos.glz());
}

bool SimViewPoint::setKey(unsigned char key)
{
	bool updated = true;
	switch(key) {
	case 'l': // right rotate
		m_roty += 2;
		break;
	case 'h':// left rotate
		m_roty -= 2;
		break;
	case 'j':
		m_rotz += 2;
		break;
	case 'k':
		m_rotz -= 2;
		break;

	case 'f':
		{
			Position &p = m_pos;
			double rad = -m_roty*M_PI/180;
			double dx = 0.2*sin(rad);
			double dz = 0.2*cos(rad);
			p.x(inRange(p.x() + dx));
			p.z(inRange(p.z() + dz));
			break;
		}

	case 'd':
		{
			Position &p = m_pos;
			double rad = -m_roty*M_PI/180;
			double dx = 0.2*sin(rad);
			double dz = 0.2*cos(rad);
			p.x(inRange(p.x() - dx));
			p.z(inRange(p.z() - dz));
			break;
		}
	case 'q':
		exit(0);
	default:
		updated = false;
		break;
	}
	return updated;
}
#endif
