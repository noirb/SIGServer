#include <unistd.h>
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/RawSound.h>
#include <sigverse/Logger.h>
#include <sigverse/Text.h>

class VoiceRecogController : public Controller
{
public:
	void onRecvMessage(RecvMessageEvent &evt);
private:
};

void VoiceRecogController::onRecvMessage(RecvMessageEvent &evt)
{
	int n = evt.getSize();

	for (int i=0; i<n; i++)
	{
		LOG_MSG(("[%d] (%s)", i, evt.getString(i)));
	}

	if (n>1)
	{
		const char *cmd = evt.getString(0);
		if (cmd)
		{
			if (strcmp(cmd, "recog")==0)
			{
				if (n>=2)
				{
					const char *waveFile = evt.getString(1);

					// load wave file and send recog request
					RawSound *sound = new RawSound();

					if (sound->loadWaveFile(waveFile))
					{
						LOG_MSG(("%s load ok. send it to julius-sp.", waveFile));

						Text * text = getText(*sound);
						if (text)
						{
							Encode enc = text->getEncode();

							const char * str = text->getString();
							LOG_MSG(("text : (%s)", str));

							delete text;
						}
					}
					else
					{
						LOG_MSG(("failed to load %s", waveFile));
					}
				}
				else
				{
					LOG_MSG(("insufficient argument. need wave file name"));
				}
			}
			else if (strcmp(cmd, "loop")==0)
			{
				if (n>=3)
				{
					const char *waveFile = evt.getString(1);
					int delayInMillisec = atoi(evt.getString(2));

					// load wave file and send recog request
					RawSound *sound = new RawSound();

					if (sound->loadWaveFile(waveFile))
					{
						LOG_MSG(("%s load ok. send it to julius-sp.", waveFile));

						for (;;)
						{
							LOG_MSG(("sending voice to sigjsp .."));
							Text * text = getText(*sound);
							if (text)
							{
								Encode enc = text->getEncode();

								const char * str = text->getString();
								LOG_MSG(("text : (%s)", str));

								delete text;
							}

							LOG_MSG(("sleep %d ms..", delayInMillisec));
							usleep(delayInMillisec * 1000);
						}
					}
					else
					{
						LOG_MSG(("failed to load %s", waveFile));
					}
				}
			}
		}
	}
}

extern "C" Controller *createController()
{
	return new VoiceRecogController;
}
