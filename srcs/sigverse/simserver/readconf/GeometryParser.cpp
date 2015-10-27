/*
 * Modified by Okamoto on 2011-03-25
 */

#include <sigverse/simserver/readconf/GeometryParser.h>
#include <sigverse/simserver/readconf/XMLUtils.h>
#include <sigverse/commonlib/Size.h>
#include <sigverse/commonlib/Rotation.h>
#include <sigverse/commonlib/Vector3d.h>
#include <sigverse/simserver/SParts.h>
#include <sigverse/simserver/readconf/Transform.h>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

#ifdef CONF_DUMP
#define ENABLE_DUMP
#define ENABLE_DUMP1
#endif
#include <sigverse/simserver/readconf/ParserDump.h>

SParts * GeometryParser::parse(DOMNode &target, Eval &eval)
{
	SParts *parts = NULL;
	char *type = XMLUtils::getAttribute(target, "type");
	if (type == NULL) {
		throw XMLUtils::Exception("<geometry> : no \"type\" attribute");
	}
	DUMP1(("geometry type = %s\n", type));

	double radius = -1;
	Size *sz = NULL;
	DOMNode *p = target.getFirstChild();

	while (p) {
		char *s = XMLString::transcode(p->getNodeName());

		if (strcmp(s, "size") == 0) {
			double *v = XMLUtils::parseSize(*p, eval);
			DUMP1(("size = (%f, %f, %f)\n", v[0], v[1], v[2]));
			sz = new Size(v[0], v[1], v[2]);
		} else if (strcmp(s, "radius") == 0) {
			radius = XMLUtils::parseRadius(*p, eval);
		}
		XMLString::release(&s);
		p = p->getNextSibling();
	}

	if (m_name.length() > 0) {

		const Rotation &r = m_t.rotation();
		const Vector3d &v = m_t.translation();

		if (strcmp(type, "box") == 0)  {
			DUMP(("box"));
			if (!sz) {
				throw XMLUtils::Exception("<geometry type=box> : no size");
			}
			DUMP(("name = %s\n", m_name.c_str()));
			parts = new SBoxParts(m_name.c_str(), Position(v.x(), v.y(), v.z()), *sz);
		} else if (strcmp(type, "sphere") == 0) {
			DUMP(("sphere"));
			if (radius < 0) {
				throw XMLUtils::Exception("<geometry type=sphere> : no radius");
			}
			parts = new SSphereParts(m_name.c_str(), Position(v.x(), v.y(), v.z()), radius);
		}

		if (parts) {
			const dReal *q = r.q();
			DUMP(("q = (%f, %f, %f, %f)\n", q[0], q[1], q[2], q[3] ));
			parts->setInitialQuaternion(q[0], q[1], q[2], q[3]);
		}
		
	}
	XMLString::release(&type);
	return parts;
}

