/*
 * Modified by Okamoto on 2011-03-31
 */

#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <stack>
#include <stdlib.h>

#define TERMINATOR '\0'
#define CHAR_ZERO  '0'
#define CHAR_TO_NUMBER(c)  ((c) - CHAR_ZERO)

#define CHAR_PLUS   '+'
#define CHAR_MINUS  '-'
#define CHAR_MULTI  '*'
#define CHAR_DIV    '/'


#define is_operator(c)	\
((c) == CHAR_PLUS || \
 (c) == CHAR_MINUS || \
 (c) == CHAR_MULTI || \
 (c) == CHAR_DIV)

static double calc(double a, double b, char op)
{
	switch(op) {
		case CHAR_PLUS:
			return (a + b);
		case CHAR_MINUS:
			return (a - b);
		case CHAR_MULTI:
			return (a * b);
		case CHAR_DIV:
			return (a / b);
		default:
			assert(0);
	}
	return 0.0;
}

int rpnCalc(char *equation, double *value)
{
	int  ret = 0;
	char *p;

	std::stack<double> stack;
	for (p = equation; *p != TERMINATOR; p++) {
		
		if (isdigit(*p)) {
			char numstr[128] = {*p};
			char *pp = &numstr[1]; p++;

			while (!isspace(*p)) {
				*pp++ = *p++;
			}
			*pp = 0;
			double v = atof(numstr);
			stack.push(v);
		} else if (is_operator(*p)){
			double n2 = stack.top(); stack.pop();
			double n1 = stack.top(); stack.pop();
			double r = calc(n1, n2, *p);
			stack.push(r);
		}
	}
	if (stack.size() != 1) {
		ret = -1;
		fprintf(stderr, "Illegal format of RPN\n");
	} else {
		*value = stack.top();
		stack.pop();
	}

	return ret;
}

/* for test */
#ifdef rpnCalc_test

int main()
{
	double	value;
	//	char *formula = "5.0 27 3 7 * + * 22 +";
	//char *formula = "5.0 27 +";
	if (rpnCalc(formula, &value) < 0) {
		return 1;
	}
	printf("%f\n", value);
	return 0;
}
	
#endif 

#ifdef rpnBC_test

#include "infix2postfix.h"

int main()
{
	char buf[1024];
	while (fgets(buf, sizeof(buf), stdin) != 0) {

		char *rpn = infix2postfix(buf, 0);
		if (!rpn) {
			fprintf(stderr, "rpn conversion error\n");
			continue;
		}
		printf("rpn : %s\n", rpn);
		double	value;
		//	char *formula = "5.0 27 3 7 * + * 22 +";
		//char *formula = "5.0 27 +";
		if (rpnCalc(rpn, &value) == 0) {
			printf("%f\n", value);
		}
		
	}
	return 0;
}
	
#endif 
