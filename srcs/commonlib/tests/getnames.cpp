/* $Id: getnames.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class GetObjectNamesTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(GetObjectNamesTest);
	CPPUNIT_TEST(test_reqAGENTS);
	CPPUNIT_TEST(test_reqENTITIES);
	CPPUNIT_TEST(test_res2);
	CPPUNIT_TEST(test_res3);
	CPPUNIT_TEST(test_res4);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void test_reqAGENTS();
	void test_reqENTITIES();
	void test_res1();
	void test_res2();
	void test_res3();
	void test_res4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GetObjectNamesTest);

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL
#define ASSERT_D_EQUAL CPPUNIT_ASSERT_DOUBLES_EQUAL

#include "decoder_tools.cpp"
#include "testdef.h"

void GetObjectNamesTest::test_reqAGENTS()
{
	typedef CommRequestGetAgentNamesEncoder E;
	E enc;


	CommDataDecoder::Result *r = decoderTest1(enc, COMM_REQUEST_GET_OBJECT_NAMES);
	ASSERT(r != NULL);
	RequestGetObjectNamesEvent *evt = (RequestGetObjectNamesEvent*)r->data();
	ASSERT(evt->getAgents());
	ASSERT(!evt->getEntities());
		
	delete r;
}

void GetObjectNamesTest::test_reqENTITIES()
{
	typedef CommRequestGetEntityNamesEncoder E;
	E enc;

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_REQUEST_GET_OBJECT_NAMES);
	ASSERT(r != NULL);
	RequestGetObjectNamesEvent *evt = (RequestGetObjectNamesEvent*)r->data();
	ASSERT(evt->getAgents());
	ASSERT(evt->getEntities());
		
	delete r;
}

#define ARY_SIZE(ARY) (int) (sizeof(ARY)/sizeof(ARY[0]))

void GetObjectNamesTest::test_res1()
{
	typedef CommResultGetObjectNamesEncoder E;
	E enc;
	char *names[] = {
		"ABC", "DEF", "GHI"
	};
	for (int i=0; i<ARY_SIZE(names); i++) {
		enc.push(names[i]);
	}

	ASSERT_EQUAL(1, enc.packetNum());

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_RESULT_GET_OBJECT_NAMES);
	ASSERT(r != NULL);
	ResultGetObjectNamesEvent *evt = (ResultGetObjectNamesEvent *)r->data();
	ASSERT(evt != NULL);
	std::vector<std::string> v;
	evt->copy(v);
	ASSERT_EQUAL(ARY_SIZE(names), (int)v.size());
	for (int i=0; i<v.size(); i++) {
		ASSERT(strcmp(names[i], v[i].c_str()) == 0);
	}
		
	delete r;
}

void GetObjectNamesTest::test_res2()
{
	typedef CommResultGetObjectNamesEncoder E;
	E enc;

	const int N = 51;
	const char *name = "FOOOOOOOOOOOO";
	for (int i=0; i<N; i++) {
		enc.push(name);
	}

	ASSERT_EQUAL(2, enc.packetNum());

	CommDataDecoder::Result *r = decoderTestN(enc, COMM_RESULT_GET_OBJECT_NAMES);
	ASSERT(r != NULL);
	ResultGetObjectNamesEvent *evt = (ResultGetObjectNamesEvent *)r->data();
	ASSERT(evt != NULL);
	std::vector<std::string> v;
	evt->copy(v);
	ASSERT_EQUAL(N, (int)v.size());
	for (int i=0; i<N; i++) {
		ASSERT(strcmp(name, v[i].c_str()) == 0);
	}
		
	delete r;
}

void GetObjectNamesTest::test_res3()
{
	typedef CommResultGetObjectNamesEncoder E;
	E enc;

	const int N = 500;
	const char *name = "FOOOOOOOOOOOO";
	for (int i=0; i<N; i++) {
		enc.push(name);
	}

	ASSERT_EQUAL(11, enc.packetNum());

	CommDataDecoder::Result *r = decoderTestN(enc, COMM_RESULT_GET_OBJECT_NAMES);
	ASSERT(r != NULL);
	ResultGetObjectNamesEvent *evt = (ResultGetObjectNamesEvent *)r->data();
	ASSERT(evt != NULL);
	std::vector<std::string> v;
	evt->copy(v);
	ASSERT_EQUAL(N, (int)v.size());
	for (int i=0; i<N; i++) {
		ASSERT(strcmp(name, v[i].c_str()) == 0);
	}
		
	delete r;
}

void GetObjectNamesTest::test_res4()
{
	char *names[] = {
		"ABC", "DEF", "GHI"
	};
	typedef std::vector<std::string> C;
	C c;
	for (int i=0; i<ARY_SIZE(names); i++) {
		c.push_back(names[i]);
	}

	typedef CommResultGetObjectNamesEncoder E;
	E enc;
	enc.push(c);

	ASSERT_EQUAL(1, enc.packetNum());

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_RESULT_GET_OBJECT_NAMES);
	ASSERT(r != NULL);
	ResultGetObjectNamesEvent *evt = (ResultGetObjectNamesEvent *)r->data();
	ASSERT(evt != NULL);
	std::vector<std::string> v;
	evt->copy(v);
	ASSERT_EQUAL(ARY_SIZE(names), (int)v.size());
	for (int i=0; i<v.size(); i++) {
		ASSERT(strcmp(names[i], v[i].c_str()) == 0);
	}
		
	delete r;
}


#include "runner.cpp"


