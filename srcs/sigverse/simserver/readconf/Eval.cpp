/*
 * Modified by Okamoto on 2011-03-25
 */

#include "Eval.h"
#include "infix2postfix.h"
#include "rpnCalc.h"
#include <assert.h>

double Eval::eval(const char *eq_)
{
	assert(eq_);
	assert(strlen(eq_) > 0);

#ifndef WIN32
	char *eq = strdup(eq_);
#else
	char *eq = _strdup(eq_);
#endif
	char *post = infix2postfix(eq, m_dict);
	if (!post) {
		free(eq);
		throw Exception(eq_);
	}

	double v;
	int r = rpnCalc(post, &v);
	if (r != 0) {
		free(eq);
		throw Exception(eq_);
	}
		
	free(eq);
	return v;
}

