/*
 * Modified by Okamoto on 2011-03-31
 */

#ifndef infix2postfix_h
#define infix2postfix_h

#include <string>
#include <string.h>
#include <stdlib.h>
#include <map>

char *infix2postfix(char *equation, std::map<std::string, std::string> *dict);

#endif /* infix2postfix_h */

