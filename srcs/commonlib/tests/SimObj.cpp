/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class SimObjTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SimObjTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
public:
	void test1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SimObjTest);

#include "SimObj.h"

void SimObjTest::test1()
{
	SimObj obj;
	obj.push(new Attribute("bool", new BoolValue(true)));
	obj.push(new Attribute("double", new DoubleValue(1.0)));
	obj.push(new Attribute("str", new StringValue("strvalue")));

	SimObj to;
	to.copy(obj);
	try {
		to.getAttr("bool");
		to.getAttr("double");
		to.getAttr("str");
	} catch(SimObjBase::Exception &e ) {
		printf("%s\n", e.msg());
		CPPUNIT_ASSERT(false);
	}
}

#include "runner.cpp"
