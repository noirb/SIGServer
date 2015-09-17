/* $Id: onRecvMsg.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/Logger.h>



class RobotController : public Controller
{
private:
	void onRecvMessage(RecvMessageEvent &evt);
};

void RobotController::onRecvMessage(RecvMessageEvent &evt)
{
	LOG_MSG(("sender : %s", evt.getSender()));

	int n = evt.getSize();
	LOG_MSG(("# of string : %d", n));
	for (int i=0; i<n; i++) {
		LOG_MSG(("string[%d] : %s", i, evt.getString(i)));
	}
}
	
extern "C"  Controller * createController ()
{
	return new RobotController;
}


