#include "JspTestClient.h"
#include "JspStringUtil.h"
#include "JspPacket.h"
#include "JspSocket.h"
#include "JspRecog.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"
#include "comm/encoder/Attach.h"
#include "Source.h"
#include "RawSound.h"
#include "Text.h"
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
	TestClient::TestClient()
	{
		// ******* don't forget to do this first !!! ********
		CommDataEncoder::setSendProc(CommUtil::sendData);
	}

	void TestClient::setJuliusSpHostName(const std::string &hostName)
	{
		m_juliusSpHostName = hostName;
	}

	void TestClient::setJuliusSpPort(int port)
	{
		m_juliusSpPort = port;
	}

	void TestClient::printSettings()
	{
		fprintf(stderr, "***************  sigjspTest settings  ****************\n");
		fprintf(stderr, "sigjsp host name (%s)\n", m_juliusSpHostName.c_str());
		fprintf(stderr, "sigjsp port      (%d)\n", m_juliusSpPort);
		fprintf(stderr, "******************************************************\n");
	}

	void TestClient::showHelp()
	{
		fprintf(stderr, "*** [ command list ] ***************************************\n");
		fprintf(stderr, "ping .............. send COMM_NS_PINGER_REQUEST to sigjsp\n");
		fprintf(stderr, "recog [wavefile] .. send REQUEST_SOUND_RECOG to sigjsp\n");
		fprintf(stderr, "info .............. show sigjsp hostname and port no\n");
		fprintf(stderr, "help .............. show help\n");
		fprintf(stderr, "quit .............. quit this program\n");
		fprintf(stderr, "************************************************************\n");
	}

	void TestClient::commandLoop()
	{
		showHelp();

		for (;;)
		{
			char line[256];

			printf("> ");
			fgets(line, sizeof(line), stdin);

			// strip off NL at the end of line
			line[strlen(line)-1]='\0';

			Jsp::StringArray a;
			a.clear();
			a.decomp(line);

			int nargs = a.count();
			char **args = a.getArray();

			if (nargs == 0 || args == NULL) continue;

			if (strcmp(args[0], "quit")==0)
			{
				return;
			}
			else if (strcmp(args[0], "help")==0)
			{
				showHelp();
			}
			else if (strcmp(args[0], "info")==0)
			{
				printSettings();
			}
			else if (strcmp(args[0], "ping")==0)
			{
				ping();
			}
			else if (strcmp(args[0], "recog")==0)
			{
				if (nargs < 2)
				{
					fprintf(stderr, "wave file not specified\n");
					continue;
				}

				soundRecog(args[1]);
			}
			else
			{
				fprintf(stderr, "? unknown command\n");
			}
		}
	}

	void TestClient::ping()
	{
		if (connectToJuliusSp())
		{
			Service::Kind kind = Service::SOUND_RECOG;

			CommRequestNSPingerEncoder enc(kind);

			// ----------------------------------------
			//	send ping
			// ----------------------------------------
			if (enc.send(m_sockJuliusSp.socket()) < 0)
			{
				fprintf(stderr, "send failed (sock=%d)\n", m_sockJuliusSp.socket());
				exit(1);
			}
			else
			{
				fprintf(stderr, "<< COMM_NS_PINGER_REQUEST sent. (sock=%d)\n", m_sockJuliusSp.socket());
			}

			// ----------------------------------------
			//	receive answer from sigjsp
			// ----------------------------------------
			receiveResultFromJuliusSp();
		}
	}

	void TestClient::soundRecog(const char *waveFile)
	{
		if (connectToJuliusSp())
		{
			// ----------------------------------------
			//	load wave file
			// ----------------------------------------
			RawSound *sound = new RawSound();

			if (sound->loadWaveFile(waveFile))
			{
				fprintf(stderr, "%s load ok\n", waveFile);

				// ----------------------------------------
				//	send wave data to sigjsp
				// ----------------------------------------
				CommRequestSoundRecogEncoder enc(*sound);
				if (enc.send(m_sockJuliusSp.socket()) < 0)
				{
					fprintf(stderr, "send failed (sock=%d)\n", m_sockJuliusSp.socket());
					exit(1);
				}
				else
				{
					fprintf(stderr, "<< REQUEST_SOUND_RECOG sent. (sock=%d)\n", m_sockJuliusSp.socket());
				}
			}
			else
			{
				// wave file load failed
				return;
			}

			// ----------------------------------------
			//	receive result from sigjsp
			// ----------------------------------------
			receiveResultFromJuliusSp();
		}
	}

	bool TestClient::connectToJuliusSp()
	{
		// --------------------------------
		//	connect to sigjsp
		// --------------------------------
		bool succeeded = m_sockJuliusSp.connect(m_juliusSpHostName.c_str(), m_juliusSpPort);
		if (!succeeded)
		{
			fprintf(stderr, "connect failed\n");
			return false;
		}

		fprintf(stderr, "connect to sigjsp ... ok\n");
		return true;
	}

	void TestClient::receiveResultFromJuliusSp()
	{
		if (!m_sockJuliusSp.connected()) return;

		CommDataDecoder decoder;
		decoder.setListener(this);

		Jsp::Packet *packet = NULL;
		m_resultReached = false;

		for (;;)
		{
			// --------------------------------
			//	receive packet from sigjsp
			// --------------------------------
			int status = m_sockJuliusSp.recvPacket(&packet);
			if (status == 0)
			{
				// connection gracefully closed by opponents

				// close socket
				fprintf(stderr, "socket closed by opponents.\n");

				// exit loop
				goto exit;
			}
			else if (status < 0)
			{
				// error
				fprintf(stderr, "socket error occurred\n");
				exit(1);
			}
			else
			{
#if 0
// for debug
				packet->writeToFile("packet.dat", true);
#endif
				// -----------------------
				//	decode packet
				// -----------------------
				Source *src = new Source(m_sockJuliusSp.socket());
				int decodedBytes = decoder.push(*src, packet->getData(), packet->getLength());
				if (decodedBytes < 0)
				{
					fprintf(stderr, "decode failed\n");
					exit(1);
				}

				delete src;
				src = NULL;

				if (m_resultReached) goto exit;
			}

			if (packet)
			{
				// no need anymore
				delete packet;
				packet = NULL;
			}
		}

exit:
		if (packet)
		{
			// no need anymore
			delete packet;
			packet = NULL;
		}

		int sock = m_sockJuliusSp.socket();
		m_sockJuliusSp.close();
		fprintf(stderr, "socket closed. (sock=%d)\n", sock);
	}

	void TestClient::recvResultNSPinger(Source &from, ResultNSPingerEvent &)
	{
		fprintf(stderr, ">> COMM_NS_PINGER_RESULT received.\n");

		m_resultReached = true;
	}

	void TestClient::recvResultSoundRecog(Source &from, ResultSoundRecogEvent &evt)
	{
		fprintf(stderr, ">> RESULT_SOUND_RECOG received. \n");

		// ------------------------------------
		//	get recognized test
		// ------------------------------------
		Text *text = evt.releaseText();

		if (text)
		{
			fprintf(stderr, "returned text : (%s)\n", text->getString());

			delete text;
		}

		m_resultReached = true;
	}
};
