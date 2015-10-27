/*
 *  Controller Template for SIGVerse
 */
#include <string>
#include <sigverse/commonlib/ControllerEvent.h>
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/Logger.h>

#define PI 3.141592
#define DEG2RAD(DEG) ( (PI) * (DEG) / 180.0 )

class WinController : public Controller 
{
public:
	void onInit(InitEvent &evt);
	double onAction(ActionEvent&);
	void onRecvMsg(RecvMsgEvent &evt);
	void onCollision(CollisionEvent &evt);

private:
	// local variables
};

/*
 * onInit
 */
void WinController::onInit(InitEvent &evt) 
{
	try 
	{
		SimObj *my = getObj(myname());

		if (!my->dynamics()) 
		{
			my->setJointAngle("LARM_JOINT2", DEG2RAD(-90));
			my->setJointAngle("RARM_JOINT2", DEG2RAD(+90));
		}
	}
	catch(SimObj::Exception &) 
	{
		;
	}
}


/*
 * onAction
 */
double WinController::onAction(ActionEvent &evt) 
{
	return 10.0;
}

/*
 * onRecvMsg
 */
void WinController::onRecvMsg(RecvMsgEvent &evt) 
{
	SimObj *my = getObj(myname());

	std::string msg = evt.getMsg();
	
	if (msg == "Hello")
	{
		my->setJointAngle("WAIST_JOINT1", DEG2RAD(+45));
	}
}

/*
 * onCollision
 */
void WinController::onCollision(CollisionEvent &evt) 
{
	return;
}

/*
 * Export this function
 */
extern "C" EXPORT_FUNC Controller * createController() 
{
	return new WinController;
}

