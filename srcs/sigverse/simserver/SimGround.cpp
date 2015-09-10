#include "SimGround.h"

#ifdef USE_OPENGL
#include <GL/glut.h>

#define LRANGE -50
#define URANGE 50

static void myGround(double h)
{
	glPushMatrix();
	GLfloat panels[][4] = {
		{0.6, 0.6, 0.6, 1.0},
		{0.3, 0.3, 0.3, 1.0},
	};
	glBegin(GL_QUADS);

	for (int j=LRANGE; j<=URANGE; j++) {
		for (int i=LRANGE; i<=URANGE; i++) {
			glMaterialfv(GL_FRONT, GL_DIFFUSE, panels[(i+j)&1]);
			glVertex3d((GLdouble)i, h, (GLdouble)j);
			glVertex3d((GLdouble)i, h, (GLdouble)(j+1));
			glVertex3d((GLdouble)(i+1), h, (GLdouble)(j+1));
			glVertex3d((GLdouble)(i+1), h, (GLdouble)j);
		}
	}
	
	glEnd();
	glPopMatrix();
}

void SimGround::draw(GLContext &c)
{
	myGround(0.0);
}

#endif // USE_OPENGL
