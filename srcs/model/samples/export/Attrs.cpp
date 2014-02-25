/* $Id: Attrs.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */

#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>

class AgentController : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};


double AgentController::onAction(ActionEvent &evt)
{
	try {

		SimObj *my = getObj(myname());

		double x = my->x();
		double y = my->y();
		double z = my->z();
		LOG_MSG(("position = (%f, %f, %f)", x, y, z));

		double qw = my->qw();
		double qx = my->qx();
		double qy = my->qy();
		double qz = my->qz();
		LOG_MSG(("quaternion = (%f, %f, %f, %f)", qw, qx, qy, qz));

		if (my->dynamics()) {
			double fx = my->fx();
			double fy = my->fy();
			double fz = my->fz();
			LOG_MSG(("force = (%f, %f, %f)", fx, fy, fz));

			double tqx = my->tqx();
			double tqy = my->tqy();
			double tqz = my->tqz();
			LOG_MSG(("torque = (%f, %f, %f)", tqx, tqy, tqz));
		}

	} catch(SimObj::NoAttributeException &) {
		
	} catch(SimObj::AttributeReadOnlyException &) {
		
	} catch(SimObj::Exception &) {
		
	}
	return 1.0; 
}

extern "C"  Controller * createController ()
{
	return new AgentController;
}

