/* $Id: onAction.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */

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
	double t = evt.time();
	
	try {
		SimObj *my = getObj(myname());


		SimObj *obj = getObj("Robot1");

	} catch(SimObj::NoAttributeException &) {
		//
		// SimObj::NoAttributeException
		//
		
		
	} catch(SimObj::AttributeReadOnlyException &) {
		//
		// SimObj::AttributeReadOnlyException
		//

		
	} catch(SimObj::Exception &) {
		
	}
	return 1.0; 
}

extern "C"  Controller * createController ()
{
	return new AgentController;
}

