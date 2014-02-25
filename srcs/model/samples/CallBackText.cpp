/* $Id: CallBackText.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/Logger.h>

class RobotController : public Controller
{
public:
	void onRecvText(RecvTextEvent &evt);
};

void RobotController::onRecvText(RecvTextEvent &evt) 
{
	LOG_MSG(("received from %s: \"%s\"", evt.getCaller(), evt.getText()));
	sendText(evt.time(), evt.getCaller(), evt.getText());
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}

