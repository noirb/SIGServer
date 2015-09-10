/*
 * Modified by MSI on 2011-05-19
 */

#ifndef WorldXMLReader_h
#define WorldXMLReader_h

#include "systemdef.h"

#if (defined USE_XERCES)

#include <xercesc/sax/HandlerBase.hpp>

#include <string>
#include <vector>
#include <map>

#include "X3DDB.h"
#include "SimpleShapeDB.h"
#include "ReadTask.h"
#include "SimObjBase.h"
#include "SSimEntity.h"

class SSimWorld;
class SSimObj;
class FilenameDB;

class WorldXMLReader : public xercesc::HandlerBase, public ReadTaskContainer
{
#if 1
// FIX20110421(ExpSS)
	typedef std::map<std::string, std::vector<ReadTask *> > TaskC;

#else
// orig
	typedef std::vector<ReadTask *> TaskC;
#endif

private:
	FilenameDB    &m_fdb;
	X3DDB         &m_x3ddb;
	SimpleShapeDB &m_ssdb;	// FIX20110421(ExpSS)
	SSimWorld     *m_world;
	SSimObj       *m_currobj;
	SSimEntity    *m_current;
	TaskC  m_tasks;
	int    m_failed;
	bool   m_currcamera;
	bool   m_dUse;
	std::string m_currfname;
	std::vector<std::string> m_objNames;	// FIX20110421(ExpSS)

private:
	std::string setFilename(std::string fname)
	{
		std::string last = m_currfname;
		m_currfname = fname;
		return last;
	}
private:
	void startElement(const XMLCh * const tagName_, xercesc::AttributeList &attrs);
	void endElement(const XMLCh * const tagName_);

#if 1
// FIX20110421(ExpSS)
	void pushTask(ReadTask *t)
	{
		if (t)
		{
			const char *key = t->getKey();
			std::string strKey = key ? key : "";
			m_tasks[strKey].push_back(t);
		}
	}

	std::vector<ReadTask *>& getTaskCol(const char *key)
	{
		std::string strKey = key ? key : "";

		TaskC::iterator i=m_tasks.find(strKey);
		if (i != m_tasks.end())
		{
			return i->second;
		}
		else
		{
			static std::vector<ReadTask *> dummyEmptyResult;

			return dummyEmptyResult;
		}
	}

#else
	void pushTask(ReadTask *t)
	{
		m_tasks.push_back(t);
	}
#endif

public:

// FIX20110421(ExpSS)
//	WorldXMLReader(FilenameDB &fdb, X3DDB &x3ddb);
	WorldXMLReader(FilenameDB &fdb, X3DDB &x3ddb, SimpleShapeDB &ssdb);

	~WorldXMLReader();

	bool read(const char *fname);
//	bool operator()(const char *fname)
//	{
//		return read(fname);
//	}
	SSimWorld *release()
	{
		SSimWorld *tmp = m_world;
		m_world = 0;
		return tmp;
	}
};

#endif // WorldXMLReader_h

#endif // USE_XERCES
