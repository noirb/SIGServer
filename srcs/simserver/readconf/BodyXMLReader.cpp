/*
 * Modified by Okamoto on 2011-03-25
 */

#include "BodyXMLReader.h"
#include "TransformStack.h"
#include "XMLUtils.h"
#include "JointParser.h"
#include "Joint.h"
#include "BodyFactory.h"
#include "SParts.h"
#include "SSimObj.h"
#include "ODEWorld.h"
#include "ODEObj.h"
#include "Eval.h"
#include "SSimObjBuilder.h"

#include <stdio.h>

#include <string>
#include <vector>
#include <stack>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

#ifdef CONF_DUMP
#define ENABLE_DUMP
#endif
#include "ParserDump.h"

class BodyParser
{
private:
	TransformStack m_transform;
	SSimObjBuilder m_b;

public:
	BodyParser(SSimObjBuilder &b) : m_b(b)
	{
	}
	void parse(DOMNode &n);
private:
	void parseChildren(DOMNode &n)
	{
		parse(n);
	}
};
	
void BodyParser::parse(DOMNode &target)
{
	DOMNode *curr = target.getFirstChild();

	std::map<std::string, std::string> dict;
	
	while (curr) {
		char *name = XMLString::transcode(curr->getNodeName());

		if (strcmp(name, "joint") == 0) {
			Eval eval(&dict);
			JointParser parser(m_transform.curr(), m_b);
			parser.parse(*curr, eval);
		} else if (strcmp(name, "define-param") == 0) {
			char *n = XMLUtils::getAttribute(*curr, "name");
			char *v = XMLUtils::getAttribute(*curr, "value");
			if (n && v) {
				dict[n] = v;
			} 
			XMLString::release(&n);
			XMLString::release(&v);
		}
		XMLString::release(&name);
		curr = curr->getNextSibling();
	}

}


bool BodyXMLReader::read(const char *fname)
{
	bool ret = false;
	SSimObjBuilder b(m_obj, m_w);

	XercesDOMParser parser;
	parser.setDoNamespaces(true);

	parser.parse(fname);
	DOMDocument *doc = parser.getDocument();
	DOMElement *root = doc->getDocumentElement();
	char *name = XMLString::transcode(root->getNodeName());
	DUMP1(("%s\n", name));

	if (strcmp("body", name) == 0) {
		try {
			BodyParser bparser(b);
			bparser.parse((DOMNode&)*root);
			//			TRANSFORM_TEST(bparse).curr;
			ret = true;
		} catch(XMLUtils::Exception &e) {
			fprintf(stderr, "%s : %s\n", fname, e.msg());
		} catch(Eval::Exception &e) {
			fprintf(stderr, "%s : eval exception : \"%s\"\n", fname, e.equation());
		}
	} 
	XMLString::release(&name);
	return ret;
}

