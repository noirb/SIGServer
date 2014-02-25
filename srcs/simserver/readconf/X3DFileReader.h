/*
 * Modified by MSI on 2011-05-19
 */

#ifndef X3DFileReader_h
#define X3DFileReader_h

class SSimObj;
class ODEWorld;
class SimpleShapeDB;	// FIX20110421(ExpSS)

class X3DFileReader
{
private:
	SSimObj &m_obj;
	ODEWorld &m_w;
	SimpleShapeDB& m_ssdb;	// FIX20110421(ExpSS)

public:
#if 1
// FIX20110421(ExpSS)
	X3DFileReader(SSimObj &obj, ODEWorld &w, SimpleShapeDB &ssdb);

	SSimObj& getSSimObj() { return m_obj; }

	bool read(const char *x3dFileName, const char *entClassName, bool bGenSimpleShapeFromX3D);
#else
// orig
		X3DFileReader(SSimObj &obj, ODEWorld &w) : m_obj(obj), m_w(w) { ; }

	bool	read(const char *fname);
	bool	operator()(const char *fname)
	{
		return read(fname);
	}
#endif
};

#endif // X3DFileReader_h


