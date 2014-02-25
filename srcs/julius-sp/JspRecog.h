#ifndef _JSP_RECOG_H_
#define _JSP_RECOG_H_

#include "JspSocket.h"
#include "JspXMLParser.h"
#include <string>
#include <vector>

namespace Jsp
{
	// ------------------------------------------
	// ------------------------------------------
	class Recog
	{
	public:
		// ------------------------------------------
		///
		// ------------------------------------------
		Recog(Jsp::Socket *sockModule, Jsp::Socket *sockAdinnet);

		// ------------------------------------------
		// ------------------------------------------
		~Recog() {}

		// ------------------------------------------
		///
		///
		///
		// ------------------------------------------
		bool recognize(
			char *waveData,
			int dataLen,
			std::vector<std::string> &outTexts);

	private:
		Jsp::Socket *m_pSockAdinnet;
		Jsp::Socket *m_pSockControl;
		Jsp::XMLParser m_parser;
	};
};

#endif

