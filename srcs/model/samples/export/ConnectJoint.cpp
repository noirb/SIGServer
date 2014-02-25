/* $Id: ConnectJoint.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/SimObj.h>

class AgentController : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};


double AgentController::onAction(ActionEvent &evt)
{
	SimObj *my = getObj(myname());
	if (my) {

		const char *jointName = "newjoint";
		my->connectJoint(jointName, "RARM", "Desk", "body");

	}

	return 1.0; 
}

extern "C"  Controller * createController ()
{
	return new AgentController;
}

