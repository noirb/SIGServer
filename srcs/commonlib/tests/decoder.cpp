/* $Id: decoder.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class DecoderTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(DecoderTest);
	CPPUNIT_TEST(test_DetectEntities);
	CPPUNIT_TEST(test_DetectEntitiesL);

	CPPUNIT_TEST(test_SimCtrl);
	CPPUNIT_TEST(test_SimCtrlL);

	CPPUNIT_TEST(test_LogMsg1);
	CPPUNIT_TEST(test_MultiData);
	CPPUNIT_TEST(test_CtrlCmd);

	CPPUNIT_TEST(test_reqCaptureViewImage);
	CPPUNIT_TEST(test_resCaptureViewImage);

	CPPUNIT_TEST(test_setJointAngle);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}
	
	void test_DetectEntities();
	void test_DetectEntitiesL();

	void test_SimCtrl();
	void test_SimCtrlL();

	void test_LogMsg1();
	void test_MultiData();

	void test_CtrlCmd();

	void test_reqCaptureViewImage();
	void test_resCaptureViewImage();

	void test_setJointAngle();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DecoderTest);

#include "CommDataDecoder.h"
#include "CommDataEncoder.h"
#include "Source.h"

void DecoderTest::test_DetectEntities()
{
	const char *controllerName = "Agent1";
	CommRequestDetectEntitiesEncoder enc(controllerName);
	CommDataDecoder decoder;
	CPPUNIT_ASSERT_EQUAL(1, enc.packetNum());

	Source from(-1, "testhost");
	int n;
	char *data = enc.encode(0, n);
	
	CPPUNIT_ASSERT(data != NULL);
	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret != NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	delete ret;
}

class ListenerImpl : public CommDataDecoder::Listener
{
public:
	bool called;
	ListenerImpl() : called(false) {}

	void recvRequestDetectEntities(Source &from, RequestDetectEntitiesEvent &evt)
	{
		CPPUNIT_ASSERT(true);
		called = true;
	};
};

void DecoderTest::test_DetectEntitiesL()
{
	const char *controllerName = "Agent1";
	CommRequestDetectEntitiesEncoder enc(controllerName);
	CommDataDecoder decoder;
	ListenerImpl l;
	decoder.setListener(&l);
	CPPUNIT_ASSERT_EQUAL(1, enc.packetNum());

	Source from(-1, "testhost");
	int n;
	char *data = enc.encode(0, n);
	
	CPPUNIT_ASSERT(data != NULL);
	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret == NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	CPPUNIT_ASSERT(l.called);
}

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL
#define ASSERT_D_EQUAL CPPUNIT_ASSERT_DOUBLES_EQUAL

/*******************************************
 *
 * Invoke Controller Method
 *
 *******************************************/

class test_SimCtrl_Listener : public CommDataDecoder::Listener
{
public:
	bool called;
	test_SimCtrl_Listener() : called(false) {}

	void recvRequestSimCtrl(Source &from, RequestSimCtrlEvent &evt) {

		ASSERT(true);
		called = true;
	}
};

void DecoderTest::test_SimCtrl()
{
	CommRequestSimCtrlEncoder simctrl_enc(SIM_CTRL_COMMAND_START);
	CommData::Encoder &enc = simctrl_enc;


	CommDataDecoder decoder;

	ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret != NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	delete ret;
}

void DecoderTest::test_SimCtrlL()
{
	CommRequestSimCtrlEncoder simctrl_enc(SIM_CTRL_COMMAND_START);
	CommData::Encoder &enc = simctrl_enc;


	CommDataDecoder decoder;
	test_SimCtrl_Listener l;
	decoder.setListener(&l);

	ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret == NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	CPPUNIT_ASSERT(l.called);
}

#include "Logger.h"

void DecoderTest::test_LogMsg1()
{
	CommLogMsgEncoder enc(LOG_MSG, "send to you");

	CommDataDecoder decoder;

	ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret != NULL);
	CPPUNIT_ASSERT(ret->type() == COMM_LOG_MSG);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	delete ret;
}

void DecoderTest::test_MultiData()
{
	char buf[1024];
	char *p = buf;

	int n;
	for (int i=0; i<2; i++) {
		CommLogMsgEncoder enc(LOG_MSG, "send to you");
		ASSERT_EQUAL(1, enc.packetNum());
		char *data = enc.encode(0, n);
		memcpy(p, data, n);
		p += n;
	}

	Source from(-1, "testhost");
	CommDataDecoder decoder;

	int left = p - buf;
	p = buf;

	while (left > 0) {
		int decoded;
		CommDataDecoder::Result *ret = decoder.push(from, buf, left, decoded);
		CPPUNIT_ASSERT(ret != NULL);
		CPPUNIT_ASSERT(ret->type() == COMM_LOG_MSG);
		CPPUNIT_ASSERT_EQUAL(n, decoded);
		left -= decoded;
		p += decoded;
		
		delete ret;
	}
}

//#include "CommandImpl.h"

void DecoderTest::test_CtrlCmd()
{
	/*
	MoveCommand move("Agent1", Vector3d(0.0, 1.0, 2.0), 5.0);
	ControllerCommandEncoder enc(move);

	CommDataDecoder decoder;

	ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret != NULL);
	CPPUNIT_ASSERT(ret->type() == COMM_CONTROLLER_COMMAND);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	delete ret;
	*/
}

#include "ViewImageInfo.h"
#include "ViewImage.h"

#include "decoder_tools.cpp"

void DecoderTest::test_reqCaptureViewImage()
{
	ViewImageInfo info(IMAGE_DATA_TYPE_ANY, COLORBIT_24, IMAGE_320X240);
	CommRequestCaptureViewImageEncoder enc("Agent1", info);
	decoderTest1(enc, COMM_REQUEST_CAPTURE_VIEW_IMAGE);
}

void DecoderTest::test_resCaptureViewImage()
{
	ViewImage *img = ViewImage::createSample();
	CommResultCaptureViewImageEncoder enc(*img);
	decoderTestN(enc, COMM_RESULT_CAPTURE_VIEW_IMAGE);
	
	delete img;
}

const double EPS = 0.0000001;

void DecoderTest::test_setJointAngle()
{
	const char *agentName = "Robot";
	const char *jname = "JointName";
	double angle = 2.5;
	CommRequestSetJointAngleEncoder enc(agentName, jname, angle);

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_REQUEST_SET_JOINT_ANGLE);

	ASSERT(r != NULL);
	RequestSetJointAngleEvent *evt = (RequestSetJointAngleEvent *)r->data();
	ASSERT(strcmp(evt->getAgentName(), agentName) == 0);
	ASSERT(strcmp(evt->getJointName(), jname) == 0);
	ASSERT_D_EQUAL(evt->getAngle(), angle, EPS);
	delete r;
}

#include "runner.cpp"
