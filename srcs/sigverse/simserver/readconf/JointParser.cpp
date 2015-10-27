/*
 * Modified by Okamoto on 2011-03-25
 */

#include <sigverse/simserver/readconf/JointParser.h>
#include <sigverse/simserver/readconf/Transform.h>
#include <sigverse/simserver/readconf/XMLUtils.h>
#include <sigverse/simserver/Joint.h>
#include <sigverse/simserver/SParts.h>
#include <sigverse/simserver/readconf/SegmentParser.h>
#include <sigverse/simserver/readconf/BodyFactory.h>

#include <vector>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

#define ARRAY_SIZE(ARY) ( (int)(sizeof(ARY)/sizeof(ARY[0])) )

#ifdef CONF_DUMP
#define ENABLE_DUMP
#define ENABLE_DUMP1
#endif
#include <sigverse/simserver/readconf/ParserDump.h>

void JointParser::parse(DOMNode &target, Eval &eval)
{
	char *name = XMLUtils::getAttribute(target, "name");
	DUMP1(("name %s\n", name));

	m_transform.push();

	DOMNode *p = target.getFirstChild();

	while (p) {
		char *s = XMLString::transcode(p->getNodeName());
		//		printf("%s\n", s);

		if (strcmp(s, "type") == 0) {

			char *type = XMLUtils::parseJointType(*p);
			DUMP1(("type=\"%s\"\n", type));
			Joint *j = 0;

			if (strcmp(type, "fixed") == 0) {
				j = new FixedJoint(name);
			} else if (strcmp(type, "ball") == 0) {
				j = new BallJoint(name);
			} else if (strcmp(type, "hinge") == 0) {
				j = new HingeJoint(name, Vector3d(0, 1, 0));
			}
			
			if (j) {
				m_joint = j;
			}
			XMLString::release(&type);
			/*
		} else if (strcmp(s, "limitOrientation") == 0) {
			double *v = XMLUtils::parseLimitOrientation(*p);
			if (m_joint) {
				delete m_joint;
			}
			m_joint = new HingeJoint(name, Vector3d(v[0], v[1], v[2]));
			*/
				
		} else if (strcmp(s, "translation") == 0) {
			double * values = XMLUtils::parseTranslation(*p, eval);
			DUMP1(("translation (%f, %f, %f)\n", values[0], values[1], values[2]));
			m_transform.curr().push(Vector3d(values[0], values[1], values[2]));

		} else if (strcmp(s, "rotation") == 0) {
			double *values = XMLUtils::parseRotation(*p);
			DUMP1(("rotation (%f, %f, %f, %f)\n", values[0], values[1], values[2], values[3]));
			Rotation rot;
			rot.setAxisAndAngle(values[0], values[1], values[2], values[3]);
			m_transform.curr().push(rot);
			
		} else if (strcmp(s, "axis") == 0) {
			double *values = XMLUtils::parseAxis(*p);
			DUMP1(("axis (%f, %f, %f)\n", values[0], values[1], values[2]));
			
			if (m_joint) {
				if (m_joint->type() == Joint::TYPE_HINGE) {
					HingeJoint *hinge = static_cast<HingeJoint*>(m_joint);
					hinge->setAxis(values[0], values[1], values[2]);
				} else {
					DUMP1(("not hinge type joint. axis ingored\n"));
				}
			} else {
				DUMP1(("no joint type"));
			}

		} else if (strcmp(s, "children") == 0) {
			DUMP1(("JOINT TRANSFORM TEST \n"));
			TRANSFORM_TEST(m_transform.curr());
			parseChildren(*p, eval);
		}
		
		p = p->getNextSibling();
		XMLString::release(&s);
	}
	{
		Transform &t = m_transform.curr();
		const Vector3d &v = t.translation();
		if (m_joint) {
			m_f.addJoint(m_joint);
			DUMP(("Joint : (%f, %f, %f)\n", v.x(), v.y(), v.z()));
			m_joint->setAnchor(v.x(), v.y(), v.z());
		}
	}
	m_transform.pop();

	char *strs[] = { name,};
	for (int i=0; i<ARRAY_SIZE(strs); i++) {
		char *p = strs[i];
		XMLString::release(&p);
	}
}

void JointParser::parseChildren(xercesc::DOMNode &target, Eval &eval)
{
	std::vector<DOMNode *> joints;
	
	DOMNode *curr = target.getFirstChild();
	while (curr) {
		char *s = XMLString::transcode(curr->getNodeName());

		if (strcmp(s, "segment") == 0) {
			SegmentParser parser(m_transform.curr(), eval);
			SParts *p = parser.parse(*curr);
			if (p) {
				m_parts = p;
				m_f.addParts(p);
			}
		} else if (strcmp(s, "joint") == 0) {
			joints.push_back(curr);
		}
		curr = curr->getNextSibling();
		XMLString::release(&s);
	}

	// Parse joints
	for (std::vector<DOMNode *>::iterator i=joints.begin(); i!=joints.end(); i++) {
		DOMNode *node = *i;
		SParts *parts = m_parts? m_parts: m_parentParts;
			
		JointParser parser(m_transform.curr(), m_f, parts);
		parser.parse(*node, eval);
		SParts *child = parser.getParts();
		Joint *j = parser.getJoint();
		if (parts && child && j) {
			m_f.connect(j, parts, child);
		}
		m_childParts = child;
	}
}


