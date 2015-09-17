/* $Id: SetPosition.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/SimObj.h>


class AgentController : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};

#define PI 3.1415926536
#define DEG2RAD(DEG) ( (PI)*(DEG)/180.0 )

double AgentController::onAction(ActionEvent &evt)
{
	try {
		SimObj *my = getObj(myname());

		
		if (!my->dynamics()) { 

			my->setPosition(1.0, 2.0, 3.0);
			my->setAxisAndAngle(0, 1.0, 0, DEG2RAD(45));
		} 

	} catch(SimObj::Exception &) {
		;
	}
	return 5.0;
}

extern "C"  Controller * createController ()
{
	return new AgentController;
}


