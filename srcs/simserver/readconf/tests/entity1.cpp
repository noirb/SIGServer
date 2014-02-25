#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

class EntityTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(EntityTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown() {}

	void test1();
};


CPPUNIT_TEST_SUITE_REGISTRATION(EntityTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include <xercesc/util/PlatformUtils.hpp>

XERCES_CPP_NAMESPACE_USE;

void EntityTest::setUp()
{
	XMLPlatformUtils::Initialize();
}


#include "EntityXMLReader.h"
#include "SSimObj.h"
#include "ODEWorld.h"
#include "X3DDB.h"
#include "XMLUtils.h"
#include "Logger.h"
#include "FilenameDB.h"

void EntityTest::test1()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
	const char *dir = "/home/yoshi/proj/irwas/work/fromnii-091127/source";
	FilenameDB fdb;
	ASSERT(fdb.pushDirectory(dir));
	
	const char *fname = "MyWorld.xml";
	SSimObj obj(NULL);
	X3DDB db;
	ODEWorld *w = ODEWorld::create(ODEWorld::Gravity(0, -9.8, 0), 0);
	try {
		EntityXMLReader read(fdb, obj, *w, db);
		bool b = read(fname);
		ASSERT(b);
	} catch(XMLUtils::Exception &e) {
		printf("XMLUtils::Exception : %s\n", e.msg());
		ASSERT(false);
	} catch(SimObjBase::Exception &e) {
		printf("SimObj::Exception : %s\n", e.msg());
		ASSERT(false);
	}
}

#include "../../../commonlib/tests/runner.cpp"


