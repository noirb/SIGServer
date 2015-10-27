/* $Id: GoRound.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/modelerr.h>
#include <sigverse/commonlib/ControllerEvent.h>
#include <sigverse/commonlib/SimObj.h>
#include <sigverse/commonlib/Logger.h>
#include <math.h>

class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

double RobotController::onAction(ActionEvent &evt)
{
	LOG_MSG(("\ncurrent time : %f", evt.time()));

	static int cnt = 0;

	try {
		const char *name = myname();
		SimObj *obj = getObj(name);
		obj->dump();
		if (!obj->dynamics()) {

			double angle = 2*M_PI*cnt*0.01;

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

