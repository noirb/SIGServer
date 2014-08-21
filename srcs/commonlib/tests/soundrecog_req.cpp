/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class SoundRecogReqTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SoundRecogReqTest);
	CPPUNIT_TEST(testReq1);
	CPPUNIT_TEST_SUITE_END();
public:
	void testReq1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SoundRecogReqTest);

#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "Source.h"
#include "RawSound.h"

#include "testdef.h"
#include "decoder_tools.cpp"


void SoundRecogReqTest::testReq1()
{
	typedef CommDataDecoder::Result Result;
	int ch = 2;
	unsigned samplingRate = 16000;
	unsigned bitsPerSample = 8;
	int datalen = 100000;
	RawSoundEndian endian = RAW_SOUND_LITTLE_ENDIAN;
	RawSoundHeader h(ch, samplingRate, bitsPerSample, endian);
	RawSound sound(h, datalen);
	{
		char *data = sound.getData();
		memset(data, 0x00, datalen);
		*(data + datalen/2) = 0xfe;
	}

	CommRequestSoundRecogEncoder enc(sound);
	Result *res = decoderTestN(enc, COMM_REQUEST_SOUND_RECOG);
	RequestSoundRecogEvent *evt = (RequestSoundRecogEvent *)res->data();
	ASSERT(evt != NULL);
	RawSound *sound_ = evt->getRawSound();
	ASSERT(sound_ != NULL);
	RawSoundHeader &h_ = sound_->getHeader();
	ASSERT_EQUAL(ch,            h_.getChannelNum());
	ASSERT_EQUAL(samplingRate,  h_.getSamplingRate());
	ASSERT_EQUAL(bitsPerSample, h_.getBitPerSample());
	ASSERT(h_.getEndian() == endian);
	ASSERT_EQUAL(datalen,       sound_->getDataLen());

	char *data = sound_->getData();
	ASSERT_EQUAL((char)0xfe, *(data + datalen/2));

	delete res;
}

#include "runner.cpp"


