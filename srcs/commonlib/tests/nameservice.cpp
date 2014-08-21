/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class NameServiceTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(NameServiceTest);
	CPPUNIT_TEST(test_reqQuery);
	CPPUNIT_TEST(test_reqQueryL);
	CPPUNIT_TEST(test_reqPinger);
	CPPUNIT_TEST(test_reqPingerL);
	CPPUNIT_TEST(test_resPinger);
	CPPUNIT_TEST(test_resPingerL);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void test_reqQuery();
	void test_reqQueryL();

	void test_reqPinger();
	void test_reqPingerL();
	void test_resPinger();
	void test_resPingerL();
};

CPPUNIT_TEST_SUITE_REGISTRATION(NameServiceTest);

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL
#define ASSERT_D_EQUAL CPPUNIT_ASSERT_DOUBLES_EQUAL

#include "decoder_tools.cpp"
#include "testdef.h"

void NameServiceTest::test_reqQuery()
{
	Service::Kind kind = Service::CAPTURE_VIEW;
	CommRequestNSQueryEncoder enc(kind);

	CommDataDecoder::Result *r = decoderTest1(enc, COMM_NS_QUERY_REQUEST);
	ASSERT(r != NULL);
	delete r;
}

void NameServiceTest::test_reqQueryL()
{
	Service::Kind kind = Service::CAPTURE_VIEW;
	CommRequestNSQueryEncoder enc(kind);

	class LImpl : public TestListenerBase
	{
	private:
		Service::Kind m_kind;
	public:
		LImpl(Service::Kind kind) : m_kind(kind) {}
	private:
		void recvRequestNSQuery(Source &, RequestNSQueryEvent &evt) {
			m_invoked = true;
			CPPUNIT_ASSERT_EQUAL(m_kind, evt.kind());
		}
	};
	LImpl l(kind);
	CommDataDecoder::Result *r = decoderTest1L(enc, COMM_NS_QUERY_REQUEST, &l);
	ASSERT(r == NULL);
}

void NameServiceTest::test_reqPinger()
{
	Service::Kind kind = Service::CAPTURE_VIEW;
	CommRequestNSPingerEncoder enc(kind);
	CommDataDecoder::Result *r = decoderTest1(enc, COMM_NS_PINGER_REQUEST);
	ASSERT(r != NULL);
	RequestNSPingerEvent *evt = (RequestNSPingerEvent *)r->data();
	ASSERT_EQUAL(kind, evt->kind());
	delete r;
}

void NameServiceTest::test_reqPingerL()
{
	Service::Kind kind = Service::CAPTURE_VIEW;
	CommRequestNSPingerEncoder enc(kind);

	class LImpl : public TestListenerBase
	{
	private:
		Service::Kind m_kind;
	public:
		LImpl(Service::Kind k) : m_kind(k) {}
	private:
		void recvRequestNSPinger(Source &, RequestNSPingerEvent &evt) {
			m_invoked = true;
			CPPUNIT_ASSERT_EQUAL(m_kind, evt.kind());
		}
	};
	LImpl l(kind);
	CommDataDecoder::Result *r = decoderTest1L(enc, COMM_NS_PINGER_REQUEST, &l);
	ASSERT(r == NULL);
}

void NameServiceTest::test_resPinger()
{
	bool active = true;
	CommResultNSPingerEncoder enc(active);
	CommDataDecoder::Result *r = decoderTest1(enc, COMM_NS_PINGER_RESULT);
	ASSERT(r != NULL);
	ResultNSPingerEvent *evt = (ResultNSPingerEvent *)r->data();
	ASSERT_EQUAL(!active, !evt->active());

	delete r;
}


void NameServiceTest::test_resPingerL()
{
	bool active = true;
	CommResultNSPingerEncoder enc(active);

	class LImpl : public TestListenerBase
	{
	private:
		bool m_active;
	public:
		LImpl(bool active) : m_active(active) {}
	private:
		void recvResultNSPinger(Source &, ResultNSPingerEvent &evt) {
			m_invoked = true;
			ASSERT_EQUAL(!m_active, !evt.active());
		}
	};
	LImpl l(active);

	CommDataDecoder::Result *r = decoderTest1L(enc, COMM_NS_PINGER_RESULT, &l);
	ASSERT(r == NULL);
}

#include "runner.cpp"

