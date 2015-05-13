/*
 * Modified by Okamoto on 2011-03-31
 */

#include <assert.h>
#include <ctype.h>
#include <stack>
#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define TERMINATOR  '\0'
#define CHAR_SPACE  ' '

#define CHAR_LEFT_PAREN '('
#define CHAR_RIGHT_PAREN ')'
#define CHAR_PLUS   '+'
#define CHAR_MINUS  '-'
#define CHAR_MULTI  '*'
#define CHAR_DIV    '/'

#define is_operator(c) \
( \
 (c) == CHAR_PLUS || \
 (c) == CHAR_MINUS || \
 (c) == CHAR_MULTI || \
 (c) == CHAR_DIV || \
 (c) == CHAR_LEFT_PAREN || \
 (c) == CHAR_RIGHT_PAREN )

static char s_pr[256];

#define PR0 ((char)0)
#define PR1 ((char)1)
#define PR2 ((char)2)

#define PRIORITY(op) (s_pr[(int)(op)])
#define SET_PRIORITY(OP, LEVEL) s_pr[(int)(OP)] = (LEVEL)

static bool s_init = false;

static void init_priority_table()
{
	if (s_init) { return; }
	SET_PRIORITY(CHAR_LEFT_PAREN, PR0);
	SET_PRIORITY(CHAR_PLUS,       PR1);
	SET_PRIORITY(CHAR_MINUS,      PR1);
	SET_PRIORITY(CHAR_MULTI,      PR2);
	SET_PRIORITY(CHAR_DIV,        PR2);
	s_init = true;
}

#define BUF_SIZE 1024
static char s_buf[BUF_SIZE];

static bool isnumchar(char c)
{
	return isdigit(c) || c == '.';
}

char *infix2postfix(char *equation, std::map<std::string, std::string> *dict)
{
	char *p;
	char *to;
	std::stack<char> stack;

	init_priority_table();

	to = s_buf;

	enum Token {
		TOKEN_NOT_SET,
		OP, NUMBER, PARAM,
	};
	Token prev = TOKEN_NOT_SET;

	for (p = equation; *p != TERMINATOR; p++) {

		if (isspace(*p)) { continue; } 

		if (isnumchar(*p)) { 

			do {
				*to++ = *p++;
			} while(isnumchar(*p));
			*to++ = CHAR_SPACE;
			p--; 
			
			prev = NUMBER;

		} else if (is_operator(*p)){ 

			switch(*p) {
				case CHAR_LEFT_PAREN:
					stack.push(*p);
					break;
				case CHAR_RIGHT_PAREN:
					while(!stack.empty()) {
						char op = stack.top(); stack.pop();
						if (op == CHAR_LEFT_PAREN) { break; }
						*to++ = op;
						*to++ = CHAR_SPACE;
					}
					break;
				case CHAR_PLUS:
				case CHAR_MINUS:
					if (prev == TOKEN_NOT_SET || prev == OP) {
						*to++ = '0';
						*to++ = CHAR_SPACE;
					}
					// go through
				case CHAR_MULTI:
				case CHAR_DIV:
					while (!stack.empty()) {
						char op = stack.top();
						if (PRIORITY(*p) <= PRIORITY(op)) {
							stack.pop();
							*to++ = op;
							*to++ = CHAR_SPACE;
						} else {
							break;
						}
					}
					stack.push(*p);
					break;
			}
			prev = OP;
		} else if (isalpha(*p)) {
			if (!dict) {
				fprintf(stderr, "no dictionary\n");
				return 0;
			}
			char name[128] = {*p}; p++;
			char *pp = &name[1];

			while (isalnum(*p) || *p == '_') {
				*pp++ = *p++;
			}
			*pp = 0; p--;
			
			std::map<std::string, std::string>::iterator i = dict->find(name);
			if (i == dict->end()) {
				fprintf(stderr, "%s : no parameter value \n", name);
				return 0;
			}
			std::string v = i->second;
			strcpy(to, v.c_str());
			to += v.length();
			*to++ = CHAR_SPACE;
			prev = PARAM;
		}
	}

	while(!stack.empty()) {
		char op = stack.top();
		stack.pop();
		*to++ = op;
		*to++ = CHAR_SPACE;
	}
	*to = TERMINATOR; 
	return s_buf;
}

#ifdef TEST
#include <stdio.h>
int main()
{
/*	char *eq = "5*3+4";*/
//	char *eq = "5.0*(27+3*7/2*3) + 22 + FOO";
	char *eq = "-1";
	char *postfix;
	M dict;
	dict["FOO"] = "1.2";
	if (postfix = infix2postfix(eq, &dict)) {
		printf("%s\n", postfix);
	}
	return 0;
}

#endif

