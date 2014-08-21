/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class ReleaseJointTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ReleaseJointTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
public:
	void test1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ReleaseJointTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"
#include "decoder_tools.cpp"

typedef CommDataDecoder::Result Result;

void ReleaseJointTest::test1()
{
	const char *agentName = "Agent";
	const char *jointName = "Joint";
	CommRequestReleaseJointEncoder enc(agentName, jointName);
	CommDataResult *r = decoderTest1(enc, COMM_REQUEST_RELEASE_JOINT);
	ASSERT(r != NULL);
	RequestReleaseJointEvent *evt = (RequestReleaseJointEvent *)r->data();
	ASSERT(strcmp(evt->getAgentName(), agentName) == 0);
	ASSERT(strcmp(evt->getJointName(), jointName) == 0);
}

#include "runner.cpp"

