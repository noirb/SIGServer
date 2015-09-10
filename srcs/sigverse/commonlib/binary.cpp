#include <sigverse/commonlib/binary.h>
#include <assert.h>
#include <string.h>

char * binary_get_string_incr(char **ptr)
{
	unsigned len = BINARY_GET_DATA_L_INCR(*ptr, unsigned);
	if (len <= 0) { return NULL; }
	char *buf = new char[len+1];
	memcpy(buf, *ptr, len);
	*ptr += BINARY_STRING_EXT_LEN(len);
	buf[len] = 0;
	return buf;
}

void binary_free_string(char **p)
{
	if (*p) {
		delete [] *p;
		*p = NULL;
	}
}

