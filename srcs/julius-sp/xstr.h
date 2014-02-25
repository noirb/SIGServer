#ifndef _X_STR_H_
#define _X_STR_H_

#include <xercesc/util/XMLString.hpp>
#include <string>

XERCES_CPP_NAMESPACE_USE

// ------------------------------------------------------------
///
// ------------------------------------------------------------
class XStr {
public:
	// ----------------------------------------------------
	// ----------------------------------------------------
	XStr() : m_xmlString(NULL) {}

	// ----------------------------------------------------
	///
	// ----------------------------------------------------
	XStr(const XMLCh *const str) : m_xmlString(NULL) {
		if (str) {
			m_xmlString = XMLString::transcode(str);
		}
	}

	// ----------------------------------------------------
	// ----------------------------------------------------
	~XStr() {
		if (m_xmlString) {
			XMLString::release(&m_xmlString);
			m_xmlString = NULL;
		}
	}

	// ----------------------------------------------------
	///
	///
	// ----------------------------------------------------
	const char *getString() { return m_xmlString; }

private:
	char *m_xmlString;
};

#endif

