/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class AttachTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(AttachTest);
	CPPUNIT_TEST(test_reqAttachView1);
	CPPUNIT_TEST(test_reqAttachView2);
	CPPUNIT_TEST(test_resAttachView1);
	CPPUNIT_TEST(test_resAttachView2);
	CPPUNIT_TEST(test_reqAttachCtrl);
	CPPUNIT_TEST(test_resAttachCtrl);
	CPPUNIT_TEST(test_reqProvideService1);
	CPPUNIT_TEST(test_reqProvideService2);
	CPPUNIT_TEST_SUITE_END();

public:
	void test_reqAttachView1();
	void test_reqAttachView2();
	void test_resAttachView1();
	void test_resAttachView2();
	void test_reqAttachCtrl();
	void test_resAttachCtrl();
	void test_reqProvideService1();
	void test_reqProvideService2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttachTest);

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#include "CommDataDecoder.h"
#include "CommDataEncoder.h"
#include "Source.h"

void AttachTest::test_reqAttachView1()
{
	const char *NAME = "Agent2";
	CommRequestAttachViewEncoder enc(NAME);
	ASSERT_EQUAL(1, enc.packetNum());

	CommDataDecoder decoder;
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	ASSERT(data != NULL);
	
	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	ASSERT(ret != NULL);
	ASSERT(ret->type() == COMM_REQUEST_ATTACH_VIEW);
	ASSERT_EQUAL(n, decoded);

	RequestAttachViewEvent *evt = (RequestAttachViewEvent*)ret->data();
	ASSERT(strcmp(NAME, evt->getViewName()) == 0);
	ASSERT(!evt->isPolling());
	delete ret;
}

void AttachTest::test_reqAttachView2()
{
	const char *NAME = "Agent2";
	bool POLLING = true;
	CommRequestAttachViewEncoder enc(NAME, POLLING);
	ASSERT_EQUAL(1, enc.packetNum());

	CommDataDecoder decoder;
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	ASSERT(data != NULL);
	
	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	ASSERT(ret != NULL);
	ASSERT(ret->type() == COMM_REQUEST_ATTACH_VIEW);
	ASSERT_EQUAL(n, decoded);

	RequestAttachViewEvent *evt = (RequestAttachViewEvent*)ret->data();
	ASSERT(strcmp(NAME, evt->getViewName()) == 0);
	ASSERT_EQUAL(POLLING, evt->isPolling());
	delete ret;
}

#include "decoder_tools.cpp"

#include "X3DDB.h"

void AttachTest::test_resAttachView1()
{
	X3DDB db;
	CommResultAttachViewEncoder enc(COMM_RESULT_OK, NULL, db);

	CommDataDecoder::Result *ret = decoderTest1(enc, COMM_RESULT_ATTACH_VIEW);
	ASSERT(ret != NULL);
	ResultAttachViewEvent *evt = (ResultAttachViewEvent *)ret->data();
	ASSERT(evt->getX3DDB() == NULL);

	delete ret;
}

static void compDB(X3DDB &ans, X3DDB &db)
{
	ASSERT_EQUAL(ans.size(), db.size());

	X3DDB::Iterator *itr = ans.getIterator();
	ASSERT(itr != NULL);
	while (true) {
		const char *k = itr->nextKey();
		if (k == NULL) { break; }
		
		const char *v = ans.get(k);
		const char *v_ = db.get(k);
		ASSERT(v_ != NULL);
		ASSERT(strcmp(v_, v) == 0);
	}
	delete itr;
}

void AttachTest::test_resAttachView2()
{
	X3DDB db;

	db.set("a", "A");
	db.set("bbbbbbbbbbbbbbbb", "BBBBBBBBBBBBBBBBB");
	db.set("ccccccccccccccccccccccc", "CCCCCCCCCCCCCCCCCCCCC");
	db.set("a:b:c:d", "file.x3d");

	CommResultAttachViewEncoder enc(COMM_RESULT_OK, NULL, db);

	CommDataDecoder::Result *ret = decoderTest1(enc, COMM_RESULT_ATTACH_VIEW);
	ASSERT(ret != NULL);
	ResultAttachViewEvent *evt = (ResultAttachViewEvent *)ret->data();

	X3DDB *retDB = evt->getX3DDB();
	ASSERT(retDB != NULL);
	compDB(db, *retDB);

	delete ret;
}


void AttachTest::test_reqAttachCtrl()
{
	CommRequestAttachControllerEncoder enc("Agent1");
	decoderTest1(enc, COMM_REQUEST_ATTACH_CONTROLLER);
}

void AttachTest::test_resAttachCtrl()
{
	CommResultAttachControllerEncoder enc(COMM_RESULT_OK, NULL);
	decoderTest1(enc, COMM_RESULT_ATTACH_CONTROLLER);
}

#include "Service.h"

void AttachTest::test_reqProvideService1()
{
	Service::Kind kind = Service::CAPTURE_VIEW | Service::DETECT_ENTITIES;
	Service service("test_service_provider", kind, "localhost", 6789);
	CommRequestProvideServiceEncoder enc(service);
	CommDataDecoder::Result *r = decoderTest1(enc, COMM_REQUEST_PROVIDE_SERVICE);
	ASSERT(r != NULL);
	RequestProvideServiceEvent *evt = (RequestProvideServiceEvent *)r->data();
	Service &s = evt->getService();
	ASSERT(strcmp(service.name(), s.name()) == 0);
	ASSERT_EQUAL(service.kind(), s.kind());
	ASSERT(strcmp(service.hostname(), s.hostname()) == 0);
	ASSERT_EQUAL(service.port(), s.port());
}

void AttachTest::test_reqProvideService2()
{
	const char *providerName = "test_service_provider";
	Service::Kind kind = Service::CAPTURE_VIEW | Service::DETECT_ENTITIES;
	Service service("test_service_provider", kind, 6789);
	CommRequestProvideServiceEncoder enc(service);
	CommDataDecoder::Result *r = decoderTest1(enc, COMM_REQUEST_PROVIDE_SERVICE);
	ASSERT(r != NULL);
	RequestProvideServiceEvent *evt = (RequestProvideServiceEvent *)r->data();
	Service &s = evt->getService();
	ASSERT(strcmp(service.name(), s.name()) == 0);
	ASSERT_EQUAL(service.kind(), s.kind());
	ASSERT(s.hostname() == NULL );
	ASSERT_EQUAL(service.port(), s.port());
}


#include "runner.cpp"

