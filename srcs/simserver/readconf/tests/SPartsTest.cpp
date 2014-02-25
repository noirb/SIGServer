#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#include "SSimObjBuilder.h"
#include "ODEWorld.h"
#include "SSimObj.h"
#include "Joint.h"
#include "Vector3d.h"
#include "Logger.h"

class SPartsTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SPartsTest);
	CPPUNIT_TEST(test_calcPosition);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void test_calcPosition();

private:
	ODEWorld *m_world;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SPartsTest);


#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

void SPartsTest::setUp()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);

	m_world = ODEWorld::create(ODEWorld::Gravity(0, -9.8, 0), 0);
}

void SPartsTest::test_calcPosition()
{
	printf("\n");

	SSimObj o(NULL);

	o.push(new Attribute("scalex", new DoubleValue(0.0)));
	o.push(new Attribute("scaley", new DoubleValue(0.0)));
	o.push(new Attribute("scalez", new DoubleValue(0.0)));

	BodyFactory *f = new SSimObjBuilder(o, *m_world);


	SParts *parent = new SSphereParts("parentParts", Position(0, 0, 0), 1);

	SParts *child = new SSphereParts("childParts", Position(2, 0, 0), 1);

	Joint *joint = new BallJoint("joint");

	f->addParts(parent);
	f->addParts(child);
	f->addJoint(joint);

	printf("**** dump(1) ****\n");
	parent->dumpConnectionInfo(0);

	f->connect(joint, parent, child);
	f->connect2(joint, NULL, parent, child);

	printf("**** dump(2) ****\n");
	parent->dumpConnectionInfo(0);
}

#include "../../../commonlib/tests/runner.cpp"

