/*
 * Modified by MSI on 2011-05-12
 */

#include "X3DFileReader.h"
#include "SSimObjBuilder.h"
#include "SgvX3DSimObjCreator.h"
#include "CJNIUtil.h"
#include <stdlib.h>
#include <string.h>

#if 1
// FIX20110421(ExpSS)
X3DFileReader::X3DFileReader(SSimObj &obj, ODEWorld &w, SimpleShapeDB &ssdb)
	: m_obj(obj), m_w(w), m_ssdb(ssdb)
{
}

bool X3DFileReader::read(
	const char *x3dFileName,
	const char *entClassName,
	bool bGenSimpleShapeFromX3D)
{
	bool b;

	char *cfg = getenv("SIGVERSE_X3DPARSER_CONFIG");

	if (cfg == NULL || strlen(cfg) == 0)
	{
		fprintf(stderr, "read ./X3DParser.cfg\n");
		b = CJNIUtil::init("X3DParser.cfg");
	}
	else
	{
		fprintf(stderr, "read %s\n", cfg);
		b = CJNIUtil::init(cfg);
	}

	if (!b)
	{
		fprintf(stderr, "cannot read x3d config file : \"%s\"", x3dFileName);
		exit(1);
	}

	SSimObjBuilder builder(m_obj, m_w);
	Sgv::X3DSimObjCreator creator(builder, m_ssdb);
	bool status = creator.createSSimObjFromX3D(x3dFileName, entClassName, bGenSimpleShapeFromX3D);
	return status;
}
#else
// orig
bool X3DFileReader::read(const char *fname)
{
	char *cfg = getenv("SIGVERSE_X3DPARSER_CONFIG");

	bool b;
	if (cfg == NULL || strlen(cfg) == 0) {
		fprintf(stderr, "read ./X3DParser.cfg\n");
		b = CJNIUtil::init("X3DParser.cfg");
	} else {
		fprintf(stderr, "read %s\n", cfg);
		b = CJNIUtil::init(cfg);
	}

	if (!b) {
		fprintf(stderr, "cannot read x3d config file : \"%s\"", fname);
		exit(1);
	}

	SSimObjBuilder builder(m_obj, m_w);
	Sgv::X3DSimObjCreator creator(builder);
	return Sgv::X3DSimObjCreator::test1(creator, fname);
}
#endif
