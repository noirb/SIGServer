#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#include <iostream>

class FilenameDBTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(FilenameDBTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	CPPUNIT_TEST(test3);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown() {}

	void test1();
	void test2();
	void test3();
};

#include "FilenameDB.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FilenameDBTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL
#define ASSERT_EQUAL_STRING(STR1, STR2) {ASSERT(STR1); ASSERT(STR2); ASSERT(strcmp(STR1, STR2) == 0); }

void FilenameDBTest::setUp()
{

}

void FilenameDBTest::test1()
{
	char buf[4096];
	FilenameDB db;
	ASSERT(db.pushDirectory("data"));
	ASSERT_EQUAL_STRING("data/Entity.xml", db.getPath("Entity.xml", buf));
	ASSERT_EQUAL_STRING("data/Agent.xml", db.getPath("Agent.xml", buf));
	ASSERT_EQUAL((char*)NULL, (char*)db.getPath("foo.xml", buf));
}

void FilenameDBTest::test2()
{
	char buf[4096];
	FilenameDB db;
	ASSERT(db.pushDirectory("data/"));
	ASSERT_EQUAL_STRING((const char*)"data/Entity.xml", db.getPath("Entity.xml", buf));
	ASSERT_EQUAL_STRING((const char*)"data/Agent.xml", db.getPath("Agent.xml", buf));
}

void FilenameDBTest::test3()
{
	char buf[4096];
	FilenameDB db;
	ASSERT(db.pushDirectory("data"));
	ASSERT(db.pushDirectory("../../conf"));
	ASSERT_EQUAL_STRING((const char*)"data/Entity.xml", db.getPath("Entity.xml", buf));
	ASSERT_EQUAL_STRING((const char*)"../../conf/plant_A.wrl", db.getPath("plant_A.wrl", buf));
}

#include "../../../commonlib/tests/runner.cpp"



