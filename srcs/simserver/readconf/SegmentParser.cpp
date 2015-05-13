/*
 * Created by Okamoto on 2011-03-25
 */

#include "SegmentParser.h"
#include "XMLUtils.h"
#include "Rotation.h"
#include "Vector3d.h"
#include "Joint.h"
#include "SParts.h"
#include "Transform.h"
#include "GeometryParser.h"
#include "ODEObj.h"

#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

#ifdef CONF_DUMP
#define ENABLE_DUMP
#define ENABLE_DUMP1
#endif
#include "ParserDump.h"

void SegmentParser::parseTransform(DOMNode &target)
{
	DOMNode *p = target.getFirstChild();
	m_transform.push();

	while (p) {
		char *s = XMLString::transcode(p->getNodeName());

		if (strcmp(s, "translation") == 0) {
			double *v = XMLUtils::parseTranslation(*p, m_eval);
			m_transform.curr().push(Vector3d(v[0], v[1], v[2]));
			DUMP(("TRANSLATION(%f, %f, %f)\n", v[0], v[1], v[2]));
		} else if (strcmp(s, "rotation") == 0) {
			double *v = XMLUtils::parseRotation(*p);
			Rotation r;
			r.setAxisAndAngle(v[0], v[1], v[2], v[3]);
			m_transform.curr().push(r);
		} else if (strcmp(s, "shape") == 0) {
			parseNode(*p);
		}
		XMLString::release(&s);
		p = p->getNextSibling();
	}
	DUMP1(("SEGMENT TRANSFORM TEST "));
	TRANSFORM_TEST(m_transform.curr());

	m_transform.pop();
}

void SegmentParser::parseNode(DOMNode &target)
{
	DOMNode *p = target.getFirstChild();

	while (p) {
		char *s = XMLString::transcode(p->getNodeName());

		if (strcmp(s, "transform") == 0) {
			parseTransform(*p);
		} else if (strcmp(s, "geometry") == 0) {
			GeometryParser parser(m_name.c_str(), m_transform.curr());
			SParts *parts = parser.parse(*p, m_eval);
			if (parts) {
				m_parts = parts;
			}
		} 
		XMLString::release(&s);
		p = p->getNextSibling();
	}
}

SParts * SegmentParser::parse(DOMNode &target)
{
	char *nameAttr = XMLUtils::getAttribute(target, "name");
	m_name = nameAttr;
	XMLString::release(&nameAttr);

	if (m_name.length() > 0) {
		DUMP1(("segment name = %s\n", m_name.c_str()));
	}
	DOMNode *p = target.getFirstChild();
	while (p) {
		char *s = XMLString::transcode(p->getNodeName());
		if (strcmp(s, "children") == 0) {
			parseNode(*p);
		} 
		XMLString::release(&s);
		p = p->getNextSibling();
	}
	return m_parts;
}


