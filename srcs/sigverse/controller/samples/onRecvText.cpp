/* $Id: onRecvText.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/ControllerEvent.h>
#include <sigverse/commonlib/Logger.h>

class RobotController : public Controller
{
public:
	void 	onRecvText(RecvTextEvent &evt);
};




void RobotController::onRecvText(RecvTextEvent &evt)
{
	
	const char *caller = evt.getCaller(); 

	const char *text = evt.getText();
	LOG_MSG(("receive from %s : \"%s\"", caller, text));
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}

