/*
 * Modified by MSI on 2011-05-19
 */

#include "systemdef.h"

#if (defined EXEC_SIMULATION && defined USE_XERCES)

#include "EntityXMLReader.h"
#include "BodyXMLReader.h"
#include "SimpleShapeXMLReader.h"
#include "SSimObj.h"
#include "Value.h"
#include "Attribute.h"
#include "ODEWorld.h"
#include "X3DFileReader.h"
#include "ReadTask.h"
#include "Logger.h"
#include "FilenameDB.h"
#include "SimpleShapeDB.h"
#include "XMLUtils.h"

#include <string>
#include <iostream>

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/SAXParser.hpp>

using xercesc::SAXParser;
using xercesc::XMLString;

#if 1
#define DUMP(MSG)
#else
#define DUMP(MSG) printf MSG
#endif

#define NS EntityXMLReaderNS
#define START_NS(NAME) namespace NAME {
#define END_NS() }

#define GET_VALUE(ATTRS, STR) XMLString::transcode(ATTRS.getValue(STR))
#define RELEASE(STR) if (STR) { XMLString::release(&STR); }

#define FREE(P) if (P) { delete P; P = 0; }


static std::string filename(std::string dirname, std::string fname)
{
	std::string f = "";
	if (dirname.length() > 0) {
		f = dirname + "/";
	}
	f += fname;
	return f;
}

static std::string createKey(const char *agentName, const char *partsName,
				 const char *stateName, const char *stateValue)
{
	std::string key = agentName;
	key += ":";
	if (partsName) {
		key += partsName;
	}
	key += ":";
	if (stateName) {
		key += stateName;
	}
	key += ":";
	if (stateValue) {
		key += stateValue;
	}

	return key;
}

inline bool ZERO_STRING(std::string s)
{
	return s.length() <= 0? true: false;
}

START_NS(NS);

struct X3D {
	typedef std::string S;
	S	partsName;
	S	stateName;
	S	stateValue;
	S	fileName;
};

// =========================================
//	X3DReadTask
// =========================================
#if 1
// FIX20110421(ExpSS)
class X3DReadTask : public ReadTask
{
public:
	X3DReadTask(
		X3DFileReader *r,
		const char *x3dFileName,
		const char *entClassName)
	: m_reader(r), m_bGenSimpleShapeFromX3D(false)
	{
		m_x3dFileName = x3dFileName ? x3dFileName : "";
		m_entClassName = entClassName ? entClassName : "";
	}

	const char *getKey()
	{
		return m_entClassName.c_str();
	}

	void enableGenSimpleShapeFromX3D(bool b)
	{
		m_bGenSimpleShapeFromX3D = b;
	}

private:
	X3DFileReader *m_reader;
	std::string m_x3dFileName;
	std::string m_entClassName;
	bool m_bGenSimpleShapeFromX3D;

	void execute()
	{
//		LOG_SYSTEM(("reading %s...", m_x3dFileName.c_str()));
		LOG_SYSTEM(("building entity (entName=(%s), className=(%s), X3DFileName=(%s))",
			m_reader->getSSimObj().name(),
			m_entClassName.c_str(),
			m_x3dFileName.c_str()));

		m_reader->read(m_x3dFileName.c_str(), m_entClassName.c_str(), m_bGenSimpleShapeFromX3D);

		delete m_reader; m_reader = NULL;
	}
};
#else
// orig

// =========================================
//	Handler
// =========================================

class X3DReadTask : public ReadTask
{
private:
	X3DFileReader *m_reader;
	std::string m_fname;
public:
	X3DReadTask(X3DFileReader *r, const char *fname)
		: m_reader(r), m_fname(fname) {}

private:
	void execute()
	{
		LOG_SYSTEM(("reading %s...", m_fname.c_str()));
		m_reader->read(m_fname.c_str());
		delete m_reader; m_reader = NULL;
	}
};
#endif

#define NOATTR_ERR(NODE, ATTR, ALIST) { this->failed++; LOG_ERR(("%s : <%s> : no \"%s\" attribute\n", this->m_fname.c_str(), NODE, ATTR)); XMLUtils::dumpAttributeList(ALIST);}

#define NOFILE_ERR(FNAME) { this->failed++; LOG_ERR(("no such file : \"%s\"", FNAME)); }

struct Handler : public xercesc::HandlerBase
{
	typedef xercesc::HandlerBase Super;
	typedef std::string S;

	S	m_fname;
	FilenameDB &m_fdb;
	SSimObj &m_obj;
	ODEWorld &m_w;
	ReadTaskContainer *m_tasks;
	int	m_depth;
	S	m_classname;

#if 1
// FIX20110421(ExpSS)
	X3DDB &	m_x3ddb;
	bool m_bInsideFileNameTag;
	S m_x3dFileName;
	bool m_bGenSimpleShapeFromX3D;
	bool m_bFirstX3DTag;

	// for simpleShape/simpleShapeFile tag
	SimpleShapeDB& m_ssdb;
	bool m_bInsideSimpleShapeTag;
	bool m_bInsideSimpleShapeFileTag;
	S m_simpleShapeFileName;
	SimpleShapeElem::SS_TYPE m_type;
	double m_x, m_y, m_z;
	double m_sx, m_sy, m_sz;
	double m_r;
	double m_h;
#else
// orig
	// for x3d tag
	X3DDB &	m_db;
	bool	m_tagFilename;
#endif
	X3D *	m_x3d;

	int	failed;

#if 1
// FIX20110421(ExpSS)
	Handler(FilenameDB &fdb, SSimObj &obj, ODEWorld &w, X3DDB &x3ddb, SimpleShapeDB &ssdb) :
		Super(),
		m_fdb(fdb),
		m_ssdb(ssdb),
		m_obj(obj), m_tasks(NULL),
		m_w(w) ,
		m_depth(0), m_x3ddb(x3ddb),
		m_x3d(NULL),
		m_bInsideFileNameTag(false),
		m_bInsideSimpleShapeTag(false),
		m_bInsideSimpleShapeFileTag(false),
		failed(0),
		m_bGenSimpleShapeFromX3D(true) {}

	void initFirstX3DTagFlag()
	{
		m_bFirstX3DTag = true;
	}
#else
// orig
	Handler(FilenameDB &fdb, SSimObj &obj, ODEWorld &w, X3DDB &db)
		: Super(),
		  m_fdb(fdb), m_obj(obj), m_tasks(NULL),
		   m_w(w) ,
		  m_depth(0), m_db(db), m_x3d(NULL), m_tagFilename(false), failed(0) {}
#endif

	std::string setFilename(const char *fname) {
		S tmp = m_fname;
		m_fname = fname;
		return tmp;
	}

	void setReadTaskContainer(ReadTaskContainer *tc)
	{
		m_tasks = tc;
	}

#if 1
// FIX20110421(ExpSS)
	void characters(const XMLCh *const chars, const unsigned int length)
	{
		char *text = XMLString::transcode(chars);

		//printf("length : %d\n", length);

		if (m_bInsideFileNameTag && m_x3d && text)
		{
			m_x3d->fileName = text;
			m_x3dFileName = text;
			// printf("filename : %s\n", text);
		}
		else if (m_bInsideSimpleShapeFileTag && text)
		{
			m_simpleShapeFileName = text;
		}

		RELEASE(text);
	}
#else
// orig
	void characters(const XMLCh *const chars, const unsigned int length)
	{
		char *text = XMLString::transcode(chars);
		//printf("length : %d\n", length);
		if (m_tagFilename && m_x3d && text) {
			m_x3d->fileName = text;
			//printf("filename : %s\n", text);
		}
		RELEASE(text);
	}
#endif

#if 1	// {
// begin(FIX20110421(ExpSS))
	// -----------------------------------------------
	// -----------------------------------------------
	void startElement(const XMLCh * const name, xercesc::AttributeList &attrs)
	{
		char *tagName = XMLString::transcode(name);

		//printf("start element(%s)\n", tagName);
		// XMLUtils::dumpAttributeList(attrs);

		if (strcmp(tagName, "define-class") == 0)
		{
			start_DefineClass(attrs);
		}
		else if (strcmp(tagName, "attr") == 0)
		{
			start_Attr(attrs);
		}
		else if (strcmp(tagName, "set-attr-value") == 0)
		{
			start_SetAttrValue(attrs);
		}
		else if (strcmp(tagName, "body") == 0)
		{
#if 1
			// sekikawa 20110328
			start_Body(attrs);
#endif
		}
		else if (strcmp(tagName, "x3d") == 0)
		{
			start_X3D(attrs);
		}
		else if (strcmp(tagName, "filename") == 0)
		{
			m_bInsideFileNameTag = true;
		}
		else if (strcmp(tagName, "state") == 0)
		{
			start_State(attrs);
		}
		else if (strcmp(tagName, "simpleShape") == 0)
		{
			m_bInsideSimpleShapeTag = true;
			start_SimpleShape(attrs);
		}
		else if (strcmp(tagName, "position") == 0)
		{
			start_Position(attrs);
		}
		else if (strcmp(tagName, "size") == 0)
		{
			start_Size(attrs);
		}
		else if (strcmp(tagName, "simpleShapeFile") == 0)
		{
			m_bInsideSimpleShapeFileTag = true;
			start_SimpleShapeFile(attrs);
		}

		RELEASE(tagName);
	}

	void start_DefineClass(xercesc::AttributeList &attrs)
	{
		char *xmlfile = GET_VALUE(attrs, "inherit");
		if (xmlfile)
		{
			char buf[4096];
			const char *f = m_fdb.getPath(xmlfile, buf);
			if (f != NULL)
			{
				LOG_DEBUG1(("reading %s ...", f));
				m_depth++;

				SAXParser *parser = new SAXParser();
				std::string lastfname = setFilename(f);
				parser->setDocumentHandler(this);

				parser->parse(f);
				delete parser;

				RELEASE(xmlfile);
				setFilename(lastfname.c_str());
				m_depth--;
			}
			else
			{
				NOFILE_ERR(xmlfile);
			}
		}

		char *name = GET_VALUE(attrs, "name");
		if (name != NULL)
		{
			m_classname = name;
		}
		else
		{
			NOATTR_ERR("define-class", "name", attrs);
		}

		RELEASE(name);
	}

	void start_Attr(xercesc::AttributeList &attrs)
	{
		char *n = GET_VALUE(attrs, "name");
		char *g = GET_VALUE(attrs, "group");
		char *t = GET_VALUE(attrs, "type");
		char *v = GET_VALUE(attrs, "value");
		//			std::cout << "attr : " <<n << std::endl;

		if (n == NULL)
		{
			NOATTR_ERR("attr", "name", attrs);
		}

		if (g == NULL)
		{
			NOATTR_ERR("attr", "group", attrs);
		}

		if (t == NULL)
		{
			NOATTR_ERR("attr", "type", attrs);
		}

		if (n != NULL && g != NULL && t != NULL)
		{
			Value *value = 0;

			if (strcmp(t, "double") == 0)
			{
				value = new DoubleValue();
			}
			else if (strcmp(t, "string") == 0)
			{
				value = new StringValue();
			}
			else if (strcmp(t, "bool") == 0)
			{
				value = new BoolValue();
			}

			if (v) { value->setString(v); }
			m_obj.push(new Attribute(n, value, g));
		}

		RELEASE(n);
		RELEASE(g);
		RELEASE(t);
		RELEASE(v);
	}

	void start_SetAttrValue(xercesc::AttributeList &attrs)
	{
		char *n = GET_VALUE(attrs, "name");
		char *v = GET_VALUE(attrs, "value");

		if (n == NULL)
		{
			NOATTR_ERR("set-attr-value", "name", attrs);
		}

		if (v == NULL)
		{
			NOATTR_ERR("set-attr-value", "value", attrs);
		}

		if (n != NULL && v != NULL)
		{
			m_obj.setAttrValue(n, v);
		}

		RELEASE(n);
		RELEASE(v);
	}

	void start_Body(xercesc::AttributeList &attrs)
	{
		char *fname = GET_VALUE(attrs, "filename");
		if (fname)
		{
			BodyXMLReader read(m_obj, m_w);
			//S f = filename(m_xmldir, fname);

			char buf[4096];
			const char *f = m_fdb.getPath(fname, buf);
			if (f != NULL)
			{
				read(f);
			}
			else
			{
				NOFILE_ERR(fname);
			}
		}
		else
		{
			NOATTR_ERR("body", "filename", attrs);
		}
	}

	void start_X3D(xercesc::AttributeList &attrs)
	{
		assert(m_x3d == NULL);

		m_x3d = new X3D();

		char *partsName = GET_VALUE(attrs, "parts");
		if (partsName)
		{
			m_x3d->partsName = partsName;
		}
		RELEASE(partsName);
	}

	void start_State(xercesc::AttributeList &attrs)
	{
		if (m_x3d)
		{
			char *n = GET_VALUE(attrs, "name");
			char *v = GET_VALUE(attrs, "value");

			if (n == NULL)
			{
				NOATTR_ERR("state", "name", attrs);
			}
			if (v == NULL)
			{
				NOATTR_ERR("state", "value", attrs);
			}

			if (n && v)
			{
				m_x3d->stateName = n;
				m_x3d->stateValue = v;
			}

			RELEASE(n);
			RELEASE(v);
		}
	}

	void start_SimpleShape(xercesc::AttributeList &attrs)
	{
		m_bGenSimpleShapeFromX3D = false;

		char *t = GET_VALUE(attrs, "type");
		if (t)
		{
			if (strcmp(t, "box") == 0)
			{
				m_type = SimpleShapeElem::SS_TYPE_BOX;
			}
			else if (strcmp(t, "cylinder") == 0)
			{
				m_type = SimpleShapeElem::SS_TYPE_CYLINDER;
			}
			else if (strcmp(t, "sphere") == 0)
			{
				m_type = SimpleShapeElem::SS_TYPE_SPHERE;
			}
			else
			{
				m_type = SimpleShapeElem::SS_TYPE_UNKNOWN;
			}

			LOG_DEBUG1((">> type=(%s)\n", t));
		}

		RELEASE(t);
	}

	void start_Position(xercesc::AttributeList &attrs)
	{
		char *px = NULL;
		char *py = NULL;
		char *pz = NULL;

		px = GET_VALUE(attrs, "x");
		if (px)
		{
			m_x = atof(px);

			py = GET_VALUE(attrs, "y");
			if (py)
			{
				m_y = atof(py);

				pz = GET_VALUE(attrs, "z");
				if (pz)
				{
					m_z = atof(pz);

					LOG_DEBUG1((">> position(x=(%f), y=(%f), z=(%f))\n", m_x, m_y, m_z));
				}
				else
				{
					NOATTR_ERR("position", "z", attrs);
				}
			}
			else
			{
				NOATTR_ERR("position", "y", attrs);
			}
		}
		else
		{
			NOATTR_ERR("position", "x", attrs);
		}

		RELEASE(px);
		RELEASE(py);
		RELEASE(pz);
	}

	void start_Size(xercesc::AttributeList &attrs)
	{
		switch (m_type)
		{
			case SimpleShapeElem::SS_TYPE_BOX:
			{
				char *psx = NULL;
				char *psy = NULL;
				char *psz = NULL;

				psx = GET_VALUE(attrs, "sx");
				if (psx)
				{
					m_sx = atof(psx);

					psy = GET_VALUE(attrs, "sy");
					if (psy)
					{
						m_sy = atof(psy);

						psz = GET_VALUE(attrs, "sz");
						if (psz)
						{
							m_sz = atof(psz);

							LOG_DEBUG1((">> size (sx, sy, sz)=(%f, %f, %f)\n", m_sx, m_sy, m_sz));
						}
						else
						{
							NOATTR_ERR("size", "sz", attrs);
						}
					}
					else
					{
						NOATTR_ERR("size", "sy", attrs);
					}
				}
				else
				{
					NOATTR_ERR("size", "sx", attrs);
				}

				RELEASE(psx);
				RELEASE(psy);
				RELEASE(psz);
			}
			break;

			case SimpleShapeElem::SS_TYPE_CYLINDER:
			{
				char *pr = NULL;
				char *ph = NULL;

				pr = GET_VALUE(attrs, "r");
				if (pr)
				{
					m_r = atof(pr);

					ph = GET_VALUE(attrs, "h");
					if (ph)
					{
						m_h = atof(ph);

						LOG_DEBUG1((">> size (r, h)=(%f, %f)\n", m_r, m_h));
					}
					else
					{
						NOATTR_ERR("size", "h", attrs);
					}
				}
				else
				{
					NOATTR_ERR("size", "r", attrs);
				}

				RELEASE(ph);
				RELEASE(pr);
			}
			break;

			case SimpleShapeElem::SS_TYPE_SPHERE:
			{
				char *pr = GET_VALUE(attrs, "r");
				if (pr)
				{
					m_r = atof(pr);

					LOG_DEBUG1((">> size (r=%f)\n", m_r));
				}
				else
				{
					NOATTR_ERR("size", "r", attrs);
				}

				RELEASE(pr);
			}
			break;

			default:
			break;
		}
	}

	void start_SimpleShapeFile(xercesc::AttributeList &attrs)
	{
		m_bGenSimpleShapeFromX3D = false;
	}

	// -----------------------------------------------
	// -----------------------------------------------
	void endElement(const XMLCh * const name)
	{
		char *tagName = XMLString::transcode(name);
		//	printf("end element(%s)\n", tagName);

		if (strcmp(tagName, "define-class") == 0)
		{
			end_DefineClass();
		}
		else if (strcmp(tagName, "filename") == 0)
		{
			m_bInsideFileNameTag = false;
		}
		else if (strcmp(tagName, "x3d") == 0)
		{
			end_X3D();
		}
		else if (strcmp(tagName, "simpleShape") == 0)
		{
			end_SimpleShape();
			m_bInsideSimpleShapeTag = false;
		}
		else if (strcmp(tagName, "simpleShapeFile") == 0)
		{
			end_SimpleShapeFile();
			m_bInsideSimpleShapeFileTag = false;
		}

		RELEASE(tagName);
	}

	void end_DefineClass()
	{
		if (m_depth == 0)
		{
			m_obj.setAttrValue("class", m_classname.c_str());
		}
		else if (m_depth == 1)
		{
			m_obj.setAttrValue("superclass", m_classname.c_str());
		}

		std::vector<ReadTask *>& taskCol = m_tasks->getTaskCol(m_classname.c_str());
		std::vector<ReadTask *>::iterator i;
		for (i=taskCol.begin(); i!=taskCol.end(); i++)
		{
			X3DReadTask *t = (X3DReadTask *)(*i);
			if (t)
			{
// sekikawa
//printf("m_bGenSimpleShape=(%s)\n", m_bGenSimpleShapeFromX3D ? "true" : "false");
				t->enableGenSimpleShapeFromX3D(m_bGenSimpleShapeFromX3D);
			}
		}
	}

	void end_X3D()
	{
DUMP(("end_X3D\n"));
		if (m_x3d)
		{
			if (m_x3d->fileName.length() > 0)
			{
DUMP(("m_x3d->fileName=(%s)\n", m_x3d->fileName.c_str()));
 
                           std::string key = createKey(
				        m_classname.c_str(),
			                m_x3d->partsName.c_str(),
					m_x3d->stateName.c_str(),
					m_x3d->stateValue.c_str());
				// printf("key : %s\n", key.c_str());

				m_x3ddb.set(key.c_str(), m_x3d->fileName.c_str());

DUMP(("m_x3d->partsName=(%s), m_x3d->stateName=(%s)\n", m_x3d->partsName.c_str(), m_x3d->stateName.c_str()));
DUMP(("m_bFirstX3DTag=(%s)\n", m_bFirstX3DTag ? "true" : "false"));

				if ((ZERO_STRING(m_x3d->partsName) && ZERO_STRING(m_x3d->stateName)) || m_bFirstX3DTag)
				{
					assert(m_tasks);

					//S f = filename(m_xmldir, m_x3d->fileName);
					char buf[1024];
					const char *fname = m_x3d->fileName.c_str();
					const char *fpath = m_fdb.getPath(fname, buf);
					if (fpath != NULL)
					{
						X3DFileReader *r = new X3DFileReader(m_obj, m_w, m_ssdb);
						X3DReadTask *t = new X3DReadTask(r, fpath, m_classname.c_str());

DUMP(("push task : className=(%s)\n", m_classname.c_str()));

						m_tasks->pushTask(t);
					}
					else
					{
						NOFILE_ERR(fname);
					}
				}
			}

			FREE(m_x3d);
		}

		m_bFirstX3DTag = false;
	}

	void end_SimpleShape()
	{
		switch (m_type)
		{
			case SimpleShapeElem::SS_TYPE_BOX:
			{
				SimpleShapeElem *s = new SimpleShapeElem(
					SimpleShapeElem::SS_TYPE_BOX);
				s->setPosition(m_x, m_y, m_z);
				s->setSize(m_sx, m_sy, m_sz);
				m_ssdb.set(m_classname.c_str(), "body", s);
			}
			break;

			case SimpleShapeElem::SS_TYPE_CYLINDER:
			{
				SimpleShapeElem *s = new SimpleShapeElem(
					SimpleShapeElem::SS_TYPE_CYLINDER);
				s->setPosition(m_x, m_y, m_z);
				s->setRadius(m_r);
				s->setHeight(m_h);
				m_ssdb.set(m_classname.c_str(), "body", s);
			}
			break;

			case SimpleShapeElem::SS_TYPE_SPHERE:
			{
				SimpleShapeElem *s = new SimpleShapeElem(
					SimpleShapeElem::SS_TYPE_SPHERE);
				s->setPosition(m_x, m_y, m_z);
				s->setRadius(m_r);
				m_ssdb.set(m_classname.c_str(), "body", s);
			}
			break;

			default:
			break;
		}
	}

	void end_SimpleShapeFile()
	{
		SimpleShapeXMLReader r(m_fdb, m_ssdb);

		r.read(m_classname.c_str(), m_simpleShapeFileName.c_str());
	}
// end(FIX20110421(ExpSS))
#else	// } else {
// begin(orig)
	void startElement(const XMLCh * const name, xercesc::AttributeList &attrs)
	{
		char *tagName = XMLString::transcode(name);

			//printf("start element(%s)\n", tagName);
		// XMLUtils::dumpAttributeList(attrs);

		if (strcmp(tagName, "define-class") == 0) {
			char *xmlfile = GET_VALUE(attrs, "inherit");
			if (xmlfile) {
				char buf[4096];
				const char *f = m_fdb.getPath(xmlfile, buf);
				if (f != NULL) {
					LOG_DEBUG1(("reading %s ...", f));
					m_depth++;
					SAXParser *parser = new SAXParser();
					std::string lastfname = setFilename(f);
					parser->setDocumentHandler(this);
					parser->parse(f);
					delete parser;
					RELEASE(xmlfile);
					setFilename(lastfname.c_str());
					m_depth--;
				} else {
					NOFILE_ERR(xmlfile);
				}
			}

			char *name = GET_VALUE(attrs, "name");
			if (name != NULL) {
				m_classname = name;
			} else {
				NOATTR_ERR("define-class", "name", attrs);
			}

			RELEASE(name);

		} else if (strcmp(tagName, "attr") == 0) {

			char *n = GET_VALUE(attrs, "name");
			char *g = GET_VALUE(attrs, "group");
			char *t = GET_VALUE(attrs, "type");
			char *v = GET_VALUE(attrs, "value");
			//			std::cout << "attr : " <<n << std::endl;

			if (n == NULL) {
				NOATTR_ERR("attr", "name", attrs);
			}

			if (g == NULL) {
				NOATTR_ERR("attr", "group", attrs);
			}

			if (t == NULL) {
				NOATTR_ERR("attr", "type", attrs);
			}

			if (n != NULL && g != NULL && t != NULL) {
				Value *value = 0;
				if (strcmp(t, "double") == 0) {
					value = new DoubleValue();
				} else if (strcmp(t, "string") == 0) {
					value = new StringValue();
				} else if (strcmp(t, "bool") == 0) {
					value = new BoolValue();
				}
				if (v) { value->setString(v); }

				m_obj.push(new Attribute(n, value, g));
			}
			RELEASE(n);
			RELEASE(g);
			RELEASE(t);
			RELEASE(v);
		} else if (strcmp(tagName, "set-attr-value") == 0) {
			char *n = GET_VALUE(attrs, "name");
			char *v = GET_VALUE(attrs, "value");

			if (n == NULL) {
				NOATTR_ERR("set-attr-value", "name", attrs);
			}

			if (v == NULL) {
				NOATTR_ERR("set-attr-value", "value", attrs);
			}

			if (n != NULL && v != NULL) {
				m_obj.setAttrValue(n, v);
			}
			RELEASE(n);
			RELEASE(v);
		} else if (strcmp(tagName, "body") == 0) {
			char *fname = GET_VALUE(attrs, "filename");
			if (fname) {
				BodyXMLReader read(m_obj, m_w);
				//S f = filename(m_xmldir, fname);
				char buf[4096];
				const char *f = m_fdb.getPath(fname, buf);
				if (f != NULL) {
					read(f);
				} else {
					NOFILE_ERR(fname);
				}
			} else {
				NOATTR_ERR("body", "filename", attrs);
			}
		} else if (strcmp(tagName, "x3d") == 0) {
			assert(m_x3d == NULL);
			m_x3d = new X3D();
			char *partsName = GET_VALUE(attrs, "parts");
			if (partsName) {
				m_x3d->partsName = partsName;
			}
			RELEASE(partsName);
		} else if (strcmp(tagName, "filename") == 0) {
			m_tagFilename = true;

		} else if (strcmp(tagName, "state") == 0) {
			if (m_x3d) {
				char *n = GET_VALUE(attrs, "name");
				char *v = GET_VALUE(attrs, "value");
				if (n == NULL) {
					NOATTR_ERR("state", "name", attrs);
				}
				if (v == NULL) {
					NOATTR_ERR("state", "value", attrs);
				}
				if (n && v) {
					m_x3d->stateName = n;
					m_x3d->stateValue = v;
				}
				RELEASE(n);
				RELEASE(v);
			}
		}
		RELEASE(tagName);
	}

	void endElement(const XMLCh * const name)
	{
		char *tagName = XMLString::transcode(name);
		//		printf("end element(%s)\n", tagName);
		if (strcmp(tagName, "define-class") == 0) {
			if (m_depth == 0) {
				m_obj.setAttrValue("class", m_classname.c_str());
			} else if (m_depth == 1) {
				m_obj.setAttrValue("superclass", m_classname.c_str());
			}
		} else if (strcmp(tagName, "filename") == 0) {
			m_tagFilename = false;
		} else if (strcmp(tagName, "x3d") == 0) {
			if (m_x3d) {
				if (m_x3d->fileName.length() > 0) {
					std::string key = createKey(m_classname.c_str(),
									m_x3d->partsName.c_str(),
									m_x3d->stateName.c_str(),
									m_x3d->stateValue.c_str());
					// printf("key : %s\n", key.c_str());
					m_db.set(key.c_str(), m_x3d->fileName.c_str());
					if (ZERO_STRING(m_x3d->partsName) &&
						ZERO_STRING(m_x3d->stateName)) {
						assert(m_tasks);

						//S f = filename(m_xmldir, m_x3d->fileName);
						char buf[1024];
						const char * fname = m_x3d->fileName.c_str();
						const char *fpath = m_fdb.getPath(fname, buf);
						if (fpath != NULL) {
							X3DFileReader *r = new X3DFileReader(m_obj, m_w);
							X3DReadTask * t = new X3DReadTask(r, fpath);
							m_tasks->pushTask(t);
						} else {
							NOFILE_ERR(fname);
						}
					}
				}
				FREE(m_x3d);
			}
		}
		RELEASE(tagName);
	}
// end(orig)
#endif // }
};


END_NS();

#if 1	// {
// begin(FIX20110421(ExpSS))
// =========================================
//	EntityXMLReader
// =========================================

EntityXMLReader::EntityXMLReader(FilenameDB &fdb, SSimObj &obj, ODEWorld &w, X3DDB &x3ddb, SimpleShapeDB &ssdb) : m_fdb(fdb), m_h(0)
{
	typedef NS::Handler H;

	m_h = new H(fdb, obj, w, x3ddb, ssdb);
}

void EntityXMLReader::setReadTaskContainer(ReadTaskContainer *tc)
{
	assert(m_h);

	// 090312 dangerous fix later
	NS::Handler *h = (NS::Handler *)m_h;
	h->setReadTaskContainer(tc);
}

EntityXMLReader::~EntityXMLReader()
{
	delete m_h;
}

bool EntityXMLReader::read(const char *fname)
{
	SAXParser *parser = new SAXParser();

	m_h->setFilename(fname);
	m_h->initFirstX3DTagFlag();
	parser->setDocumentHandler(m_h);

	char buf[4096];
	const char *fpath = m_fdb.getPath(fname, buf);
	if (fpath != NULL)
	{
		LOG_DEBUG1(("reading %s...", fpath));

		parser->parse(fpath);

		delete parser;
		return (!m_h->failed) ? true : false;
	}
	else
	{
		LOG_ERR(("no such file : \"%s\"", fname));
		delete parser;	// added by sekikawa (2011/3/3)
		return false;
	}
}

// end(FIX20110421(ExpSS))
#else	// } else {
// begin(orig)

EntityXMLReader::EntityXMLReader(FilenameDB &fdb, SSimObj &obj, ODEWorld &w, X3DDB &db) : m_fdb(fdb), m_h(0)
{
	typedef NS::Handler H;
	m_h = new H(fdb, obj, w, db);
}

void EntityXMLReader::setReadTaskContainer(ReadTaskContainer *tc)
{
	assert(m_h);
	// 090312 dangerous fix later
	NS::Handler *h = (NS::Handler*)m_h;
	h->setReadTaskContainer(tc);
}

EntityXMLReader::~EntityXMLReader()
{
	delete m_h;
}

bool EntityXMLReader::read(const char *fname)
{
	SAXParser *parser = new SAXParser();
	m_h->setFilename(fname);
	parser->setDocumentHandler(m_h);
	char buf[4096];
	const char *fpath = m_fdb.getPath(fname, buf);
	if (fpath != NULL) {
		LOG_DEBUG1(("reading %s...", fpath));
		parser->parse(fpath);
		delete parser;
		return !m_h->failed? true: false;
	} else {
		LOG_ERR(("no such file : \"%s\"", fname));
		return false;
	}
}
// end(orig)
#endif	// }

#ifdef EntityXMLReader_test

#include <xercesc/util/PlatformUtils.hpp>

int main(int argc, char **argv)
{
	dInitODE();
	xercesc::XMLPlatformUtils::Initialize();

	//
	Parts::initCounter();
	SSimObj::initCounter();

	char *fname = "Desk.xml";
	if (argc == 2) {
		fname = argv[1];
	}
	SSimObj *obj = new SSimObj();
	ODEWorld *w = ODEWorld::create(ODEWorld::Gravity(0, -9.8, 0), 0);
	EntityXMLReader read("../conf", *obj, *w);
	read(fname);
	obj->dump();
	return 0;
}

#endif

#endif // USE_XERCES && EXEC_SIMULATION


