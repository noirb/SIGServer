/* $Id: Gravity.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
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


double RobotController::onAction(ActionEvent &evt)
{
	LOG_MSG(("\ncurrent time : %f", evt.time()));

	static int cnt = 0;

	double tx = 0.0;
	double tz = 0.0;

	try {
		{
			const char *name = myname();
			SimObj *obj = getObj(name);
			double x = obj->x();
			//			double y = obj->y();
			double z = obj->z();
			if (obj) {
				LOG_MSG(("dynamics = %s", obj->dynamics()? "true": "false"));
				LOG_MSG(("P (%f, %f, %f)", obj->x(), obj->y(), obj->z()));
				LOG_MSG(("F (%f, %f, %f)", obj->fx(), obj->fy(), obj->fz()));
				LOG_MSG(("Q (%f, %f, %f, %f)", obj->qw(), obj->qx(), obj->qy(), obj->qz()));

			}

			if (z < 2.0) {
				double fz = 0.0;
				double fx = 0.0;
			
				double dz = -(z - tz);
				double dx = -(x - tx);
				LOG_MSG(("(dx, dy, dz) = (%f, %f, %f)", dx, 0, dz));
				double l = sqrt(dx*dx + dz*dz);
				if (l > 0.5) {
					fz = 5*dz/(l);
					fx = 5*dz/(l);
				}
				obj->fz(fz); obj->fx(fx);
				obj->setForce(fx, 0, fz);

			}
		}

		{
			std::string target;
			if (strcmp(myname(), "Robot1") == 0) {
				target = "Robot2";
			} else {
				target = "Robot1";
			}
			char *text = "hello";
			Controller::sendText(evt.time(), target.c_str(), text);
		}
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

