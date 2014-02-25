// ************************************************************
// ************************************************************

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Vector3d.h"
#include "Rotation.h"
#include "Logger.h"
#include <math.h>

class VectorMath : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(VectorMath);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void test1();

	void degAxis2Quaternion(double ax, double ay, double az, double deg, double &w, double &x, double &y, double &z);
};

CPPUNIT_TEST_SUITE_REGISTRATION(VectorMath);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

void VectorMath::setUp()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
}

void VectorMath::degAxis2Quaternion(double ax, double ay, double az, double deg, double &w, double &x, double &y, double &z)
{
	double len=sqrt(ax*ax+ay*ay+az*az);
	ax /= len;
	ay /= len;
	az /= len;

	double rad = 3.14159265*deg/180;
	double c = cos(rad/2);
	double s = sin(rad/2);

	w = c;
	x = ax*s;
	y = ay*s;
	z = az*s;
}

// ============================================================
// ============================================================
void VectorMath::test1()
{
	printf("\n");

	double ax, ay, az, deg;
	ax = 0;		
	ay = 1;
	az = 0;
	deg = -90;	

	Rotation r;
#if 1
	double w, x, y, z;
	degAxis2Quaternion(ax, ay, az, deg, w, x, y, z);
	r.setQuaternion(w, x, y, z);
#else
	r.setAxisAndAngle(ax, ay, az, deg*3.14159265/180);
#endif

	Vector3d v1(2, 0, 0);		
	Vector3d anchor(1, 0, 0);	

	Vector3d v2 = v1;
	v2 -= anchor;
	v2.rotate(r);
	v2 += anchor;

	printf("v1=(%f, %f, %f)\n", v1.x(), v1.y(), v1.z());	
	printf("v2=(%f, %f, %f)\n", v2.x(), v2.y(), v2.z());	
}

#include "../../../commonlib/tests/runner.cpp"

