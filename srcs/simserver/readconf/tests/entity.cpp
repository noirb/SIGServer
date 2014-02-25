#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

class EntityTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(EntityTest);
	CPPUNIT_TEST(test_x3d);
	CPPUNIT_TEST(test_noParent);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown() {}

	void test_x3d();
	void test_noParent();
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

void EntityTest::test_x3d()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
	const char *dir = "./data";
	FilenameDB fdb;
	ASSERT(fdb.pushDirectory(dir));
	
	const char *fname = "Robot-x3d.xml";
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
					 

	{
		const char *p = db.get("Robot::eating:false");
		ASSERT(p != NULL);
		ASSERT(strcmp("robot-body.wrl", p) == 0);
	}
	{
		const char *p = db.get("Robot::eating:true");
		ASSERT(p != NULL);
		ASSERT(strcmp("robot-body-eating.wrl", p) == 0);
	}

	{
		const char *p = db.get("Robot:L_ARM::");
		ASSERT(p != NULL);
		ASSERT(strcmp("robot-l-arm.wrl", p) == 0);
	}
}

void EntityTest::test_noParent()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
	const char *dir = "./data";
	FilenameDB fdb;
	ASSERT(fdb.pushDirectory(dir));
	
	const char *fname = "Robot-NoParent.xml";
	SSimObj obj(NULL);
	X3DDB db;
	ODEWorld *w = ODEWorld::create(ODEWorld::Gravity(0, -9.8, 0), 0);
	try {
		EntityXMLReader read(fdb, obj, *w, db);
		bool b = read(fname);
		ASSERT(!b);
	} catch(XMLUtils::Exception &e) {
		printf("XMLUtils::Exception : %s\n", e.msg());
		ASSERT(false);
	} catch(SimObjBase::Exception &e) {
		printf("SimObjBase::Exception : %s\n", e.msg());
		ASSERT(true);
	}
}


#include "../../../commonlib/tests/runner.cpp"


