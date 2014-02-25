/* $Id: jointforce.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class GetJointForceTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(GetJointForceTest);
	CPPUNIT_TEST(testReq);
	CPPUNIT_TEST(testRes1);
	CPPUNIT_TEST(testRes2);
	CPPUNIT_TEST_SUITE_END();
public:
	void testReq();
	void testRes1();
	void testRes2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GetJointForceTest);

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"
#include "testdef.h"
#include "decoder_tools.cpp"

typedef CommDataDecoder::Result Result;

void GetJointForceTest::testReq()
{
	const char *agentName = "Agent1";
	const char *jointName = "JOINT_NAME";
	CommRequestGetJointForceEncoder enc(agentName, jointName);

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_REQUEST_GET_JOINT_FORCE);
	ASSERT(r != NULL);
	RequestGetJointForceEvent *evt = (RequestGetJointForceEvent *) r->data();
	ASSERT(evt != NULL);
	ASSERT(strcmp(evt->getAgentName(), agentName) == 0);
	ASSERT(strcmp(evt->getJointName(), jointName) == 0);
}

void GetJointForceTest::testRes1()
{

	CommResultGetJointForceEncoder enc;
	enc.success(false);

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_RESULT_GET_JOINT_FORCE);
	ASSERT(r != NULL);

	ResultGetJointForceEvent *evt = (ResultGetJointForceEvent *) r->data();
	ASSERT(evt != NULL);
	ASSERT_EQUAL(false, evt->success());
}

void GetJointForceTest::testRes2()
{

	CommResultGetJointForceEncoder enc;
	char *partsNames[] = {
		"parts1", "parts2",
	};
	enc.success(true);
	JointForce *jfs = enc.getJointForces();
	jfs[0].set(partsNames[0], Vector3d(0.0, 1.0, 2.0), Vector3d(3.0, 4.0, 5.0));
	jfs[1].set(partsNames[1], Vector3d(10.0, 11.0, 12.0), Vector3d(13.0, 14.0, 15.0));

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_RESULT_GET_JOINT_FORCE);
	ASSERT(r != NULL);

	ResultGetJointForceEvent *evt = (ResultGetJointForceEvent *) r->data();
	ASSERT(evt != NULL);
	ASSERT_EQUAL(true, evt->success());

	const double EPS = 0.0000001;

	ASSERT_EQUAL(2, evt->getJointForceNum());

	for (int idx=0; idx < evt->getJointForceNum(); idx++) {
		JointForce *jf = evt->getJointForce(idx);
		ASSERT(jf != NULL);
		ASSERT(strcmp(jf->getPartsName(), partsNames[idx]) == 0);

		const Vector3d &f = jf->getForce();
		ASSERT_D_EQUAL(idx*10.0, f.x(), EPS);
		ASSERT_D_EQUAL(idx*10.0 + 1.0, f.y(), EPS);
		ASSERT_D_EQUAL(idx*10.0 + 2.0, f.z(), EPS);

		const Vector3d &tq = jf->getTorque();
		ASSERT_D_EQUAL(idx*10.0 + 3.0, tq.x(), EPS);
		ASSERT_D_EQUAL(idx*10.0 + 4.0, tq.y(), EPS);
		ASSERT_D_EQUAL(idx*10.0 + 5.0, tq.z(), EPS);
	}
							

}


#include "runner.cpp"

