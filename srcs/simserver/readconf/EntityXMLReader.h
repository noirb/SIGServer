/*
 * Modified by MSI on 2011-05-19
 */

#ifndef EntityXMLReader_h
#define EntityXMLReader_h


#include "systemdef.h"

#include <xercesc/sax/HandlerBase.hpp>

#if (defined USE_XERCES)

#include "X3DDB.h"
#include "SimpleShapeDB.h"	// FIX20110421(ExpSS)

class SSimObj;
class ODEWorld;
class ReadTaskContainer;
class FilenameDB;

namespace EntityXMLReaderNS {
	class Handler;
}

class EntityXMLReader
{
private:
	typedef std::string S;
private:
	FilenameDB & m_fdb;
	EntityXMLReaderNS::Handler *m_h;
public:
#if 1
// FIX20110421(ExpSS)
	EntityXMLReader(FilenameDB &fdb, SSimObj &obj, ODEWorld &w, X3DDB &db, SimpleShapeDB &ssdb);
#else
// orig
	EntityXMLReader(FilenameDB &fdb, SSimObj &obj, ODEWorld &w, X3DDB &db);
#endif

	~EntityXMLReader();

	void setReadTaskContainer(ReadTaskContainer *tc);

	bool read(const char *fname);
//	bool operator()(const char *fname) { return read(fname); }
};

#endif

#endif // EntityXMLReader_h
