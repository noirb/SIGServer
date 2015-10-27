/* $Id: jjudge.c,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */

#include <string.h>
#include <stdio.h>
#include <sigverse/simserver/jjudge.h>

void jjudge_init(jjudge_t *judge) { judge->ascii = 1; }

#if defined(WIN32)
#define JJUDGE_IN_SJIS1_RANGE(c) ((0x80 <= (c) && (c) <= 0x9f) || (0xe0 <= (c) && (c) <= 0xef) || (0xf0 <= (c) && (c) <= 0xfc))
#define JJUDGE_IN_SJIS2_RANGE(c) ((0x40 <= (c) && (c) <= 0x7e) || (0x80 <= (c) && (c) <= 0xfc))
#endif

int jjudge_equal(jjudge_t *judge, unsigned char jc, unsigned char c)
{
#if defined(WIN32)
	if (judge->ascii && JJUDGE_IN_SJIS1_RANGE(jc)) {
		judge->ascii = 0; return 0;
	} else if (!judge->ascii && JJUDGE_IN_SJIS2_RANGE(jc)) {
		judge->ascii = 1; return 0;
	} else if (judge->ascii && jc == c) {
		return 1;
	}
#else
	if (jc == c) { return 1; }
#endif
	return 0;
}

int jjudge_equal_preserve(jjudge_t *judge, unsigned char jc, unsigned char c)
{
#if defined(WIN32)
	if (judge->ascii && jc == c) { return 1; }
#else
	if (jc == c) { return 1; }
#endif
	return 0;
}

char *jjudge_replace_char(char *buf, int from, int to)
{
	char *p = buf;

	jjudge_t judger;
	jjudge_init(&judger);

	while (*p) {
		unsigned char jc = *p;
		unsigned char c = from;
		if (jjudge_equal(&judger, jc, c)) *p = to;
		p++;
	}
	return buf;
}
