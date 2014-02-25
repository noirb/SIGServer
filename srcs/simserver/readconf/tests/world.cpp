
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class WorldTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(WorldTest);
	CPPUNIT_TEST(test);
	CPPUNIT_TEST(test_NoWorld);
	CPPUNIT_TEST(test_BadWorld1);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void test();
	void test_NoWorld();
	void test_BadWorld1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(WorldTest);


#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include "WorldXMLReader.h"
#include "SSimWorld.h"
#include "X3DDB.h"
#include "FilenameDB.h"
#include "Logger.h"

#if 1
// FIX20110421(ExpSS)
#include "SimpleShapeDB.h"
#endif

void WorldTest::setUp()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
}

void WorldTest::test()
{
	/*
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
	const char *dir = "../../conf";
	FilenameDB fdb;
	ASSERT(fdb.pushDirectory(dir));
	X3DDB db;
	WorldXMLReader r(fdb, db);
	bool b = r.read("MyWorld.xml");
	ASSERT(b);

	SSimWorld *w = r.release();
	ASSERT(w != NULL);
	w->setup();
	w->dump();
	delete w;
	*/

}


void WorldTest::test_NoWorld()
{
	const char *dir = "./data";
	FilenameDB fdb;
	ASSERT(fdb.pushDirectory(dir));
	X3DDB db;
#if 1
// FIX20110421(ExpSS)
	SimpleShapeDB ssdb;
	WorldXMLReader r(fdb, db, ssdb);
#else
	WorldXMLReader r(fdb, db);
#endif
	bool b = r.read("NoWorld.xml");
	ASSERT(!b);
}


void WorldTest::test_BadWorld1()
{
	try {
		const char *dir = "./data";
		FilenameDB fdb;
		ASSERT(fdb.pushDirectory(dir));
		X3DDB db;
#if 1
// FIX20110421(ExpSS)
		SimpleShapeDB ssdb;
		WorldXMLReader r(fdb, db, ssdb);
#else
		WorldXMLReader r(fdb, db);
#endif
		bool b = r.read("BadWorld1.xml");
		ASSERT(!b);
	} catch(SimObjBase::Exception &e) {
		fprintf(stderr, "SimObjBase::Exception : %s\n", e.msg());
		ASSERT(true);
	}
}


#include "../../../commonlib/tests/runner.cpp"
