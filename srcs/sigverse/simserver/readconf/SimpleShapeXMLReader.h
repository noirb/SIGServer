/*
 * Modified by MSI on 2011-05-12
 */

#ifndef _SIMPLE_SHAPE_XML_READER_H_
#define _SIMPLE_SHAPE_XML_READER_H_

#include <sigverse/commonlib/systemdef.h>

#if (defined USE_XERCES)

#include <sigverse/simserver/readconf/SimpleShapeDB.h>
#include <xercesc/sax/HandlerBase.hpp>
#include <string>
#include <map>

class FilenameDB;

class SimpleShapeXMLReader : public xercesc::HandlerBase
{
public:
	SimpleShapeXMLReader(FilenameDB &fdb, SimpleShapeDB &ssdb);
	~SimpleShapeXMLReader();

	bool read(const char *entClassName, const char *simpleShapeFileName);

private:
	FilenameDB& m_fdb;
	SimpleShapeDB& m_ssdb;
	int	m_failed;
	std::string m_entClassName;
	std::string m_currfname;
	SimpleShapeElem::SS_TYPE m_type;
	std::string m_partsName;
	double m_x, m_y, m_z;
	double m_sx, m_sy, m_sz;
	double m_r;
	double m_h;

	void startElement(const XMLCh *const tagName_, xercesc::AttributeList &attrs);
	void endElement(const XMLCh *const tagName_);
};

#endif	// USE_XERCES

#endif	// _SIMPLE_SHAPE_XML_READER_H_

