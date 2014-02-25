/* $Id: attrs.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class AttrsTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(AttrsTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	CPPUNIT_TEST_SUITE_END();
public:
	void test1();
	void test2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttrsTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"
#include "decoder_tools.cpp"

typedef CommDataDecoder::Result Result;

void AttrsTest::test1()
{
	CommRequestGetAttributesEncoder enc;
	enc.push("fx");
	enc.push("x");
	Result *res = decoderTest1(enc, COMM_REQUEST_GET_ATTRIBUTES);
	RequestGetAttributesEvent *evt = (RequestGetAttributesEvent *)res->data();
	int n = evt->getAttrNum();
	ASSERT_EQUAL(2, n);
	{
		const char *str = evt->getAttrName(0);
		ASSERT(strcmp(str, "fx") == 0);
	}

	{
		const char *str = evt->getAttrName(1);
		ASSERT(strcmp(str, "x") == 0);
	}
}


void AttrsTest::test2()
{
	CommRequestGetAttributesEncoder enc;
	const int N = 100;
	for (int i=0; i<N; i++) {
		char buf[100];
		sprintf(buf, "attr%d", i);
		enc.push(buf);
	}
	Result *res = decoderTest1(enc, COMM_REQUEST_GET_ATTRIBUTES);
	RequestGetAttributesEvent *evt = (RequestGetAttributesEvent *)res->data();
	int n = evt->getAttrNum();
	ASSERT_EQUAL(N, n);

	for (int i=0; i<n; i++) {
		const char *str = evt->getAttrName(i);
		char buf[100];
		sprintf(buf, "attr%d", i);
		ASSERT(strcmp(str, buf) == 0);
	}
}


#include "runner.cpp"

