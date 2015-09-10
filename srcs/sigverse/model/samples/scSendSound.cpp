/* $Id: scSendSound.cpp,v 1.2 2011-05-12 08:33:53 msi Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/RawSound.h>
#include <sigverse/Logger.h>


class RobotController : public Controller
{
public:
	void onInit(InitEvent &evt);
	double onAction(ActionEvent &evt);
	void onRecvMessage(RecvMessageEvent &evt);
	void onRecvSound(RecvSoundEvent &evt);
};

void RobotController::onInit(InitEvent &evt)
{
	LOG_MSG(("controller started"));
}

double RobotController::onAction(ActionEvent &evt)
{
	return 1.0;
}

void RobotController::onRecvMessage(RecvMessageEvent &evt)
{
	int n = evt.getSize();

	if (n>0)
	{
		std::string strCmd = evt.getString(0);
		const char *cmd = strCmd.c_str();
		LOG_MSG(("cmd : %s", cmd));

		if (strcmp(cmd, "sound") == 0)
		{
			RawSound sound;

			std::string strTarget = evt.getString(1);
			const char *target = strTarget.c_str();
			LOG_MSG(("target : %s", target));

			const char *soundFile = "sample.wav";

			LOG_MSG(("loading %s ... ", soundFile));
			if (sound.loadWaveFile(soundFile))
			{
				LOG_MSG(("send %s to %s", soundFile, target));
				sendSound(0, target, sound);
			}
			else
			{
				LOG_MSG(("failed to load %s", soundFile));
			}
		}
	}
}

void RobotController::onRecvSound(RecvSoundEvent &evt)
{
	RawSound * sound = evt.getRawSound();
	const char *name = evt.getCaller();

	int datalen = sound->getDataLen();
//	char *data = sound->getData();
	LOG_MSG(("sound data received. (sender=%s, size=%d)", name, datalen));
}

extern "C" Controller *createController()
{
	return new RobotController;
}

