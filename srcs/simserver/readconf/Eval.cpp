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

	char *eq = strdup(eq_);
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

#ifdef Eval_test
int main()
{

	char *p = "2.5 + 1.75 * FOO1_FO0";
	Eval::std::map<std::string, std::string> dict;
	dict["FOO1_FOO"] = "2.0";

	Eval eval(&dict);
	try {
		double v = eval(p);
		printf("%s = %f\n", p, v);
	} catch(Eval::Exception &) {
		fprintf(stderr, "exception\n");
		return 1;
	}
	return 0;

}
#endif

