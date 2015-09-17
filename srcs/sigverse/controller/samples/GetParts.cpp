/* $Id: GetParts.cpp,v 1.2 2011-06-08 10:07:57 msi Exp $ */
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

		const char *partsName = "WAIST_LINK1";
		CParts * parts = my->getParts(partsName);
		if (parts != NULL) {

			Vector3d pos;
			parts->getPosition(pos);

			LOG_MSG(("parts(%s) : pos(%f, %f, %f)",
				 parts->name(),
				 pos.x(), pos.y(), pos.z()));

			Rotation r;
			parts->getRotation(r);

			const dReal *q = r.q();
			LOG_MSG(("parts(%s) : quaternion(%f, %f, %f)",
				 parts->name(),
				 q[0], q[1], q[2], q[3]));
		}
		else
		{
			LOG_MSG(("(%s) not found", partsName));
		}

	} catch(SimObj::Exception &) {


	}
	return 1.0; 
}

extern "C"	Controller * createController ()
{
	return new AgentController;
}

