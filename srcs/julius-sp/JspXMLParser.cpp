#include "JspXMLParser.h"
#include "JspLog.h"
#include "xstr.h"
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// begin(add)(FIX20101029)
#include <xercesc/util/TransService.hpp>
#define TRANS_BUF_LEN	1024
// end(add)

namespace Jsp {

	/////////////////////////////////////////////////////////
	//
	//	MemoryText
	//
	MemoryText::MemoryText() : m_textBuf(NULL), m_textBufSize(0) {
		m_lines.clear();
	}

	MemoryText::~MemoryText() {
		_free();
	}

	void MemoryText::clear() {
		_free();
	}

	void MemoryText::addXMLBeginStatement() {
		m_lines.push_back("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	}

	void MemoryText::addLine(const char *line) {
#if 1
		m_lines.push_back(line);
#else
		std::string lineNL = line;
		lineNL = lineNL + "\n";

		m_lines.push_back(lineNL);
#endif
	}

	bool MemoryText::loadFromFile(const char *xmlFile) {
		std::string line;
		std::ifstream ifs(xmlFile);

		if (ifs.fail())
		{
			// file does not exist
			return false;
		}

		for (;;)
		{
			std::getline(ifs, line);

			if (ifs.eof())
			{
				if (line.size() > 0) m_lines.push_back(line);
				break;
			}

			m_lines.push_back(line);
		}

		ifs.close();

		return true;
	}

	void MemoryText::printText() {
		int nLines = m_lines.size();

		for (int i=0; i<nLines; i++) {
			std::string line = m_lines[i];

			Jsp::printLog(LOG_DEBUG, "[%3d] (%s)\n", i, line.c_str());
		}
	}

	void MemoryText::makeMemoryImage() {
		if (m_textBuf) {
			free(m_textBuf);
			m_textBuf = NULL;
		}

		m_textBufSize = _countTotalTextSizeInByte() + 1;
		m_textBuf = (char *)malloc(sizeof(char)*m_textBufSize);
		if (m_textBuf) {
			char *p = m_textBuf;

			int nLines = m_lines.size();
			for (int i=0; i<nLines; i++) {
				std::string line = m_lines[i];
				int lineSize = line.size();

				memcpy(p, line.c_str(), lineSize);
				p += lineSize;
			}

			m_textBuf[m_textBufSize-1] = '\0';
		}
	}

	int MemoryText::_countTotalTextSizeInByte() {
		int c=0;

		int nLines = m_lines.size();
		for (int i=0; i<nLines; i++) {
			std::string line = m_lines[i];

			c += line.size();
		}

		return c;
	}

	void MemoryText::_free() {
		m_lines.clear();

		if (m_textBuf) {
			free(m_textBuf);
			m_textBuf = NULL;
		}

		m_textBufSize = 0;
	}

	/////////////////////////////////////////////////////////
	//
	//	XMLParser
	//

//	static const char *encodingName = "SHIFT-JIS";
	static const char *encodingName = "UTF-8";
	static XMLFormatter::UnRepFlags unRepFlags = XMLFormatter::UnRep_CharRef;

	XMLParser::XMLParser() {
		// Initialize the XML4C2 system
		try
		{
			 XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& toCatch)
		{
			XStr msg(toCatch.getMessage());
			Jsp::printLog(LOG_FATAL, "Error during initialization: \n");
			Jsp::printLog(LOG_FATAL, "%s\n", msg.getString());

			return;
		}

		m_parser = new SAXParser();

		m_parser->setValidationScheme(SAXParser::Val_Auto);
		m_parser->setDoNamespaces(false);
		m_parser->setDoSchema(false);
		m_parser->setValidationSchemaFullChecking(false);
	}

	XMLParser::~XMLParser() {
		if (m_parser) {
			delete m_parser;
			m_parser = NULL;
		}

		XMLPlatformUtils::Terminate();
	}

	bool XMLParser::parse(const char *xmlFile, std::vector<std::string> &outTexts) {
		try
		{
			XMLParseHandler handler(encodingName, unRepFlags);
			m_parser->setDocumentHandler(&handler);
			m_parser->setErrorHandler(&handler);

			m_parser->parse(xmlFile);
		}
		catch (const XMLException& excep)
		{
			XStr msg(excep.getMessage());
			Jsp::printLog(LOG_FATAL, "*** exception occurred ***\n");
			Jsp::printLog(LOG_FATAL, "msg: %s\n", msg.getString());

			return false;
		}

		return true;
	}

	bool XMLParser::parse(Jsp::MemoryText &memText, std::vector<std::string> &outTexts) {
		try
		{
			XMLParseHandler handler(encodingName, unRepFlags);
			handler.setOutTexts(&outTexts);

			m_parser->setDocumentHandler(&handler);
			m_parser->setErrorHandler(&handler);

			memText.makeMemoryImage();

			char *p = memText.getTextPtr();
			int len = memText.getTextLength();

#if 0
Jsp::printLog(LOG_DEBUG, "*********\n");
Jsp::printLog(LOG_DEBUG, "%d\n", len);
Jsp::printLog(LOG_DEBUG, "%s", p);
Jsp::printLog(LOG_DEBUG, "*********\n");
#endif

			MemBufInputSource memsrc(
				(const XMLByte * const)p,
				(const XMLSize_t) len,
				(const XMLCh * const)"memory buffer");

			m_parser->parse(memsrc);
		}
		catch (const XMLException& excep)
		{
			XStr msg(excep.getMessage());
			Jsp::printLog(LOG_FATAL, "*** exception occurred ***\n");
			Jsp::printLog(LOG_FATAL, "msg: %s\n", msg.getString());

			return false;
		}

		return true;
	}

	/////////////////////////////////////////////////////////
	//
	//	XMLParseHandler
	//
	XMLParseHandler::XMLParseHandler(
		const char *const encodingName,
		const XMLFormatter::UnRepFlags unRepFlags) :
		fFormatter(encodingName, 0, this, XMLFormatter::NoEscapes, unRepFlags),
//		fFormatter(encodingName, 0, this, XMLFormatter::StdEscapes, unRepFlags),
		m_poutTexts(NULL)
	{
		m_indent = 0;

		// begin(add)(FIX20101029)
		// make transcoder object
		XMLTransService::Codes failReason;
		fTranscoder=XMLPlatformUtils::fgTransService->makeNewTranscoderFor(
			"UTF-8", failReason, TRANS_BUF_LEN, XMLPlatformUtils::fgMemoryManager
		);

		// exit if failed
		if (!fTranscoder)
		{
	  		fprintf(stderr, "*** error ***\n");
			exit(1);
		}
		// end(add)
	}

	XMLParseHandler::~XMLParseHandler()
	{
		// begin(add)(FIX20101029)
		// free transcoder object
		if (fTranscoder) delete fTranscoder;
		// end(add)
	}

	// -------------------------------------------------------
	void XMLParseHandler::startDocument() {
//		Jsp::printLog(LOG_DEBUG, "start_doc\n");
		m_indent++;
	}

	void XMLParseHandler::startElement(const XMLCh *const name, AttributeList &attributes) {

		XStr strName(name);

#if 1
		char indentStr[256];
		strcpy(indentStr, "");
		for (int a=0; a<m_indent; a++) strcat(indentStr, "  ");

		Jsp::printLog(LOG_DEBUG, "%s** %s **\n", indentStr, strName.getString());

		unsigned int len = attributes.getLength();
		for (unsigned int i=0; i<len; i++) {
			XStr attrName(attributes.getName(i));
			XStr attrValue(attributes.getValue(i));

			Jsp::printLog(LOG_DEBUG, "%s  %s (%s)\n", indentStr, attrName.getString(), attrValue.getString());
		}
#endif

		m_indent++;

		if (strcmp(strName.getString(), "WHYPO") == 0) {
			unsigned int len = attributes.getLength();
			for (unsigned int i=0; i<len; i++) {

// begin(fix)(FIX20101029)
#if 1
// new
				std::string attrName = transcode(attributes.getName(i));
				std::string attrValue = transcode(attributes.getValue(i));

				if (strcmp(attrName.c_str(), "WORD") == 0) {
					if (m_poutTexts) {
						m_poutTexts->push_back(attrValue);
					}
				}
#else
// orig
				XStr attrName(attributes.getName(i));
				XStr attrValue(attributes.getValue(i));

				if (strcmp(attrName.getString(), "WORD") == 0) {
					if (m_poutTexts) {
						m_poutTexts->push_back(attrValue.getString());
					}
				}
#endif
// end(fix)
			}
		}
	}

	void XMLParseHandler::endElement(const XMLCh *const name) {
		m_indent--;
	}

	void XMLParseHandler::endDocument() {
//		Jsp::printLog(LOG_DEBUG, "end_doc\n");
		m_indent--;
	}

	// -------------------------------------------------------
	void XMLParseHandler::warning(const SAXParseException &excep) {
	}

	void XMLParseHandler::error(const SAXParseException &excep) {
	}

	void XMLParseHandler::fatalError(const SAXParseException &excep) {
	}

	// -------------------------------------------------------
// begin(add)(FIX20101029)
	std::string XMLParseHandler::transcode(const XMLCh* pxSrcString)
	{
		std::string strReturn = "";
		XMLByte pbBuffer[TRANS_BUF_LEN+1];
		unsigned int srcLen = XMLString::stringLen(pxSrcString);
		const XMLCh* pxSrc = pxSrcString;

		while (srcLen > 0)
		{
			// convering XMLCh (=Unicode16) to UTF-8
			const unsigned srcChars = (srcLen > TRANS_BUF_LEN) ? TRANS_BUF_LEN : srcLen;
			unsigned int charsEaten;
			const unsigned int outBytes=fTranscoder->transcodeTo(
				pxSrc, srcChars, pbBuffer, TRANS_BUF_LEN, charsEaten, XMLTranscoder::UnRep_Throw
			);

			if (outBytes>0) pbBuffer[outBytes]=0;

			// accumulate converted strings
			std::string tmp((const char *)pbBuffer);
			strReturn += tmp;

			// update loop variable
			pxSrc += charsEaten;
			srcLen -= charsEaten;
		}

		return strReturn;
	}
// end(add)
};
