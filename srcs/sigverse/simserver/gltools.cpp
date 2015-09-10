/* $Id: gltools.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include <sigverse/commonlib/systemdef.h>
#ifdef USE_OPENGL
#include <GL/glut.h>
#include <math.h>

void glBox(double x, double y, double z)

	GLdouble hx = x*0.5;
	GLdouble hy = y*0.5;
	GLdouble hz = z*0.5;

	GLdouble vertex[][3] = {
		{-hx, -hy, -hz},
		{hx, -hy, -hz},
		{hx, hy, -hz},
		{-hx, hy, -hz},
		{-hx, -hy, hz},
		{hx, -hy, hz},
		{hx, hy, hz},
		{-hx, hy, hz},
	};

	static int face[][4] = {
		{ 0, 1, 2, 3},
		{ 1, 5, 6, 2},
		{ 5, 4, 7, 6},
		{ 4, 0, 3, 7},
		{ 4, 5, 1, 0},
		{ 3, 2, 6, 7},
	};

	static GLdouble normal[][3] = {
		{ 0.0, 0.0, -1.0},
		{ 1.0, 0.0,  0.0},
		{ 0.0, 0.0,  1.0},
		{ -1.0, 0.0, 0.0},
		{ 0.0, -1.0, 0.0},
		{ 0.0, 1.0, 0.0},
	};

	static GLfloat red[] = {0.8, 0.2, 0.2, 1.0};

	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);

	glBegin(GL_QUADS);
	for (int j=0; j<6; j++) {
		glNormal3dv(normal[j]);
		for (int i=4-1; i>=0; i--) {
			glVertex3dv(vertex[face[j][i]]);
		}
	}

	glEnd();
}


void glArmLeg(double girth, double length, double r1, double r2)
{
	glRotated(r1, 1.0, 0.0, 0.0);
	glBox(girth, length, girth);

	glTranslated(0.0, -0.05-length, 0.0);
	glRotated(r2, 1.0, 0.0, 0.0);
	glBox(girth, length, girth);
}


void glCylinder(double r, double l)
{
	int n = 24;
	l *= 0.5;

	GLdouble a  = M_PI*2.0/n;
	GLdouble sa = sin(a);
	GLdouble ca = cos(a);

	// body
	{
		GLdouble ny = 1.0, nz = 0.0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i<=n; i++) {
			glNormal3d(ny, nz, 0);
			glVertex3d(ny*r, nz*r, l);
			glNormal3d(ny, nz, 0);
			glVertex3d(ny*r, nz*r, -l);
			GLdouble tmp = ca*ny - sa*nz;
			nz = sa*ny + ca*nz;
			ny = tmp;
		}
		glEnd();
	}

		
	GLdouble start_nx = 0.0;
	GLdouble start_ny = 1.0;
	for (int j=0; j<(n/4); j++) {
		GLdouble start_nx2 = ca*start_nx + sa*start_ny;
		GLdouble start_ny2 = -sa*start_nx + ca*start_ny;
		GLdouble nx = start_nx;
		GLdouble ny = start_ny;;
		GLdouble nz = 0;
		GLdouble nx2 = start_nx2;
		GLdouble ny2 = start_ny2;
		GLdouble nz2 = 0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i<=n; i++) {
			glNormal3d(ny2, nz2, nx2);
			glVertex3d(ny2*r, nz2*r, l + nx2*r);
			glNormal3d(ny, nz, nx);
			glVertex3d(ny*r, nz*r, l + nx*r);
			// rotate
			GLdouble tmp;
			tmp = ca*ny - sa*nz;
			nz = sa*ny + ca*nz;
			ny = tmp;

			tmp = ca*ny2 - sa*nz2;
			nz2 = sa*ny2 + ca*nz2;
			ny2 = tmp;
			
		}
		glEnd();
		start_nx = start_nx2;
		start_ny = start_ny2;
	}

	for (int j=0; j<(n/4); j++) {
		GLdouble start_nx2 = ca*start_nx - sa*start_ny;
		GLdouble start_ny2 = sa*start_nx + ca*start_ny;
		GLdouble nx = start_nx;
		GLdouble ny = start_ny;;
		GLdouble nz = 0;
		GLdouble nx2 = start_nx2;
		GLdouble ny2 = start_ny2;
		GLdouble nz2 = 0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i<=n; i++) {
			glNormal3d(ny, nz, nx);
			glVertex3d(ny*r, nz*r, -l + nx*r);
			glNormal3d(ny2, nz2, nx2);
			glVertex3d(ny2*r, nz2*r, -l + nx2*r);
			// rotate
			GLdouble tmp;
			tmp = ca*ny - sa*nz;
			nz = sa*ny + ca*nz;
			ny = tmp;

			tmp = ca*ny2 - sa*nz2;
			nz2 = sa*ny2 + ca*nz2;
			ny2 = tmp;
			
		}
		glEnd();
		start_nx = start_nx2;
		start_ny = start_ny2;
	}

}
#endif
