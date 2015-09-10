/* $Id: SendText.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>


class RobotController : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};

double RobotController::onAction(ActionEvent &evt)
{

#if 1
	sendText(evt.time(), "Robot2", "hello!");
#else

	double distance = 10.0;
	sendText(evt.time(), NULL, "hello!", distance);
#endif
	return 3.0;
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}


