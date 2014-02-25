#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#include "SSimObjBuilder.h"
#include "ODEWorld.h"
#include "SSimObj.h"
#include "Joint.h"
#include "Vector3d.h"
#include "FilenameDB.h"
#include "Logger.h"

class SSimObjBuilderTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SSimObjBuilderTest);
	CPPUNIT_TEST(test_applyScaleV);
	CPPUNIT_TEST(test_addParts);
	CPPUNIT_TEST(test_1);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void test_applyScaleV();
	void test_addParts();
	void test_1();

private:
	ODEWorld *m_world;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SSimObjBuilderTest);


#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

void SSimObjBuilderTest::setUp()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);

	m_world = ODEWorld::create(ODEWorld::Gravity(0, -9.8, 0), 0);
}

void SSimObjBuilderTest::test_applyScaleV()
{
	printf("\n");

	SSimObj o(NULL);
	o.push(new Attribute("scalex", new DoubleValue(0.0)));
	o.push(new Attribute("scaley", new DoubleValue(0.0)));
	o.push(new Attribute("scalez", new DoubleValue(0.0)));
	o.scalex(0.7);
	o.scaley(0.7);
	o.scalez(0.7);

	BodyFactory *f = new SSimObjBuilder(o, *m_world);
	Vector3d v(1, 1, 1);

	printf("v=(%f, %f, %f)\n", v.x(), v.y(), v.z());
	f->applyScaleV(v);
	printf("v=(%f, %f, %f)\n", v.x(), v.y(), v.z());

	delete f;

#if 0
	const char *dir = "./data";
	FilenameDB fdb;
	ASSERT(fdb.pushDirectory(dir));
#endif
}

void SSimObjBuilderTest::test_addParts()
{
	printf("\n");

	SSimObj o(NULL);
	o.push(new Attribute("scalex", new DoubleValue(0.0)));
	o.push(new Attribute("scaley", new DoubleValue(0.0)));
	o.push(new Attribute("scalez", new DoubleValue(0.0)));

	printf("*** dump(1) ***\n");
	o.dump();

	BodyFactory *f = new SSimObjBuilder(o, *m_world);

	Position pos1(1, 2, 3);
	SParts *parts1 = new SSphereParts("p1", pos1, 0.5);
	parts1->dump();
	f->addParts(parts1);

	Position pos2(10, -5, 7);
	SParts *parts2 = new SSphereParts("p2", pos2, 1.4);
	parts2->dump();
	f->addParts(parts2);

	Joint *joint1 = new BallJoint("joint1");
	f->addJoint(joint1);
	f->connect(joint1, parts1, parts2);
	f->connect2(joint1, NULL, parts1, parts2);

	printf("*** dump(2) ***\n");
	o.dump();
	SParts *p = o.getSParts("p1");
	p->dumpConnectionInfo(0);
}

void SSimObjBuilderTest::test_1()
{
	printf("\n");

	Position pos(1, 2, 3);
	SParts *parts = new SSphereParts("parts", pos, 5);

	double x1, y1, z1;
	parts->givePosition(x1, y1, z1);

	printf("(%f, %f, %f)\n", x1, y1, z1);

	Parts *p1 = (Parts *)parts;
	double x2, y2, z2;
	p1->givePosition(x2, y2, z2);

	printf("(%f, %f, %f)\n", x2, y2, z2);
}

#include "../../../commonlib/tests/runner.cpp"
