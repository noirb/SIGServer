/*
 * Written by okamoto on 2011-03-31
 */

#include <sigverse/commonlib/Vector3d.h>
#include <sigverse/commonlib/Rotation.h>
#include <sigverse/commonlib/Logger.h>

//#ifndef IRWAS_OGRE_CLIENT

Vector3d & Vector3d::rotate(const Rotation &r)
{
	Vector3d rv;
	r.apply(*this, rv);
	*this = rv;
	return *this;
}


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
