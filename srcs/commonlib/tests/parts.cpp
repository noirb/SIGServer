/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class PartsTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(PartsTest);
	CPPUNIT_TEST(test_Box);
	CPPUNIT_TEST(test_Cylinder);
	CPPUNIT_TEST(test_Sphere);
	CPPUNIT_TEST_SUITE_END();

public:
	void test_Box();
	void test_Cylinder();
	void test_Sphere();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PartsTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL
#define ASSERT_D_EQUAL CPPUNIT_ASSERT_DOUBLES_EQUAL

#include "CParts.h"
#include "PartsCmpnt.h"

const double EPS = 0.00001;

void PartsTest::test_Box()
{
	Position pos(0.0, 1.0, 2.0);
	Size sz(3.0, 4.0, 5.0);
	BoxParts box("box", pos, sz);

	Parts &parts = box;
	int n;
	char *data= parts.toBinary(n);
	ASSERT(data != NULL);

	{
		CParts * d = CParts::decode(data+2);
		ASSERT(d != NULL);

		ASSERT(strcmp(d->name(), "box") == 0);
		ASSERT(box.getType() == d->getType());
		BoxPartsCmpnt *ext = (BoxPartsCmpnt*)( d->extdata());
		ASSERT(ext != NULL);
		Size &sz_ = ext->size();
		ASSERT_D_EQUAL(sz_.x(), sz.x(), EPS);
		ASSERT_D_EQUAL(sz_.y(), sz.y(), EPS);
		ASSERT_D_EQUAL(sz_.z(), sz.z(), EPS);
		delete d;
	}

	{
		// clone test
		CParts *clone = ( (CParts&)box ).clone();
		ASSERT(clone != NULL);
		ASSERT(strcmp(clone->name(), "box") == 0);
		ASSERT(box.getType() == clone->getType());
		
		BoxPartsCmpnt *ext = (BoxPartsCmpnt*)( clone->extdata());
		ASSERT(ext != NULL);
		Size &sz_ = ext->size();
		ASSERT_D_EQUAL(sz_.x(), sz.x(), EPS);
		ASSERT_D_EQUAL(sz_.y(), sz.y(), EPS);
		ASSERT_D_EQUAL(sz_.z(), sz.z(), EPS);
		delete clone;
	}
}


void PartsTest::test_Cylinder()
{
	Position pos(0.0, 1.0, 2.0);
	double rad = 0.1;
	double len = 1.5;
	CylinderParts cylinder("cylinder", pos, rad, len);

	Parts &parts = cylinder;
	int n;
	char *data= parts.toBinary(n);
	ASSERT(data != NULL);

	CParts * d = CParts::decode(data+2);
	ASSERT(d != NULL);

	ASSERT(strcmp(d->name(), "cylinder") == 0);
	ASSERT(cylinder.getType() == d->getType());
	CylinderPartsCmpnt *ext = (CylinderPartsCmpnt*)( d->extdata());
	ASSERT(ext != NULL);
	ASSERT_D_EQUAL(rad, ext->radius(), EPS);
	ASSERT_D_EQUAL(len, ext->length(), EPS);
	delete d;
}

void PartsTest::test_Sphere()
{
	Position pos(0.0, 1.0, 2.0);
	const double RADIUS = 5.0;

	SphereParts sphere("sphere", pos, RADIUS);
	Parts &parts = sphere;

	int n;
	char *data = parts.toBinary(n);
	ASSERT(data != NULL);

	{
		CParts * d = CParts::decode(data+2);
		ASSERT(d != NULL);

		ASSERT(strcmp(d->name(), "sphere") == 0);
		ASSERT(sphere.getType() == d->getType());
	
		SpherePartsCmpnt *ext = (SpherePartsCmpnt*)d->extdata();
		ASSERT(ext != NULL);
		ASSERT_D_EQUAL(RADIUS, ext->radius(), EPS);
		delete d;
	}

	{
		CParts *clone = ( (CParts&)parts ).clone();
		ASSERT(clone != NULL);

		ASSERT(strcmp(clone->name(), "sphere") == 0);
		ASSERT(sphere.getType() == clone->getType());
	
		SpherePartsCmpnt *ext = (SpherePartsCmpnt*)clone->extdata();
		ASSERT(ext != NULL);
		ASSERT_D_EQUAL(RADIUS, ext->radius(), EPS);
		delete clone;
	}
}

#include "runner.cpp"

