#ifndef _JSP_H_
#define _JSP_H_

#include "JspSocket.h"
#include "CommDataDecoder.h"
#include "CommDataListener.h"
#include <string>

namespace Jsp
{
	// ----------------------------------------------------
	// ----------------------------------------------------
	class JuliusServiceProvider : public CommDataDecoder::Listener
	{
	public:
		// ----------------------------------------------------
		// ----------------------------------------------------
		JuliusServiceProvider();

		// ----------------------------------------------------
		///
		///
		// ----------------------------------------------------
		void setLoadedSigJspConfFile(const char *pLoadedSigJspConfFile);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setSimServerHostName(const std::string &serverName);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setSimServerPort(int port);

		// ----------------------------------------------------
		///
		///
		// ----------------------------------------------------
		void setJspServiceProviderName(const std::string &serviceProviderName);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setJspServiceAcceptPort(int port);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setJuliusModulePort(int port);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setJuliusAdinnetPort(int port);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setJuliusPath(const std::string &juliusPath);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setJuliusConfFile(const std::string &juliusConfFile);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void setDelayTimeForStartJulius(int msec);

		// ----------------------------------------------------
		// ----------------------------------------------------
		void printSettings();

		// ----------------------------------------------------
		// ----------------------------------------------------
		void invokeJulius();

		// ----------------------------------------------------
		// ----------------------------------------------------
		void killJulius();

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		bool isJuliusAvairable() { return m_startJuliusOk; }

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		std::string getHostName();

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		std::string getHostIPAddr();

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		bool connectToSimServer();

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		bool sendProvideServiceRequest();

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		bool setupServiceAcceptSocket();

		// ----------------------------------------------------
		// ----------------------------------------------------
		void recognizeLoop();

	private:
		std::string m_LoadedSigJspConfFile;
		std::string m_simServerHostName;
		int m_simServerPort;
		std::string m_serviceProviderName;
		int m_jspServiceAcceptPort;
		int m_juliusModulePort;
		int m_juliusAdinnetPort;
		std::string m_juliusPath;
		std::string m_juliusConfFile;
		int m_delayTimeForStartJulius;

		int m_childPid;
		Jsp::Socket m_sockModule;
		Jsp::Socket m_sockAdinnet;

		Jsp::Socket m_sockSimServer;
		Jsp::Socket m_sockServiceAccept;

		bool m_startJuliusOk;

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void recvRequestNSPinger(Source &from, RequestNSPingerEvent &evt);

		// ----------------------------------------------------
		///
		// ----------------------------------------------------
		void recvRequestSoundRecog(Source &from, RequestSoundRecogEvent &evt);
	};
};

#endif

