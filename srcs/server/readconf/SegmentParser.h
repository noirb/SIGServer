/*
 * Created by Okamoto on 2011-03-25
 */

#ifndef SegmentParser_h
#define SegmentParser_h

#include "TransformStack.h"

#include <string>

#include <xercesc/dom/DOMNode.hpp>

class SParts;
class Joint;
class Eval;

class SegmentParser
{
	TransformStack m_transform;
private:
	std::string m_name;
	SParts *    m_parts;
	Eval &      m_eval;
public:
	SegmentParser(const Transform &t, Eval &eval)
		: m_transform(t), m_parts(NULL), m_eval(eval)
	{
		;
	}
	~SegmentParser() {

	}

	SParts * parse(xercesc::DOMNode &);

//	SParts * operator()(xercesc::DOMNode &n)
//	{
//		return parse(n);
//	}
private:
	void parseNode(xercesc::DOMNode &);
	void parseTransform(xercesc::DOMNode &);
};


#endif // SegmentParser_h
 

