/* $Id: ctrl.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#include "testdef.h"

class ControllerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ControllerTest);
	CPPUNIT_TEST(test_Ctrl_OnInit);
	CPPUNIT_TEST(test_Ctrl_OnAction);
	CPPUNIT_TEST(test_Ctrl_OnRecvText);
	CPPUNIT_TEST(test_Ctrl_OnRecvSound);
	CPPUNIT_TEST(test_Ctrl_OnRecvMessage1);
	CPPUNIT_TEST(test_Ctrl_OnRecvMessage2);
	CPPUNIT_TEST(test_Ctrl_OnCollision);
	CPPUNIT_TEST_SUITE_END();

public:
	void test_Ctrl_OnInit();
	void test_Ctrl_OnAction();
	void test_Ctrl_OnRecvText();
	void test_Ctrl_OnRecvSound();
	void test_Ctrl_OnRecvMessage1();
	void test_Ctrl_OnRecvMessage2();
	void test_Ctrl_OnCollision();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ControllerTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include "CommDataDecoder.h"
#include "CommDataEncoder.h"
#include "Source.h"

#include "InvokeMethod.h"
#include "ControllerInf.h"
#include "ControllerEvent.h"

class test_OnInit_Controller : public ControllerInf
{
public:
	bool called;
	test_OnInit_Controller() : called(false) {}

	void onInit(InitEvent &) {
		called = true;
	}
};

void ControllerTest::test_Ctrl_OnInit()
{
	CommInvokeMethodOnInitEncoder enc;

	CommDataDecoder decoder;
	test_OnInit_Controller ctrl;
	decoder.setController(&ctrl);

	ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");
	int n;
	char *data = enc.encode(0, n);

	CPPUNIT_ASSERT(data != NULL);
	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret == NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	CPPUNIT_ASSERT(ctrl.called);
}

class test_OnAction_Controller : public ControllerInf
{
public:
	bool called;
	test_OnAction_Controller() : called(false) {}

	double onAction(ActionEvent &) {
		called = true;
		return 0.0;
	}
};

void ControllerTest::test_Ctrl_OnAction()
{
	CommInvokeMethodOnActionEncoder enc(1.0);

	CommDataDecoder decoder;
	test_OnAction_Controller ctrl;
	decoder.setController(&ctrl);

	ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");
	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret == NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	CPPUNIT_ASSERT(ctrl.called);
}

class test_OnRecvText_Controller : public ControllerInf
{
public:
	bool called;
	test_OnRecvText_Controller() : called(false) {}

	void onRecvText(RecvTextEvent &evt) {
		ASSERT(true);
		called = true;
	}
};

void ControllerTest::test_Ctrl_OnRecvText()
{
	CommInvokeMethodOnRecvTextEncoder enc(1.0, "from", "to", "hellow", ENCODE_ASCII, 1.0);
	CommDataDecoder decoder;
	test_OnRecvText_Controller ctrl;
	decoder.setController(&ctrl);

	ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");
	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret == NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	CPPUNIT_ASSERT(ctrl.called);
}

#include "RawSound.h"
#include "InvokeOnRecvSound.h"

class test_OnRecvSound_Controller : public ControllerInf
{
public:
	bool called;
	test_OnRecvSound_Controller() : called(false) {}

	void onRecvSound(RecvSoundEvent &evt) {
		ASSERT(true);
		called = true;
	}
};

void ControllerTest::test_Ctrl_OnRecvSound()
{
	RawSoundHeader h(1, 16000, 8, RAW_SOUND_LITTLE_ENDIAN);
	RawSound sound (h, 4);
	{
		int n = sound.getDataLen();
		char *ptr = sound.getData();
		memset(ptr, 0x01, n);
	}
	CommInvokeMethodOnRecvSoundEncoder enc(1.0, "from", "to", sound);

	CommDataDecoder decoder;
	test_OnRecvSound_Controller ctrl;
	decoder.setController(&ctrl);

	Source from(-1, "testhost");
	for (int i=0; i<enc.packetNum(); i++) {
		int n;
		char *data = enc.encode(i, n);
		CPPUNIT_ASSERT(data != NULL);
		int decoded;
		CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
		ASSERT_EQUAL(n, decoded);
		ASSERT(ret == NULL);
	}
	ASSERT(ctrl.called);
}

class test_OnRecvMessage_Controller : public ControllerInf
{
private:
	std::string m_sender;
public:
	bool called;
	test_OnRecvMessage_Controller(const char *sender) : called(false) {
		if (sender) { m_sender = sender; }
	}

	void onRecvMessage(RecvMessageEvent &evt) {
		ASSERT(true);
		called = true;
		ASSERT(strcmp(evt.getSender(), m_sender.c_str()) == 0);
	}
};


#define ARY_SIZE(ARY) ( (int)(sizeof(ARY)/sizeof(ARY[0])) )

void ControllerTest::test_Ctrl_OnRecvMessage1()
{
	char * strs[] = { "foo", "bar"};
	char *from = "sender";
	char *to = "receiver";

	int nstrs = ARY_SIZE(strs);
	CommInvokeMethodOnRecvMessageEncoder enc(from, to, nstrs, strs);

	CommDataDecoder decoder;
	test_OnRecvMessage_Controller ctrl(from);
	decoder.setController(&ctrl);

	Source src(-1, "testhost");
	ASSERT_EQUAL(1, enc.packetNum());

	int n;
	char *data = enc.encode(0, n);
	ASSERT(data != NULL);
	int decoded = decoder.push(src, data, n);
	ASSERT_EQUAL(decoded, n);
	ASSERT(ctrl.called);
}

void ControllerTest::test_Ctrl_OnRecvMessage2()
{
	char * strs[] = { "foo", "bar"};
	char *from = NULL;
	char *to = "receiver";

	int nstrs = ARY_SIZE(strs);
	CommInvokeMethodOnRecvMessageEncoder enc(from, to, nstrs, strs);

	CommDataDecoder decoder;
	test_OnRecvMessage_Controller ctrl(from);
	decoder.setController(&ctrl);

	Source src(-1, "testhost");
	ASSERT_EQUAL(1, enc.packetNum());

	int n;
	char *data = enc.encode(0, n);
	ASSERT(data != NULL);
	int decoded = decoder.push(src, data, n);
	ASSERT_EQUAL(decoded, n);
	ASSERT(ctrl.called);
}

class test_OnCollision_Controller : public ControllerInf
{
public:
	typedef std::vector<std::string> C;
public:
	bool called;
private:
	double m_time;
	const C & m_with;
public:
	test_OnCollision_Controller(double t, const C &with)
		: called(false), m_time(t), m_with(with) {}

	void onCollision(CollisionEvent &evt) {
		ASSERT(true);
		called = true;
		const CollisionEvent::WithC & result = evt.getWith();
		ASSERT_EQUAL(result.size(), m_with.size());
		int n = result.size();
		for (int i=0; i<n; i++) {
			std::string r = result[i];
			std::string g = m_with[i];
			printf("%s -> %s", r.c_str(), g.c_str());
			ASSERT(strcmp(r.c_str(), g.c_str()) == 0);
		}
		ASSERT_D_EQUAL(evt.time(), m_time, 0.000001);
	}
};

#define ARRAY_SIZE(ARY) ( (int) (sizeof(ARY)/sizeof(ARY[0])) )

void ControllerTest::test_Ctrl_OnCollision()
{
	double time = 5.3;
	char *names[] = { "target1", "target2", "target3" };
	typedef std::vector<std::string> C;
	C with;
	for (int i=0; i<ARRAY_SIZE(names); i++) {
		with.push_back(names[i]);
	}
		
	CommInvokeMethodOnCollisionEncoder enc(time, with);

	CommDataDecoder decoder;

	test_OnCollision_Controller ctrl(time, with);
	decoder.setController(&ctrl);

	Source src(-1, "testhost");
	ASSERT_EQUAL(1, enc.packetNum());

	int n;
	char *data = enc.encode(0, n);
	ASSERT(data != NULL);
	int decoded = decoder.push(src, data, n);
	ASSERT_EQUAL(decoded, n);
	ASSERT(ctrl.called);
}

#include "runner.cpp"


	
