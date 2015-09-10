/* $Id: GoRound.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/Controller.h>
#include <sigverse/modelerr.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>
#include <math.h>

class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

#define PI 3.14159265359

double RobotController::onAction(ActionEvent &evt)
{
	LOG_MSG(("\ncurrent time : %f", evt.time()));

	static int cnt = 0;

	try {
		const char *name = myname();
		SimObj *obj = getObj(name);
		obj->dump();
		if (!obj->dynamics()) {

			double angle = 2*PI*cnt*0.01;

			double xx = 5*sin(angle);
			double yy = 0.5;
			double zz = 5*cos(angle);
		
			LOG_MSG(("pos (%f, %f, %f)", xx, yy, zz));
			obj->setPosition(xx, yy, zz);
			obj->setAxisAndAngle(0.0, 1.0, 0.0, angle);
		}
		obj->dump();

	} catch(SimObj::NoAttributeException &) {
		
	} catch(SimObj::AttributeReadOnlyException &) {
		
	} catch(SimObj::Exception &) {
		
	}
	cnt++;

	return 0.1;
}
		
extern "C"  Controller * createController ()
{
	return new RobotController;
}

