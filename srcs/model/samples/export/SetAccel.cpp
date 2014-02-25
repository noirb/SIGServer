/* $Id: SetAccel.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>


class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

double RobotController::onAction(ActionEvent &evt)
{
	SimObj *obj = getObj(myname());
	if (obj) {
		obj->setAccel(0.0, 100.0, 0.0);
	}

	return 5.0;
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}

