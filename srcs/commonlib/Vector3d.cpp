/*
 * Written by okamoto on 2011-03-31
 */

#include "Vector3d.h"
#include "Rotation.h"
#include "Logger.h"

//#ifndef IRWAS_OGRE_CLIENT
#if (defined EXEC_SIMULATION || defined IRWAS_TEST_CLIENT || defined CONTROLLER)
Vector3d & Vector3d::rotate(const Rotation &r)
{
	Vector3d rv;
	r.apply(*this, rv);
	*this = rv;
	return *this;
}

#endif

/*
void Vector3d::rotateByQuaternion(double qw, double qx, double qy, double qz)
{
	  //LOG_MSG(("test1"));
  Vector3d rv;
	  LOG_MSG(("(%f, %f, %f, %f)", qw, qx, qy, qz));

	  Rotation r(qw, qx, qy, qz);
	  //r.apply(*this, rv);
	  //*this = rv;
	  //r.apply(*this, rv);
	  //*this = rv;

}

*/
