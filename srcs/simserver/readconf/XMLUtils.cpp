/*
 * Modified by Okamoto on 2011-03-25
 */


#include "XMLUtils.h"
#include "Eval.h"
#include "Logger.h"

#include <stdarg.h>

#include <vector>
#include <string>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>


XERCES_CPP_NAMESPACE_USE

#define RELEASE(STR) if (STR) { XMLString::release(&STR); }

static char * getChildText(DOMNode &n)
{
	DOMNode *p = n.getFirstChild();
	while (p) {

		if (p->getNodeType() == DOMNode::TEXT_NODE) {
			DOMText *t = static_cast<DOMText*>(p);
			return XMLString::transcode(t->getTextContent());
		} 

		p = p->getNextSibling();
	}
	return NULL;
}

class Splitter
{
private:
	typedef std::vector<std::string> C;
private:
	C	m_strs;
public:
	int split(char *str)
	{

		while (true) {
			char *p = strtok(str, " \t\n\r");
			if (!p) { break; }
			m_strs.push_back(std::string(p));
			str = NULL;
		}
		return m_strs.size();
	}
		
			
	const char *operator[](int i)
	{
		return m_strs[i].c_str();
	}
};

class AtoF
{
public:
	double operator()(const char *p)
	{
		return atof(p);
	}
};
			    
template<int N, class EVAL> double * getTextDouble(DOMNode &n, EVAL &eval)
{
	static double v[N];
	char *text = getChildText(n);
	if (!text) { return NULL; }
	Splitter sp;
	if (sp.split(text) != N) {
		XMLString::release(&text);
		return NULL;
	}
	for (int i=0; i<N; i++) {
		v[i] = eval(sp[i]);
	}
	XMLString::release(&text);
	return v;
}

template<int N> double * getTextDouble(DOMNode &n)
{
	AtoF eval;
	return getTextDouble<N, AtoF>(n, eval);
}

double * XMLUtils::parseLimitOrientation(xercesc::DOMNode &n)
{
	double *v = getTextDouble<4>(n);
	if (!v) {
		throw Exception("no limitOrientation values");
	}
	return v;
}

double * XMLUtils::parseTranslation(DOMNode &n, Eval &eval)
{
	double *v = getTextDouble<3, Eval>(n, eval);
	if (!v) {
		throw Exception("no translation values");
	}
	return v;
}

double * XMLUtils::parseRotation(DOMNode &n)
{
	double *v = getTextDouble<4>(n);
	if (!v) {
		throw Exception("no rotation values");
	}
	return v;
}

double * XMLUtils::parseAxis(DOMNode &n)
{
	double *v = getTextDouble<3>(n);
	if (!v) {
		throw Exception("no axis values");
	}
	return v;
}

double * XMLUtils::parseSize(DOMNode &n, Eval &eval)
{
	double *v = getTextDouble<3, Eval>(n, eval);
	if (!v) {
		throw Exception("no size values");
	}
	return v;
}

double XMLUtils::parseRadius(DOMNode &n, Eval &eval)
{
	double *v = getTextDouble<1, Eval>(n, eval);
	if (!v) {
		throw Exception("no radius values");
	}
	return v[0];
}

char * XMLUtils::parseJointType(DOMNode &target)
{
	return getChildText(target);
}


char * XMLUtils::getAttribute(DOMNode &n, const char *attrname)
{
	char *ret;
	DOMNamedNodeMap *nmap = n.getAttributes();
	DOMNode *nameNode = NULL;
	if (nmap == NULL) { goto err; }

	{
		XMLCh *xmlstr = XMLString::transcode(attrname);
		nameNode = nmap->getNamedItem(xmlstr);
		if (!nameNode) {
			goto err;
		}

		ret = XMLString::transcode(nameNode->getNodeValue());
		XMLString::release(&xmlstr);
		return ret;

	}
 err:
	char *p = XMLString::transcode(n.getNodeName());
	char buf[128];
	strcpy(buf, p);
	XMLString::release(&p);
	throw Exception("%s has no %s attribute", buf, attrname);
}

XMLUtils::Exception::Exception(const char *fmt, ...) 
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	m_msg = buf;
}

void XMLUtils::Exception::set(const char *fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	m_msg = buf;
}

void XMLUtils::dumpAttributeList(const xercesc::AttributeList &attrs) {
	for (unsigned int i=0; i<attrs.getLength(); i++) {
		char *text = XMLString::transcode(attrs.getName(i));
		char *value = XMLString::transcode(attrs.getValue(i));
		LOG_ERR(("\tname=\"%s\" value=\"%s\"", text? text:"", value? value:""));
		RELEASE(text);
		RELEASE(value);
	}
}
