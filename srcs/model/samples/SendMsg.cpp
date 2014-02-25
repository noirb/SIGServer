/* $Id: SendMsg.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>


class RobotController : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};

#define ARY_SIZE(ARY) ( (int)(sizeof(ARY)/sizeof(ARY[0])) )

double RobotController::onAction(ActionEvent &evt)
{
	char *strs[] = {
		"Hello!!",
		"How are you",
	};

#if 1
	sendMessage("Robot2", ARY_SIZE(strs), strs);
#else
	broadcastMessage(ARY_SIZE(strs), strs);
#endif
	return 3.0;
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}


