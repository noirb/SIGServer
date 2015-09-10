/*
 * Created by okamoto on 2011-03-25
 */

#ifndef jjudge_h
#define jjudge_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jjudge_s
{
	int ascii;
} jjudge_t;

extern void jjudge_init(jjudge_t *judge);
extern int jjudge_equal(jjudge_t *judge, unsigned char jc, unsigned char c);
extern int jjudge_equal_preserve(jjudge_t *judge, unsigned char jc, unsigned char c);

extern char *jjudge_replace_char(char *buf, int from, int to);

#ifdef __cplusplus
}
#endif

#endif /* jjudge_h */
