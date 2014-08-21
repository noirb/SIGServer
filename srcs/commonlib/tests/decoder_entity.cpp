/*
 * Written by okamoto on 2011-03-25
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

class DecoderEntityTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(DecoderEntityTest);

	CPPUNIT_TEST(test_reqGetEntity);
	CPPUNIT_TEST(test_resGetEntity);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}
	void test_reqGetEntity();
	void test_resGetEntity();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DecoderEntityTest);

#include "CommDataDecoder.h"
#include "CommDataEncoder.h"
#include "Source.h"
#include "SimObj.h"
#include "Attribute.h"
#include "CParts.h"
#include "PartsCmpnt.h"


#include "decoder_tools.cpp"

#define ASSERT CPPUNIT_ASSERT
#define ASSERT_EQUAL CPPUNIT_ASSERT_EQUAL

#define ASSERT_D_EQUAL CPPUNIT_ASSERT_DOUBLES_EQUAL

void DecoderEntityTest::test_reqGetEntity()
{
	CommRequestGetEntityEncoder enc("Agent1");
	CommDataDecoder::Result *r = decoderTest1(enc, COMM_REQUEST_GET_ENTITY);
	ASSERT(r != NULL);
	delete r;
}

void DecoderEntityTest::test_resGetEntity()
{
	const double EPS = 0.00001;

	try {
		CommDataDecoder::Result *res = NULL;
		Size pos(4.0, 5.0, 6.0);
		Position body_pos(0, 1, 2);
		Size body_sz(3, 4, 5);
		{
			SimObj obj;
			obj.push(new Attribute("name", new StringValue("Agent1")));
			obj.push(new Attribute("class", new StringValue("Agent")));
			obj.push(new Attribute("dynamics", new BoolValue(false)));
			obj.push(new Attribute("x", new DoubleValue(1.0)));
			obj.push(new Attribute("y", new DoubleValue(2.0)));
			obj.push(new Attribute("z", new DoubleValue(3.0)));
			obj.push(new BoxParts("body", body_pos, body_sz));

			obj.setPosition(pos.x(), pos.y(), pos.z());
			ASSERT(obj.ops() & SimObjBase::OP_SET_POSITION);
			 
			CommResultGetEntityEncoder enc(1.0, &obj);
			res = decoderTest1(enc, COMM_RESULT_GET_ENTITY);
		}

		ASSERT(res->type() == COMM_RESULT_GET_ENTITY);
		ResultGetEntityEvent *evt = (ResultGetEntityEvent*)res->data();
	
		SimObj *newobj = evt->release();
		ASSERT(newobj != NULL);
		ASSERT(strcmp(newobj->name(), "Agent1") == 0);
		ASSERT(strcmp(newobj->classname(), "Agent") == 0);

		ASSERT(newobj->ops() & SimObjBase::OP_SET_POSITION);
		ASSERT_D_EQUAL(newobj->x(), pos.x(), EPS);
		ASSERT_D_EQUAL(newobj->y(), pos.y(), EPS);
		ASSERT_D_EQUAL(newobj->z(), pos.z(), EPS);


		Parts * body = newobj->getParts("body");
		ASSERT(body != NULL);
		ASSERT(body->getType() == PARTS_TYPE_BOX);
		BoxPartsCmpnt *data = (BoxPartsCmpnt*)body->extdata();
		ASSERT(data != NULL);
		Size &sz_ = data->size();

		ASSERT_D_EQUAL(body_sz.x(), sz_.x(), EPS);
		ASSERT_D_EQUAL(body_sz.y(), sz_.y(), EPS);
		ASSERT_D_EQUAL(body_sz.z(), sz_.z(), EPS);
		ASSERT(data->size() == body_sz);


		delete newobj;
		delete res;
	} catch(SimObjBase::Exception &e) {
		printf("Exception : %s\n", e.msg());
		ASSERT(0);
	}
}

#include "runner.cpp"

