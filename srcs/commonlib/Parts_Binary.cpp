/* $Id: Parts_Binary.cpp,v 1.2 2011-03-31 08:15:57 okamoto Exp $ */
#include "CParts.h"
#include "PartsCmpnt.h"
#include "binary.h"
#include <assert.h>

#ifdef IRWAS_OGRE_CLIENT
#include "IrcApp.h"
#include "IrcWorld.h"
#endif

char *	Parts::toBinary(int &n)
{
	const int RESIZE = 200;
	if (!m_buf) {
		m_bufsize = RESIZE;
		m_buf = new char[m_bufsize];
	}
	char *p = m_buf;
	BINARY_SET_DATA_S_INCR(p, DataLengthType, 0); // data length
	BINARY_SET_DATA_L_INCR(p, Id, m_id);
	BINARY_SET_DATA_S_INCR(p, PartsType, m_type);
	BINARY_SET_STRING_INCR(p, m_name.c_str());

	const dReal *pos = getPosition();

	for (int i=0; i<3; i++) {
		double v = pos[i];
		BINARY_SET_DOUBLE_INCR(p, v);
	}
	
	BINARY_SET_DATA_S_INCR(p, RotationType, ROTATION_TYPE_QUATERNION);

	const dReal *q = getQuaternion();

	for (int i=0; i<4; i++) {
		double v = q[i];
		BINARY_SET_DOUBLE_INCR(p, v);
	}

	PartsCmpnt *ext = extdata();
	if (ext) {
		DataLengthType extlen = ext->datalen();
		BINARY_SET_DATA_S_INCR(p, DataLengthType, extlen);
		char *extdata = ext->dataBinary();
		memcpy(p, extdata, extlen);
		p += extlen;
	}
	n = p - m_buf;
	assert(n < m_bufsize);

	BINARY_SET_DATA_S(m_buf, DataLengthType, n);
	return m_buf;
}

char *CylinderPartsCmpnt::dataBinary()
{
	char *p = m_buf;
	BINARY_SET_DOUBLE_INCR(p, radius());
	BINARY_SET_DOUBLE_INCR(p, length());
	return m_buf;
}

char *BoxPartsCmpnt::dataBinary()
{
	char *p = m_buf;

	// modified by sekikawa (2007/11/30)
	// I don't know why but in Win32 platform, original code (below) doesn't work as expected.
	// (size values cannot be copied to buf (all zero))

	// on Linux also (2009/01/27)
	double szx = m_sz.x();
	double szy = m_sz.y();
	double szz = m_sz.z();
	BINARY_SET_DOUBLE_INCR(p, szx);
	BINARY_SET_DOUBLE_INCR(p, szy);
	BINARY_SET_DOUBLE_INCR(p, szz);

	return m_buf;
}

char *SpherePartsCmpnt::dataBinary()
{
	return reinterpret_cast<char*>(&m_radius);
}

#define ARRAY_SIZE(ARY) (int) (sizeof(ARY)/sizeof(ARY[0]))

#ifdef SIGVERSE_OGRE_CLIENT
CParts *CParts::decode(char *data, int aid)
#else
#ifdef IRWAS_OGRE_CLIENT
CParts *CParts::decode(char *data, int aid)
#else
CParts *CParts::decode(char *data)
#endif	// IRWAS_OGRE_CLIENT
#endif	// SIGVERSE_OGRE_CLIENT
{
	char *p = data;
	Id id = BINARY_GET_DATA_L_INCR(p, Id);
	PartsType type = BINARY_GET_DATA_S_INCR(p, PartsType);
	char *name =  BINARY_GET_STRING_INCR(p);
	dReal pos[3];
	for (int i=0; i<ARRAY_SIZE(pos); i++) {
		pos[i] = BINARY_GET_DOUBLE_INCR(p);
	}

	RotationType rottype = BINARY_GET_DATA_S_INCR(p, RotationType);
	assert(rottype == ROTATION_TYPE_QUATERNION);
	dReal q[4];
	for (int i=0; i<ARRAY_SIZE(q); i++) {
		q[i] = BINARY_GET_DOUBLE_INCR(p);
	}

	DataLengthType extlen = BINARY_GET_DATA_S_INCR(p, DataLengthType);

	CParts *parts = 0;
	switch(type) {
	case PARTS_TYPE_BOX:
		{
			assert(extlen == BoxPartsCmpnt::DATA_LEN);
			dReal sz[3];
			for (int i=0; i<ARRAY_SIZE(sz); i++) {
				sz[i] = BINARY_GET_DOUBLE_INCR(p);
			}
			parts = new BoxParts(name,
					     Position(pos[0], pos[1], pos[2]),
					     Size(sz[0], sz[1], sz[2]));
		}
		break;
	case PARTS_TYPE_CYLINDER:
		{
			assert(extlen = CylinderPartsCmpnt::DATA_LEN);
			dReal radius = BINARY_GET_DOUBLE_INCR(p);
			dReal length = BINARY_GET_DOUBLE_INCR(p);
			parts = new CylinderParts(name,
						  Position(pos[0], pos[1], pos[2]),
						  radius, length);
		}
		break;
	case PARTS_TYPE_SPHERE:
		{
			assert(extlen = SpherePartsCmpnt::DATA_LEN);
			dReal rad = BINARY_GET_DOUBLE_INCR(p);
			parts = new SphereParts(name,
						Position(pos[0], pos[1], pos[2]),
						rad);
			break;
		}
	default:
		break;
	}

	assert(parts);
	parts->setId(id);
	parts->setQuaternion(q[0], q[1], q[2], q[3]);

	BINARY_FREE_STRING(name);

	return parts;
}


