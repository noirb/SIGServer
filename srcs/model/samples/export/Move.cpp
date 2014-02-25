/* $Id: Move.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 


#ifdef DEPRECATED
#include <sigverse/Controller.h>

class RobotController : public Controller
{
public:
	double onAction(ActionEvent&);
};


double RobotController::onAction(ActionEvent &evt)
{
	moveTo(5, 5, 3);
	
	return 5.0;
}
		
extern "C"  Controller * createController ()
{
	return new RobotController;
}

#endif // DEPRECATED

