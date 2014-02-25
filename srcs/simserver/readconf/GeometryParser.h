/*
 * Modified by Okamoto on 2011-03-25
 */

#ifndef GeometryParser_h
#define GeometryParser_h

#include <xercesc/dom/DOMNode.hpp>

#include <string>

class Transform;
class SParts;
class Eval;

class GeometryParser
{
private:
	std::string m_name;
	const Transform & m_t;
public:
	GeometryParser(const char *name, const Transform &t) : m_name(name), m_t(t) {;}
public:
	SParts * operator()(xercesc::DOMNode &n, Eval &eval)
	{
		return parse(n, eval);
	}
	SParts * parse(xercesc::DOMNode &target, Eval &);
};

#endif // GeometryParser_h
 

