
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class WorldTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(WorldTest);
	CPPUNIT_TEST(test);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void test();
};

CPPUNIT_TEST_SUITE_REGISTRATION(WorldTest);


#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include "WorldXMLReader.h"
#include "SSimWorld.h"
#include "X3DDB.h"
#include "FilenameDB.h"
#include "Logger.h"


void WorldTest::setUp()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
}

void WorldTest::test()
{
	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);
	const char *dir = "/home/yoshi/proj/irwas/work/fromnii-091127/source";

	LOG_STDOUT();
	LOG_OUTPUT_LEVEL(LOG_ALL);

	FilenameDB fdb;
	ASSERT(fdb.pushDirectory(dir));
	X3DDB db;
	WorldXMLReader r(fdb, db);
	bool b = r.read("MyWorld.xml");
	ASSERT(b);

}



#include "../../../commonlib/tests/runner.cpp"
