/*
 * Modified by Okamoto on 2011-03-25
 */

#ifndef JointParser_h
#define JointParser_h

#include <xercesc/dom/DOMNode.hpp>

#include "TransformStack.h"

class Joint;
class SParts;
class BodyFactory;
class Eval;

class JointParser
{
private:
	TransformStack m_transform;
	BodyFactory &m_f;
	Joint	*m_joint;
	SParts	*m_parts;
	SParts	*m_parentParts;
	SParts	*m_childParts;
public:
	JointParser(const Transform &t, BodyFactory &f, SParts *parentParts = 0)
		: m_transform(t), m_f(f),
		  m_joint(0), m_parts(0), m_parentParts(parentParts), m_childParts(0) {;}
public:
	void	parse(xercesc::DOMNode &, Eval &eval);
	void 	operator()(xercesc::DOMNode &n, Eval &eval) { parse(n, eval); }

	Joint	*getJoint() { return m_joint; }
	SParts	*getParts()
	{
		return m_parts? m_parts: m_childParts;
	}
private:
	void 	parseChildren(xercesc::DOMNode &, Eval &);
};

#endif // JointParser_h
 

