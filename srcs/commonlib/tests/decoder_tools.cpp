/*
 * Written by okamoto on 2011-03-25
 */

CommDataDecoder::Result * decoderTest1(CommDataEncoder &enc, CommDataType type)
{
	CommDataDecoder decoder;

	CPPUNIT_ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret != NULL);
	CPPUNIT_ASSERT_EQUAL(ret->type(), type);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	return ret;
}

typedef CommDataDecoder::Listener L;

class TestListenerBase : public L
{
protected:
	bool	m_invoked;
protected:
	TestListenerBase() : m_invoked(false), L() {}
public:
	bool 	invoked() { return m_invoked; }
};

CommDataDecoder::Result * decoderTest1L(CommDataEncoder &enc,
					CommDataType type,
					TestListenerBase *l
					)
{
	CommDataDecoder decoder;
	decoder.setListener(l);
	
	CPPUNIT_ASSERT_EQUAL(1, enc.packetNum());
	Source from(-1, "testhost");

	int n;
	char *data = enc.encode(0, n);
	CPPUNIT_ASSERT(data != NULL);

	int decoded;
	CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
	CPPUNIT_ASSERT(ret == NULL);
	CPPUNIT_ASSERT_EQUAL(n, decoded);
	CPPUNIT_ASSERT(l->invoked());
	return ret;
}

CommDataDecoder::Result * decoderTestN(CommDataEncoder &enc, CommDataType type)
{
	CommDataDecoder decoder;

	Source from(-1, "testhost");
	for (int i=0; i<enc.packetNum(); i++) {

		int n;
		char *data = enc.encode(i, n);
		CPPUNIT_ASSERT(data != NULL);

		int decoded;
		CommDataDecoder::Result *ret = decoder.push(from, data, n, decoded);
		CPPUNIT_ASSERT_EQUAL(n, decoded);
		if (i == enc.packetNum()-1) {
			CPPUNIT_ASSERT(ret != NULL);
			CPPUNIT_ASSERT_EQUAL(ret->type(), type);
			return ret;
		}
	}
	CPPUNIT_ASSERT(false);
	return NULL;
}
