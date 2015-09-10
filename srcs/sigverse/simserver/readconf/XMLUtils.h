/*
 * Modified by Okamoto on 2011-03-25
 */

#ifndef XMLUtils_h
#define XMLUtils_h

#include <string>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/sax/AttributeList.hpp>

class Eval;

class XMLUtils
{
public:
	class Exception
	{
	private:
		std::string m_msg;
	public:
		Exception() {}
		Exception(const char *fmt, ...);
		void	set(const char *fmt, ...);
		const char *msg() { return m_msg.c_str(); }
	};

	class NoAttributeException : public Exception
	{
	public:
		NoAttributeException(const char *node, const char *attr) : Exception() {
			set("\"%s\" : \"%s\" does not have such attribute", attr, node);
		}
	};

	class NoChildNodeException : public Exception
	{
	public:
		NoChildNodeException(const char *node, const char *attr) : Exception() {
			set("\"%s\" : \"%s\" does not have such child node", attr, node);
		}
	};

public:
	static double * parseTranslation(xercesc::DOMNode &n, Eval &);
	static double * parseRotation(xercesc::DOMNode &n);
	static double * parseAxis(xercesc::DOMNode &n);
	static double * parseLimitOrientation(xercesc::DOMNode &n);
	static double * parseSize(xercesc::DOMNode &n, Eval &);
	static double parseRadius(xercesc::DOMNode &n, Eval &);
	static char * parseJointType(xercesc::DOMNode &target);
	static char * getAttribute(xercesc::DOMNode &n, const char *attrname);
	static void dumpAttributeList(const xercesc::AttributeList &attrs);
};


#endif // XMLUtils_h
 

