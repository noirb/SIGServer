/* $Id: GLDraw.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include "systemdef.h"

#ifdef USE_OPENGL

#include "SimObjBase.h"
#include "CParts.h"
#include "PartsCmpnt.h"

#include <GL/glut.h>
#include "gltools.h"

#define DEF_R(ARY, POS, M) \
	GLdouble ARY[] = { \
		M[0], M[4], M[8],  0.0, \
		M[1], M[5], M[9], 0.0, \
		M[2], M[6], M[10],  0.0, \
		POS.glx(), POS.gly(), POS.glz(), 1.0,\
	};

void SimObjBase::draw(GLContext &c)
{
	PartsIterator *itr = getPartsIterator();
	Parts *p = NULL;
	while (itr && (p = itr->next()) != NULL) {
		glPushMatrix();
		p->draw(c);
		glPopMatrix();
	}
	delete itr;

	/*
	printf("Agent(%d) :  %s\n", id(), name());
	printf("(%f, %f, %f)\n", x(), y(), z());
	printf("\t : (%f, %f, %f, %f)\n", r00(), r01(), r02(), r03());
	printf("\t : (%f, %f, %f, %f)\n", r10(), r11(), r12(), r13());
	printf("\t : (%f, %f, %f, %f)\n", r20(), r21(), r22(), r23());
	for (PartsM::iterator i=m_parts.begin(); i!=m_parts.end(); i++) {
		Parts *p = i->second;
		p->dump();
	}
	*/
}

void BoxParts::draw(GLContext &c)
{
	const dReal * m = getRotation();

	// R(gl)
	const dReal *pos_ = getPosition();
	Position pos(pos_[0], pos_[1], pos_[2]);


	DEF_R(r, pos, m);

	glMultMatrixd(r);
	Size &sz = m_cmpnt->size();

	glBox(sz.glx(), sz.gly(), sz.glz());
}

void CylinderParts::draw(GLContext &c)
{
	const dReal * m = getRotation();

	// R(gl)
	const dReal *pos_ = getPosition();
	Position pos(pos_[0], pos_[1], pos_[2]);

	DEF_R(r, pos, m);

	glMultMatrixd(r);

	double radius = m_cmpnt->radius();
	double length = m_cmpnt->length();
	glCylinder(radius, length);
}

void SphereParts::draw(GLContext &c)
{
	const dReal * m = getRotation();

	// R(gl)
	const dReal *pos_ = getPosition();
	Position pos(pos_[0], pos_[1], pos_[2]);

	DEF_R(r, pos, m);

	glMultMatrixd(r);

	double radius = m_cmpnt->radius();

	GLUquadricObj *quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluSphere(quad, radius, 10, 10);
	gluDeleteQuadric(quad);
}

#endif



