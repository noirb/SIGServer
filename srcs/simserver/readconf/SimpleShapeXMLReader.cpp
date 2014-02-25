/*
 * Modified by MSI on 2011-05-12
 */

#include "systemdef.h"

#if (defined USE_XERCES && defined EXEC_SIMULATION)

#include "SimpleShapeXMLReader.h"
#include "SimpleShapeDB.h"
#include "FilenameDB.h"
#include "Logger.h"
#include "XMLUtils.h"
#include <stdio.h>

#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

XERCES_CPP_NAMESPACE_USE;

#define NS WorldXMLReaderNS
#define START_NS(NAME) namespace NAME {
#define END_NS() }

#define GET_VALUE(ATTRS, STR) XMLString::transcode(ATTRS.getValue(STR))
#define RELEASE(STR) if (STR) { XMLString::release(&STR); }

#define NOATTR_ERR(NODE, ATTR, ALIST) { m_failed++; LOG_ERR(("%s : <%s> : no \"%s\" attribute\n", this->m_currfname.c_str(), NODE, ATTR)); XMLUtils::dumpAttributeList(ALIST); }

#define NOFILE_ERR(FNAME) { m_failed++; LOG_ERR(("no such file : %s", FNAME)); }

///////////////////////////////////////////////////////////////
//
//	SimpleShapeXMLReader

SimpleShapeXMLReader::SimpleShapeXMLReader(FilenameDB &fdb, SimpleShapeDB &ssdb) :
	m_fdb(fdb),
	m_ssdb(ssdb),
	m_failed(0),
	m_type(SimpleShapeElem::SS_TYPE_UNKNOWN)
{
}

SimpleShapeXMLReader::~SimpleShapeXMLReader()
{
}

void SimpleShapeXMLReader::startElement(const XMLCh *const tagName_, xercesc::AttributeList &attrs)
{
	char *tagName = XMLString::transcode(tagName_);

	if (strcmp(tagName, "parts") == 0)
	{
		char *n = NULL;
		char *t = NULL;

		n = GET_VALUE(attrs, "name");
		if (n)
		{
			m_partsName = n;

			t = GET_VALUE(attrs, "type");
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

//				printf(">> parts(name=(%s), type=(%s))\n", n, t);
			}
		}
		else
		{
			NOATTR_ERR("parts", "name", attrs);
		}

		RELEASE(n);
		RELEASE(t);
	}
	else if (strcmp(tagName, "position") == 0)
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

//					printf(">> position(x=(%f), y=(%f), z=(%f))\n", m_x, m_y, m_z);
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
	else if (strcmp(tagName, "size") == 0)
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

//							printf(">> size (sx, sy, sz)=(%f, %f, %f)\n", m_sx, m_sy, m_sz);
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

//						printf(">> size (r, h)=(%f, %f)\n", m_r, m_h);
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

//					printf(">> size (r=%f)\n", m_r);
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

	RELEASE(tagName);
}

void SimpleShapeXMLReader::endElement(const XMLCh *const tagName_)
{
	char *tagName = XMLString::transcode(tagName_);

	if (strcmp(tagName, "parts") == 0)
	{
//		printf("<< parts(m_type=%d)\n", m_type);

		switch (m_type)
		{
			case SimpleShapeElem::SS_TYPE_BOX:
			{
				SimpleShapeElem *s = new SimpleShapeElem(
					SimpleShapeElem::SS_TYPE_BOX);
				s->setPosition(m_x, m_y, m_z);
				s->setSize(m_sx, m_sy, m_sz);
				m_ssdb.set(m_entClassName.c_str(), m_partsName.c_str(), s);
			}
			break;

			case SimpleShapeElem::SS_TYPE_CYLINDER:
			{
				SimpleShapeElem *s = new SimpleShapeElem(
					SimpleShapeElem::SS_TYPE_CYLINDER);
				s->setPosition(m_x, m_y, m_z);
				s->setRadius(m_r);
				s->setHeight(m_h);
				m_ssdb.set(m_entClassName.c_str(), m_partsName.c_str(), s);
			}
			break;

			case SimpleShapeElem::SS_TYPE_SPHERE:
			{
				SimpleShapeElem *s = new SimpleShapeElem(
					SimpleShapeElem::SS_TYPE_SPHERE);
				s->setPosition(m_x, m_y, m_z);
				s->setRadius(m_r);
				m_ssdb.set(m_entClassName.c_str(), m_partsName.c_str(), s);
			}
			break;

			default:
			break;
		}
	}

	RELEASE(tagName);
}

bool SimpleShapeXMLReader::read(const char *entClassName, const char *simpleShapeFileName)
{
	XMLPlatformUtils::Initialize();

	SAXParser *parser = new SAXParser();
	parser->setDocumentHandler(this);

	m_entClassName = entClassName ? entClassName : "";

	bool ret = false;
	try
	{
		char buf[4096];
		const char *fpath = m_fdb.getPath(simpleShapeFileName, buf);
		if (fpath != NULL)
		{
			m_currfname = fpath;
			parser->parse(fpath);
		}
		else
		{
			NOFILE_ERR(simpleShapeFileName);
		}

		if (m_failed > 0)
		{
			goto err;
		}
	}
	catch(const XMLException &e)
	{
		char *msg = XMLString::transcode(e.getMessage());

		LOG_ERR(("parse error :%s", simpleShapeFileName));
		LOG_ERR(("%s", msg));

		XMLString::release(&msg);
		goto err;
	}

	ret = true;

 err:
	delete parser;

	return ret;
}

#endif // USE_XERCES && EXEC_SIMULATION

