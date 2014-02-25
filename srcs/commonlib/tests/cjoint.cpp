/* $Id: cjoint.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class ConnectJointTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ConnectJointTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	CPPUNIT_TEST_SUITE_END();
public:
	void test1();
	void test2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectJointTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"
#include "decoder_tools.cpp"

typedef CommDataDecoder::Result Result;

void ConnectJointTest::test1()
{
	const char *jointName = "Joint";
	const char *agentName = "Agent";
	const char *agentParts = "AgentParts";
	const char *targetName = "Target";
	const char *targetParts= "TargetParts";
	CommRequestConnectJointEncoder enc(jointName,
					   agentName, agentParts,
					   targetName, targetParts);
	
	CommDataResult *r = decoderTest1(enc, COMM_REQUEST_CONNECT_JOINT);
	ASSERT(r != NULL);
	RequestConnectJointEvent *evt = (RequestConnectJointEvent *)r->data();
	ASSERT(strcmp(evt->getJointName(), jointName) == 0);

	ASSERT(strcmp(evt->getAgentName(), agentName) == 0);
	ASSERT(strcmp(evt->getAgentPartsName(), agentParts) == 0);

	ASSERT(strcmp(evt->getTargetName(), targetName) == 0);
	ASSERT(strcmp(evt->getTargetPartsName(), targetParts) == 0);
}

void ConnectJointTest::test2()
{
	const char *jointName = "Joint";
	const char *agentName = "Agent";
	const char *agentParts = NULL;
	const char *targetName = "Target";
	const char *targetParts= NULL;
	CommRequestConnectJointEncoder enc(jointName,
					   agentName, agentParts,
					   targetName, targetParts);
	
	CommDataResult *r = decoderTest1(enc, COMM_REQUEST_CONNECT_JOINT);
	ASSERT(r != NULL);
	RequestConnectJointEvent *evt = (RequestConnectJointEvent *)r->data();
	ASSERT(strcmp(evt->getJointName(), jointName) == 0);

	ASSERT(strcmp(evt->getAgentName(), agentName) == 0);
	ASSERT(evt->getAgentPartsName() == NULL);

	ASSERT(strcmp(evt->getTargetName(), targetName) == 0);
	ASSERT(evt->getTargetPartsName() == NULL);
}

#include "runner.cpp"

