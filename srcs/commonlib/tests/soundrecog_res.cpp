/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class SoundRecogTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SoundRecogTest);
	CPPUNIT_TEST(testRes1);
	CPPUNIT_TEST(testRes2);
	CPPUNIT_TEST(testRes3);
	CPPUNIT_TEST_SUITE_END();
public:
	void testRes1();
	void testRes2();
	void testRes3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SoundRecogTest);

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"

#include "testdef.h"
#include "decoder_tools.cpp"


void SoundRecogTest::testRes1()
{
	typedef CommDataDecoder::Result Result;
	Encode encode = ENCODE_UTF8;
	const char *text = "‚±‚ñ‚É‚¿‚Í";
	CommResultSoundRecogEncoder enc(encode, text);
	Result *res = decoderTest1(enc, COMM_RESULT_SOUND_RECOG);
	ResultSoundRecogEvent *evt = (ResultSoundRecogEvent *)res->data();
	ASSERT(evt != NULL);
	Text *text_ = evt->getText();
	ASSERT(text_ != NULL);
	ASSERT(text_->getEncode() == encode);
	ASSERT(strcmp(text_->getString(), text) == 0);

	delete res;
}

void SoundRecogTest::testRes2()
{
	typedef CommDataDecoder::Result Result;
	Encode encode = ENCODE_SHIFT_JIS;
	const char *text = NULL;
	CommResultSoundRecogEncoder enc(encode, text);
	Result *res = decoderTest1(enc, COMM_RESULT_SOUND_RECOG);
	ResultSoundRecogEvent *evt = (ResultSoundRecogEvent *)res->data();
	ASSERT(evt != NULL);
	Text *text_ = evt->getText();
	ASSERT(text_ != NULL);
	ASSERT(text_->getEncode() == encode);
	const char *t = text_->getString();
	ASSERT(t == NULL || strlen(t) <= 0);

	delete res;
}

void SoundRecogTest::testRes3()
{
	typedef CommDataDecoder::Result Result;
	Encode encode = ENCODE_SHIFT_JIS;
	const char *text = "";
	CommResultSoundRecogEncoder enc(encode, text);
	Result *res = decoderTest1(enc, COMM_RESULT_SOUND_RECOG);
	ResultSoundRecogEvent *evt = (ResultSoundRecogEvent *)res->data();
	ASSERT(evt != NULL);
	Text *text_ = evt->getText();
	ASSERT(text_ != NULL);
	ASSERT(text_->getEncode() == encode);
	const char *t = text_->getString();
	ASSERT(t == NULL || strlen(t) <= 0);

	delete res;
}

#include "runner.cpp"


