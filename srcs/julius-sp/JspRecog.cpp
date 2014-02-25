#include "JspRecog.h"
#include "JspSocket.h"
#include "JspXMLParser.h"
#include "JspLog.h"
#include <string>
#include <vector>

#define JSP_RECOG_LINE_BUF_LEN	1024

namespace Jsp {
	Recog::Recog(Jsp::Socket *sockModule, Jsp::Socket *sockAdinnet) {
		m_pSockControl = sockModule;
		m_pSockAdinnet = sockAdinnet;
	}

	bool Recog::recognize(
		char *waveData,
		int dataLen,
		std::vector<std::string> &outTexts) {

		if (!m_pSockAdinnet || !m_pSockControl) return false;

		outTexts.clear();

		// send wave data
		m_pSockAdinnet->sendData(waveData, dataLen);

		// send end mark (length==0 means end of data)
		m_pSockAdinnet->sendData(NULL, 0);

		Jsp::MemoryText memText;
		memText.addXMLBeginStatement();

		bool insideRecogOut = false;
		for (;;) {
			char line[JSP_RECOG_LINE_BUF_LEN];

			char *str = m_pSockControl->getTextData(line, sizeof(line));
			if (!str) break;

			if (strcmp(str, "<RECOGOUT>")==0) {
				memText.addLine(str);
				insideRecogOut = true;
			} else if (strcmp(str, "</RECOGOUT>")==0) {
				memText.addLine(str);
				break;
			} else if (insideRecogOut) {
#if 1
				int n = strlen(str);
				bool bInsideDoubleQuote = false;
				std::string putStr("");
				for (int i=0; i<n; i++) {

					char ch = str[i];
					if (ch == '"') {
						bInsideDoubleQuote = bInsideDoubleQuote ? false : true;
						putStr += ch;
					} else {
						if (bInsideDoubleQuote)
						{
							if (ch == '<') {
								putStr += "&lt;";
							} else if (ch == '>') {
								putStr += "&gt;";
							} else if (ch == '&') {
								putStr += "&amp;";
							} else if (ch == '\'') {
								putStr += "&apos;";
							} else {
								putStr += ch;
							}
						} else {
							putStr += ch;
						}
					}
				}

				memText.addLine(putStr.c_str());
#else
				memText.addLine(str);
#endif
			}
		}

		if (getLogLevel() >= LOG_DEBUG)
		{
			Jsp::printLog(LOG_DEBUG, "******** input text for XMLParser *********\n");
			memText.printText();
			Jsp::printLog(LOG_DEBUG, "*******************************************\n");
		}

		if (!m_parser.parse(memText, outTexts)) {
			Jsp::printLog(LOG_FATAL, "[ERROR] ***** XML Parse Error ******\n");
			return false;
		}

		return true;
	}
};
