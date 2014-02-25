/*
 * Modified by Okamoto on 2011-03-25
 */

#ifndef ParserDump_h
#define ParserDump_h

#ifdef ENABLE_TRANSFORM_TEST
#define TRANSFORM_TEST(T) \
{ \
	Vector3d v(1.0, 0.0, 0.0); \
	printf("v(%f, %f, %f) -> ", v.x(), v.y(), v.z());\
	(T).apply(v); \
	printf("(%f, %f, %f)\n", v.x(), v.y(), v.z());\
}
#else
#define TRANSFORM_TEST(T) 
#endif

#ifdef ENABLE_DUMP1
#define DUMP1(MSG) LOG_SYSTEM(MSG)
#else
#define DUMP1(MSG) 
#endif

#ifdef ENABLE_DUMP
#define DUMP(MSG) LOG_SYSTEM(MSG)
#else
#define DUMP(MSG) 
#endif


#endif //
 
