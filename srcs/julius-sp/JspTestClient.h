#ifndef _JSP_TEST_CLIENT_H_
#define _JSP_TEST_CLIENT_H_

#include "JspSocket.h"
#include "CommDataDecoder.h"
#include "CommDataListener.h"
#include <string>

namespace Jsp
{
	class TestClient : public CommDataDecoder::Listener
	{
	public:
		TestClient();

		void setJuliusSpHostName(const std::string &hostName);
		void setJuliusSpPort(int port);

		void printSettings();
		void showHelp();

		void commandLoop();

		void ping();
		void soundRecog(const char *waveFile);

		bool connectToJuliusSp();
		void receiveResultFromJuliusSp();

	private:
		std::string m_juliusSpHostName;
		int m_juliusSpPort;
		Jsp::Socket m_sockJuliusSp;

		bool m_resultReached;

		void recvResultNSPinger(Source &from, ResultNSPingerEvent &);
		void recvResultSoundRecog(Source &from, ResultSoundRecogEvent &);
	};
};

#endif
