/* $Id: fname.c,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */

#include <string.h>
#include <stdio.h>
#include <sigverse/simserver/jjudge.h>
#include <sigverse/simserver/fname.h>

char *fname_as_file(char *buf)
{
	char *mark = NULL;
	char *p = buf;

	jjudge_t judger;
	jjudge_init(&judger);

	while (*p) {
		unsigned char c = *p;
		if (jjudge_equal(&judger, c, FNAME_FS_CHAR)) { if (!mark) mark = p; }
		else mark = NULL;
		p++;
	}

	if (mark) *mark = '\0';
	return buf;
}

char *fname_as_dir(char *buf)
{ 
	return strcat(fname_as_file(buf), FNAME_FS_CSTR);
}

char *fname_get_parent(char *buf)
{
	char *mark = NULL;
	char *p = buf;
	int conti = 0;

	jjudge_t judger;
	jjudge_init(&judger);

	fname_as_file(buf);

	while (*p) {
		unsigned char c = *p;
		if (jjudge_equal(&judger, c, FNAME_FS_CHAR))
		{
			if (!conti) { mark = p; conti++; }
		}
		else conti = 0;
		p++;
	}

	if (mark) { *mark = '\0'; return buf; }
	return NULL;
}

char *fname_get_file(char *buf)
{
	char *mark = NULL;
	char *p = buf;

	jjudge_t judger;
	jjudge_init(&judger);

	fname_as_file(buf);

	while (*p) {
		unsigned char c = *p;
		if (jjudge_equal(&judger, c, FNAME_FS_CHAR)) { mark = p; }
		p++;
	}

	if (mark) {
		mark++;
		p = buf;
		while ((*p++ = *mark++));
	}

	return buf;
}

