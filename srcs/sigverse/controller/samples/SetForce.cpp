/* $Id: SetForce.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/SimObj.h>
#include <sigverse/commonlib/Logger.h>


class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

double RobotController::onAction(ActionEvent &evt)
{
	SimObj *obj = getObj(myname());
	if (obj) {
		obj->setForce(0.0, 1000.0, 0.0);
	}

	return 5.0;
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}

