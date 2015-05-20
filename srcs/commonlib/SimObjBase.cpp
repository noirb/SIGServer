/* 
 * Written by okamoto on 2011-07-27
 * Modified by Tetsunari Inamura on 2013-12-29
 *   Add English comments (Translation from v2.2.0 is finished)
 *   Reduce warning in compilation
 * Modified by Tetsunari Inamura on 2013-12-30
 *   Change setVelocity to setLinearVelocity
 */

#include "SimObjBase.h"
#include "Parts.h"
#include "binary.h"
#include "Logger.h"
#include <iostream>

#ifdef SIGVERSE_OGRE_CLIENT
// begin(added by sekikawa)(2009/2/12)
#include "SgvSimServHelper.h"
// end(added by sekikawa)
#endif	// SIGVERSE_OGRE_CLIENT

SimObjBase::SimObjBase()
	: m_attached(false), m_ops(OP_NOT_SET)
{
}


SimObjBase::~SimObjBase()
{
	for (AttrM::iterator i=m_attrs.begin(); i!=m_attrs.end(); i++) {
		Attribute *p = i->second;
		delete p;
	}
	m_attrs.clear();
}

Attribute & SimObjBase::getAttr(const char *name) const
{
	AttrM::const_iterator i = m_attrs.find(name);
	if (i == m_attrs.end()) { throw NoAttributeException(name); }
	return *i->second;
}

bool SimObjBase::isAttr(const char *name)
{
	AttrM::const_iterator i = m_attrs.find(name);
	if (i == m_attrs.end()) { return false; }
	else { return true; }
}

Attribute * SimObjBase::hasAttr(const char *name) const
{
	AttrM::const_iterator i = m_attrs.find(name);
	return (i != m_attrs.end())? i->second: NULL;
}

void SimObjBase::setForce(double fx_, double fy_, double fz_)
{
	if (!dynamics()) { return; }

	fx(fx_); fy(fy_); fz(fz_);
}

void SimObjBase::setTorque(double x_, double y_, double z_)
{
	if (!dynamics()) { return; }

	tqx(x_); tqy(y_); tqz(z_);
}

/**
 * @brief Set linear velocity of the entity
 * @param vx X element of velocity
 * @param vy Y element of velocity
 * @param vz Z element of velocity
 */
// Changed from setVelocity: by inamura on 2013-12-30
void SimObjBase::setLinearVelocity(double vx_,double vy_,double vz_)
{
	// Set is not permitted in dynamics on mode
	if (!dynamics()) { return; }
	vx(vx_);
	vy(vy_);
	vz(vz_);
}


/**
 * @brief Set angular velocity of the entity
 * @param x_ X element of angular velocity [rad/s]
 * @param y_ Y element of angular velocity [rad/s]
 * @param z_ Z element of angular velocity [rad/s]
 */
void SimObjBase::setAngularVelocity(double x_,double y_,double z_)
{
	// Set is not permitted in dynamics on mode
	if (!dynamics()) { return; }
	avx(x_);
	avy(y_);
	avz(z_);
}


void SimObjBase::setPosition(const Vector3d &v)
{
	if (dynamics()) { return; }

	x(v.x()); y(v.y()); z(v.z());
}

void SimObjBase::setPosition(double x_, double y_, double z_)
{
	if (dynamics()) { return; }

	x(x_); y(y_); z(z_);
}


#ifdef USE_ODE
void SimObjBase::setAxisAndAngle(double ax, double ay, double az, double angle)
{
	if (dynamics()) { return; }

	Rotation r;
	r.setAxisAndAngle(ax, ay, az, angle);
	const dReal *q = r.q();
	setQ(q);
}

//for okonomiyaki
/*!
 * @brief It rotates for the specification of the relative angle.
 * @param[in] x-axis rotation weather(i of quaternion complex part)
 * @param[in] y-axis rotation weather(j of quaternion complex part)
 * @param[in] z-axis rotation weather(k of quaternion complex part)
 * @param[in] flag for ansolute / relational (1.0=absolute, else=relational)
 */
void SimObjBase::setAxisAndAngle(double ax, double ay, double az, double angle, double direct)
{
	// The angle is used now at the relative angle specification.
	if (dynamics()) { return; }

	Rotation r;
	if (direct != 1.0) r.setQuaternion(qw(), qx(), qy(), qz());

	// alculate relational angle
	r.setAxisAndAngle(ax, ay, az, angle, direct);
	const dReal *q = r.q();
	setQ(q);
}


// begin(added by sekikawa)(2009/2/12)
#else
#ifdef SIGVERSE_OGRE_CLIENT
void SimObjBase::setAxisAndAngle(double ax, double ay, double az, double degAngle)
{
	Sgv::Quaternion q(degAngle*180/M_PI, ax, ay, az);

	qw(q.qw());
	qx(q.qx());
	qy(q.qy());
	qz(q.qz());
}

void SimObjBase::setAxisAndAngle(double ax, double ay, double az, double Angle, double direct)
{
}
#else

void SimObjBase::setAxisAndAngle(double ax, double ay, double az, double Angle)
{
}

void SimObjBase::setAxisAndAngle(double ax, double ay, double az, double Angle, double direct)
{
}

#endif	// SIGVERSE_OGRE_CLIENT
// end(added by sekikawa)
#endif

void SimObjBase::setQ(const dReal *q)
{
	int i = 0;
	qw(q[i]); i++;
	qx(q[i]); i++;
	qy(q[i]); i++;
	qz(q[i]); i++;
}

void SimObjBase::setRotation(const Rotation &r)
{
	if (dynamics()) { return; }
	
	const dReal *q = r.q();
	setQ(q);
}

void SimObjBase::copy(const SimObjBase &o)
{
	const AttrM &attrs = o.attrs();

	for (AttrM::const_iterator i=attrs.begin(); i!=attrs.end(); i++) {

		Attribute *a = i->second;

		Attribute *to = hasAttr(a->name());
		if (to != NULL) {
			to->value().copy(a->value());
		} else {
			this->push(new Attribute(*a));
		}
	}

	m_ops = o.m_ops;
}

/**
 * @brief  Binalize of the entity
 * @param  Size of binalized data
 * @return Binalized data
 */
char *SimObjBase::toBinary(int &n)
{
	const int RESIZE = 1000;
	static int bufsize = RESIZE;
	static char *buf = new char[bufsize];

	char *p = buf;
	p += sizeof(DataLengthType); // skip datalen

	BINARY_SET_DATA_L_INCR(p, Id, id());
	BINARY_SET_STRING_INCR(p, name());
	BINARY_SET_STRING_INCR(p, classname());
	short attached = isAttached();
	BINARY_SET_DATA_S_INCR(p, short, attached);
	BINARY_SET_DATA_L_INCR(p, Operation, m_ops);

	DataOffsetType	ao_offset = p - buf;
	BINARY_SET_DATA_S_INCR(p, DataOffsetType, 0); // skip attr offset

	DataOffsetType	bo_offset = p - buf;
	BINARY_SET_DATA_S_INCR(p, DataOffsetType, 0); // skip body offset

	// attrs
	{
		DataOffsetType 	attrs_offset = p - buf;
		BINARY_SET_DATA_S(buf + ao_offset, DataOffsetType, attrs_offset);

		BINARY_SET_DATA_S_INCR(p, DataLengthType, 0); // skip attrs size

		DataLengthType attrssize = sizeof(DataLengthType);

		for (AttrM::iterator i=m_attrs.begin(); i!=m_attrs.end(); i++) {

			int head = p - buf;
			Attribute *attr = i->second;
			int nn;
			char *pp = attr->toBinary(nn);
			if (head + nn >= bufsize) {
				int newsize = bufsize + RESIZE;
				char *newbuf = new char[newsize];
				memcpy(newbuf, buf, head);
				delete buf;
				buf = newbuf;
				bufsize = newsize;
				p = buf + head;
			}
			memcpy(p, pp, nn);
			p += nn;
			attrssize += nn;
		}
		BINARY_SET_DATA_S(buf + attrs_offset, DataLengthType, attrssize);
	}


	// set body offset value
	{
		DataOffsetType body_offset = p - buf;
		BINARY_SET_DATA_S(buf + bo_offset, DataOffsetType, body_offset);

		// body

		BINARY_SET_DATA_S_INCR(p, DataLengthType, 0); // skip body size
		DataLengthType bodysize = sizeof(DataLengthType);
		//for (PartsM::iterator i=m_parts.begin(); i!=m_parts.end(); i++) {

		PartsIterator *itr = getPartsIterator();
		Parts *parts = NULL;

		while (itr && (parts = itr->next()) != NULL) {

			if (parts->isBlind()) { continue; }
			
			int head = p - buf;
			//Parts *parts = i->second;

			int nn;
			char *pp = parts->toBinary(nn);
			if (head + nn >= bufsize) {
				int newsize = bufsize + RESIZE;
				char *newbuf = new char[newsize];
				memcpy(newbuf, buf, head);
				delete buf;
				buf = newbuf;
				bufsize = newsize;
				p = buf + head;
			}
			memcpy(p, pp, nn);
			p += nn;
			bodysize += nn;
		}
		BINARY_SET_DATA_S(buf + body_offset, DataLengthType, bodysize);
		delete itr;
	}

	n = p-buf;
	// set datalen value
	BINARY_SET_DATA_S(buf, DataLengthType, n);

	return buf;
}

void SimObjBase::dump()
{
	printf("Agent(%d) :  %s\n", id(), name());
	for (AttrM::const_iterator i=m_attrs.begin(); i!=m_attrs.end(); i++) {
		std::cout << i->second->toString()<< std::endl;
	}

	PartsIterator *itr = getPartsIterator();
	Parts *p = NULL;
	while (itr && (p = itr->next()) != NULL) {
		p->dump();
	}
	delete itr;
}

#define ARY_SIZE(ARY) (int)(sizeof(ARY)/sizeof(ARY[0]))

bool SimObjBase::checkAttrs()
{
	// added by yahara@tome (2011/02/22)
	// TODO: strange codes; it should be modified:  by inamura on 2013-12-29
#define DEFINE_ATTR_STRING(NAME, TOKEN) (char *)TOKEN,
#define DEFINE_ATTR_DOUBLE(NAME, TOKEN) (char *)TOKEN,
#define DEFINE_ATTR_BOOL(NAME, TOKEN)   (char *)TOKEN, // modified by inamura on 2013-12-29
	// TODO: The above three definitions will be used to extract just attribution string from SimObjBaseAttrs.h
	//       This way is too dangerous. by inamura
	static char *namesForAgent[] = {
		(char *)"name", (char *)"class",
#include "SimObjBaseAttrs.h"
	};

	static char *namesForEntities[] = {
		(char *)"name", (char *)"class",
#define NO_AGENT_ATTRS
#include "SimObjBaseAttrs.h"
#undef NO_AGENT_ATTRS
	};

	char **names = isAgent()? namesForAgent: namesForEntities;
	const char *myname = name();
	bool b = true;
	for (int i=0; i<ARY_SIZE(names); i++) {
		const char *attrname = names[i];
		if (hasAttr(attrname) == NULL) {
			b = false;
			LOG_ERR(("%s : no attribute \"%s\"", myname? myname:"noname", attrname));
		}
	}
	return b;
#undef DEFINE_ATTR_DOUBLE
#undef DEFINE_ATTR_BOOL
}

