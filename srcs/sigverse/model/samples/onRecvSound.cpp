/* $Id: onRecvSound.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/RawSound.h>
#include <sigverse/Logger.h>
#include <sigverse/Text.h>


class AgentController : public Controller
{
public:
	void 	onRecvSound(RecvSoundEvent &evt);
};

void AgentController::onRecvSound(RecvSoundEvent &evt)
{
	RawSound * sound = evt.getRawSound();

	RawSoundHeader &h = sound->getHeader();
	LOG_MSG(("channel = %d", h.getChannelNum()));
	LOG_MSG(("sampling rate = %u", h.getSamplingRate()));

	RawSoundEndian endian = h.getEndian();
	LOG_MSG(("endian : %s", endian == RAW_SOUND_LITTLE_ENDIAN? "little": "big"));
	int datalen = sound->getDataLen();
	char * data = sound->getData();
	// ...

	Text * text = getText(*sound);
	if (text) {

		Encode enc = text->getEncode();
		
		const char * str = text->getString();
		LOG_MSG(("text : %s", str));

		delete text;
	}
}

extern "C"  Controller * createController ()
{
	return new AgentController;
}

