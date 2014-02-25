#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class BodyTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(BodyTest);
	CPPUNIT_TEST(test);
	CPPUNIT_TEST(testArm);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown() {}

	void test();
	void testArm();

};

CPPUNIT_TEST_SUITE_REGISTRATION(BodyTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "Parts.h"
#include "SSimObj.h"
#include "ODEWorld.h"
#include "BodyXMLReader.h"
#include "XMLUtils.h"

XERCES_CPP_NAMESPACE_USE;

void BodyTest::setUp()
{
	Parts::initCounter();
	XMLPlatformUtils::Initialize();
}

void BodyTest::test()
{

	char *fname = "data/robot-body.xml";
	SSimObj obj(NULL);
	ODEWorld *w = ODEWorld::create(ODEWorld::Gravity(0, -9.8, 0), 0);
	BodyXMLReader r(obj, *w);
	r.read(fname);

	obj.push(new Attribute("name", new StringValue("TestAgent1")));
	obj.push(new Attribute("classname", new StringValue("TestAgentClass")));
	obj.push(new Attribute("dynamics", new BoolValue(true)));
	obj.push(new Attribute("x", new DoubleValue(0.0)));
	obj.push(new Attribute("y", new DoubleValue(0.0)));
	obj.push(new Attribute("z", new DoubleValue(0.0)));
	obj.push(new Attribute("qw", new DoubleValue(0.0)));
	obj.push(new Attribute("qx", new DoubleValue(0.0)));
	obj.push(new Attribute("qy", new DoubleValue(0.0)));
	obj.push(new Attribute("qz", new DoubleValue(0.0)));
	obj.push(new Attribute("fx", new DoubleValue(0.0)));
	obj.push(new Attribute("fy", new DoubleValue(0.0)));
	obj.push(new Attribute("fz", new DoubleValue(0.0)));


	obj.dump();
	SParts *p = obj.getSBody();
	ASSERT(p != NULL);
	try {
		obj.applyParams(true);
		ASSERT(true);
	} catch(XMLUtils::Exception &e) {
		printf("XMLUtils::Exception : %s\n", e.msg());
		ASSERT(false);
	} catch(SimObjBase::Exception &e ) {
		fprintf(stderr, "Exception : %s\n", e.msg());
		ASSERT(false);
	} catch(const XMLException &e) {
		char *msg = XMLString::transcode(e.getMessage());
		fprintf(stderr, "%s", msg);
		XMLString::release(&msg);
	}

}

void BodyTest::testArm()
{

	char *fname = "data/arm-body.xml";
	SSimObj obj(NULL);
	ODEWorld *w = ODEWorld::create(ODEWorld::Gravity(0, -9.8, 0), 0);
	BodyXMLReader r(obj, *w);
	r.read(fname);

	obj.push(new Attribute("name", new StringValue("TestAgent1")));
	obj.push(new Attribute("classname", new StringValue("TestAgentClass")));
	obj.push(new Attribute("dynamics", new BoolValue(true)));
	obj.push(new Attribute("x", new DoubleValue(0.0)));
	obj.push(new Attribute("y", new DoubleValue(0.0)));
	obj.push(new Attribute("z", new DoubleValue(0.0)));
	obj.push(new Attribute("qw", new DoubleValue(0.0)));
	obj.push(new Attribute("qx", new DoubleValue(0.0)));
	obj.push(new Attribute("qy", new DoubleValue(0.0)));
	obj.push(new Attribute("qz", new DoubleValue(0.0)));
	obj.push(new Attribute("fx", new DoubleValue(0.0)));
	obj.push(new Attribute("fy", new DoubleValue(0.0)));
	obj.push(new Attribute("fz", new DoubleValue(0.0)));


	obj.dump();
	SParts *p = obj.getSBody();
	ASSERT(p != NULL);
	try {
		obj.applyParams(true);
		ASSERT(true);
	} catch(XMLUtils::Exception &e) {
		printf("XMLUtils::Exception : %s\n", e.msg());
		ASSERT(false);
	} catch(SimObjBase::Exception &e ) {
		fprintf(stderr, "Exception : %s\n", e.msg());
		ASSERT(false);
	} catch(const XMLException &e) {
		char *msg = XMLString::transcode(e.getMessage());
		fprintf(stderr, "%s", msg);
		XMLString::release(&msg);
	}

}

#include "../../../commonlib/tests/runner.cpp"
