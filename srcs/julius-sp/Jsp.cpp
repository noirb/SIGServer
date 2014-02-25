#include "Jsp.h"
#include "JspStringUtil.h"
#include "JspPacket.h"
#include "JspSocket.h"
#include "JspRecog.h"
#include "JspLog.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"
#include "comm/encoder/Attach.h"
#include "Source.h"
#include "RawSound.h"
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string>
#include <vector>

namespace Jsp
{
	JuliusServiceProvider::JuliusServiceProvider()
	{
		m_childPid = -1;
		m_startJuliusOk = false;

		// ******* don't forget to do this first !!! ********
		CommDataEncoder::setSendProc(CommUtil::sendData);
	}

	void JuliusServiceProvider::setLoadedSigJspConfFile(const char *pLoadedSigJspConfFile)
	{
		if (pLoadedSigJspConfFile)
		{
			// sigjsp's config file was successfully loaded
			m_LoadedSigJspConfFile = pLoadedSigJspConfFile;
		}
		else
		{
			// not loaded (not exist)
			m_LoadedSigJspConfFile = "";
		}
	}

	void JuliusServiceProvider::setSimServerHostName(const std::string &serverName)
	{
		m_simServerHostName = serverName;
	}

	void JuliusServiceProvider::setSimServerPort(int port)
	{
		m_simServerPort = port;
	}

	void JuliusServiceProvider::setJspServiceProviderName(const std::string &serviceProviderName)
	{
		m_serviceProviderName = serviceProviderName;
	}

	void JuliusServiceProvider::setJspServiceAcceptPort(int port)
	{
		m_jspServiceAcceptPort = port;
	}

	void JuliusServiceProvider::setJuliusModulePort(int port)
	{
		m_juliusModulePort = port;
	}

	void JuliusServiceProvider::setJuliusAdinnetPort(int port)
	{
		m_juliusAdinnetPort = port;
	}

	void JuliusServiceProvider::setJuliusPath(const std::string &juliusPath)
	{
		m_juliusPath = juliusPath;
	}

	void JuliusServiceProvider::setJuliusConfFile(const std::string &juliusConfFile)
	{
		m_juliusConfFile = juliusConfFile;
	}

	void JuliusServiceProvider::setDelayTimeForStartJulius(int msec)
	{
		m_delayTimeForStartJulius = msec;
	}

	void JuliusServiceProvider::printSettings()
	{
		Jsp::printLog(LOG_ALWAYS, "********************   sigjsp settings  *********************\n");

		Jsp::printLog(LOG_ALWAYS, "sigjsp.conf path              ");
		if (m_LoadedSigJspConfFile.size() > 0)
		{
			Jsp::printLog(LOG_ALWAYS, "(%s)\n", m_LoadedSigJspConfFile.c_str());
		}
		else
		{
			Jsp::printLog(LOG_ALWAYS, "(NOT FOUND)\n");
		}

		Jsp::printLog(LOG_ALWAYS, "sigserver hostname            (%s)\n",
			m_simServerHostName.c_str());
		Jsp::printLog(LOG_ALWAYS, "sigserver port                (%d)\n",
			m_simServerPort);
		Jsp::printLog(LOG_ALWAYS, "sigjsp service provider name  (%s)\n",
			m_serviceProviderName.c_str());
		Jsp::printLog(LOG_ALWAYS, "sigjsp service accept port    (%d)\n",
			m_jspServiceAcceptPort);
		Jsp::printLog(LOG_ALWAYS, "julius path                   (%s)\n",
			m_juliusPath.c_str());
		Jsp::printLog(LOG_ALWAYS, "julius config file            (%s)\n",
			m_juliusConfFile.c_str());
		Jsp::printLog(LOG_ALWAYS, "julius module port            (%d)\n",
			m_juliusModulePort);
		Jsp::printLog(LOG_ALWAYS, "julius adinnet port           (%d)\n",
			m_juliusAdinnetPort);
		Jsp::printLog(LOG_ALWAYS, "delay for start julius        (%d)\n",
			m_delayTimeForStartJulius);
		Jsp::printLog(LOG_ALWAYS, "log level (1-3)               (%d)\n",
			getLogLevel());
		Jsp::printLog(LOG_ALWAYS, "*************************************************************\n");
	}

	void JuliusServiceProvider::invokeJulius()
	{
		m_childPid = fork();

		if (m_childPid == 0)
		{
			// +++++++++++++++++++++++++++++++++++
			//	child process executes here
			// +++++++++++++++++++++++++++++++++++

			// -----------------------------------
			//	make parameter for execv()
			// -----------------------------------
			Jsp::StringArray a;

			a.clear();
			a.add(m_juliusPath.c_str());	// (ex) "/usr/local/bin/julius-4.1.2"
			a.add("-C");
			a.add(m_juliusConfFile.c_str());	// (ex) "fast.jconf"

			char **pp = a.getArray(true);

			// -----------------------------------
			//	start julius
			// -----------------------------------
			execv(m_juliusPath.c_str(), pp);
		}
		else
		{
			// +++++++++++++++++++++++++++++++++++
			//	parent process executes here
			// +++++++++++++++++++++++++++++++++++

			// -----------------------------------
			//	wait until julius started
			// -----------------------------------
			usleep(m_delayTimeForStartJulius * 1000);	// micro seconds

			int delayTime;
			int retryCount;
			bool succeeded;

			// -----------------------------------
			//	connect to julius module port
			// -----------------------------------
			delayTime = 100000;
			retryCount = 5;
			while (retryCount > 0)
			{
				succeeded = m_sockModule.connect("localhost", m_juliusModulePort);

				if (succeeded) break;

				usleep(delayTime);
				delayTime *= 2;
				retryCount--;
			}

			if (!succeeded)
			{
				Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++++++\n");
				Jsp::printLog(LOG_FATAL, "  julius module port connect failed\n");
				Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++++++\n");

				return;
			}

			Jsp::printLog(LOG_ALWAYS, "===================================\n");
			Jsp::printLog(LOG_ALWAYS, "  julius module port connect ok!\n");
			Jsp::printLog(LOG_ALWAYS, "===================================\n");

			// -----------------------------------
			//	connect to julius adinnet port
			// -----------------------------------
			delayTime = 100000;
			retryCount = 5;
			while (retryCount > 0)
			{
				succeeded = m_sockAdinnet.connect("localhost", m_juliusAdinnetPort);

				if (succeeded) break;

				usleep(delayTime);
				delayTime *= 2;
				retryCount--;
			}

			if (!succeeded)
			{
				Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++++++\n");
				Jsp::printLog(LOG_FATAL, "  julius adinnet port connect failed\n");
				Jsp::printLog(LOG_FATAL, "++++++++++++++++++++++++++++++++++++++\n");

				return;
			}

			Jsp::printLog(LOG_ALWAYS, "===================================\n");
			Jsp::printLog(LOG_ALWAYS, "  julius adinnet port connect ok!\n");
			Jsp::printLog(LOG_ALWAYS, "===================================\n");

			m_startJuliusOk = true;
		}
	}

	void JuliusServiceProvider::killJulius()
	{
		if (m_startJuliusOk && (m_childPid > 0))
		{
			kill(m_childPid, SIGKILL);

			m_childPid = -1;
			m_startJuliusOk = false;
		}
	}

	std::string JuliusServiceProvider::getHostName()
	{
		std::string strHostName("");

		// ---------------------------------------
		//	get host name of this machine
		// ---------------------------------------
		char hostname[1024];

		int status = gethostname(hostname, sizeof(hostname));
		if (status < 0)
		{
			Jsp::printLog(LOG_FATAL, "cannot retreive hostname of local machine\n");
			return strHostName;
		}

		strHostName = hostname;

		return strHostName;
	}

	std::string JuliusServiceProvider::getHostIPAddr()
	{
		std::string strIPAddr("");
		std::string hostname = getHostName();

		// ---------------------------------------
		//	get IP Addr from host name
		// ---------------------------------------
		struct hostent *hp = gethostbyname(hostname.c_str());
		if (!hp)
		{
			Jsp::printLog(LOG_FATAL, "unknown host : %s\n", hostname.c_str());
			return strIPAddr;
		}

		in_addr addr;
		memcpy(&addr, hp->h_addr_list[0], sizeof(in_addr));

		// ---------------------------------------
		//	convert to string
		// ---------------------------------------
		strIPAddr = inet_ntoa(addr);

		return strIPAddr;
	}

	bool JuliusServiceProvider::connectToSimServer()
	{
		return m_sockSimServer.connect(m_simServerHostName.c_str(), m_simServerPort);
	}

	bool JuliusServiceProvider::sendProvideServiceRequest()
	{
		if (!m_sockSimServer.connected()) return false;

		Jsp::printLog(LOG_ALWAYS, "register (%s:%d) as service provider\n", m_serviceProviderName.c_str(), m_jspServiceAcceptPort);

		::Service::Kind kind = ::Service::SOUND_RECOG;
		::Service service("julius-sp", kind, m_serviceProviderName.c_str(), m_jspServiceAcceptPort);

		// send COMM_REQUEST_PROVIDE_SERVICE
		CommRequestProvideServiceEncoder enc(service);
		if (enc.send(m_sockSimServer.socket()) < 0)
		{
			Jsp::printLog(LOG_FATAL, "packet send error [%s:%d]\n", __FILE__, __LINE__);
			return false;
		}

		return true;
	}

	bool JuliusServiceProvider::setupServiceAcceptSocket()
	{
		return m_sockServiceAccept.bind(m_jspServiceAcceptPort);
	}

	void JuliusServiceProvider::recognizeLoop()
	{
		CommDataDecoder decoder;
		decoder.setListener(this);

#if 0
		// for debug
		const char *dumpFile = "packet.dat";
		unlink(dumpFile);
#endif

		for (;;)
		{
			Jsp::Socket newSock;
			std::string hostname;

			Jsp::printLog(LOG_ALWAYS, "waiting for connection ... (accept sock: %d)\n", m_sockServiceAccept.socket());

			// ******** (1) ********
			if (!m_sockServiceAccept.accept(newSock, hostname))
			{
				Jsp::printLog(LOG_FATAL, "accept failed.\n");
				break;
			}

			Jsp::printLog(LOG_ALWAYS, "connection accepted. (sock=%d) \n", newSock.socket());

			Source *src = new Source(newSock.socket(), hostname.c_str());

			Jsp::Packet *packet = NULL;
			for (;;)
			{
				int status = newSock.recvPacket(&packet);
				if (status == 0)
				{
					// connection gracefully closed by opponents

					// close socket
					Jsp::printLog(LOG_DEBUG, "socket closed by opponents.\n");

					// break loop and wait for another connection request again.
					break;
				}
				else if (status < 0)
				{
					// error
					Jsp::printLog(LOG_FATAL, "socket error occurred\n");
					return;
				}
				else
				{

#if 0
// for debug
					packet->writeToFile(dumpFile, true);
#endif
					// ------------------------------------
					//	decode packet and get result
					// ------------------------------------
					int decodedBytes = decoder.push(*src, packet->getData(), packet->getLength());
					if (decodedBytes < 0)
					{
						Jsp::printLog(LOG_FATAL, "decode failed\n");
						return;
					}
				}

				if (packet)
				{
					// no need anymore
					delete packet;
					packet = NULL;
				}
			}

			// ***********************************************
			//	[CAUTION]
			//	YOU MUST NOT CLOSE SOCKET HERE!!
			//	newSock has already beed closed by opponents.
			//	if you close socket here, accept() never
			//	succeed again (see (1) above)
			// ***********************************************
			// newSock.close();

			delete src;
			src = NULL;
		}
	}

	void JuliusServiceProvider::recvRequestNSPinger(Source &from, RequestNSPingerEvent &)
	{
		Jsp::printLog(LOG_INFO, "*****  recvRequestNSPinger called  *****\n");

		CommResultNSPingerEncoder enc(true);

		if (enc.send(from.socket()) < 0)
		{
			Jsp::printLog(LOG_FATAL, "[ERROR] failed to send NSPingerResult\n");
			return;
		}

		Jsp::printLog(LOG_INFO, "*****  sendNSPingerResult ... ok  *****\n");
	}

	void JuliusServiceProvider::recvRequestSoundRecog(Source &from, RequestSoundRecogEvent &evt)
	{
		int n;

		Jsp::printLog(LOG_INFO, "*****  recvRequestSoundRecog called  *****\n");

		// ------------------------------------
		//	get wave data
		// ------------------------------------
		RawSound *sound = evt.releaseRawSound();

		if (m_startJuliusOk && sound)
		{
			// ------------------------------------
			//	recognize
			// ------------------------------------
			Jsp::Recog recog(&m_sockModule, &m_sockAdinnet);

			char *wave = sound->getData();
			int waveLen = sound->getDataLen();

#if 0
			FILE *waveFp = fopen("out.wav", "w");
			if (waveFp)
			{
				fwrite(wave, 1, waveLen, waveFp);
				fclose(waveFp);
			}
#endif

			std::vector<std::string> outTexts;
			if (recog.recognize(wave, waveLen, outTexts))
			{
				// ------------------------------------
				//	get result of recognition
				// ------------------------------------
				Jsp::printLog(LOG_INFO, "*******  Recognition Result  *******\n");

				n = outTexts.size();
				for (int i=0; i<n; i++)
				{
					Jsp::printLog(LOG_INFO, "(%s)\n", outTexts[i].c_str());
				}

				// ------------------------------------
				//	send it back to controller
				// ------------------------------------
				std::string outText("");
				n = outTexts.size();
				for (int i=0; i<n; i++)
				{
					outText = outText + outTexts[i];
				}

				CommResultSoundRecogEncoder enc(ENCODE_UTF8, outText.c_str());
				if (enc.send(from.socket()) < 0)
				{
					Jsp::printLog(LOG_FATAL, "failed to send SoundRecogResult\n");
				}
				else
				{
					Jsp::printLog(LOG_INFO, "*****  sendSoundRecogResult ... ok  *****\n");
				}
			}
		}

		if (sound)
		{
			delete sound;
			sound = NULL;
		}
	}
};
