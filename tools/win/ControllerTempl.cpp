/*
 *  Controller Template for SIGVerse
 *
 */
#include <sigverse/commonlib/ControllerEvent.h>
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/Logger.h>

class MyController : public Controller 
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
void MyController::onInit(InitEvent &evt) 
{
	return;
}


/*
 * onAction
 */
double MyController::onAction(ActionEvent &evt) 
{
	return 1.0;
}

/*
 * onRecvMsg
 */
void MyController::onRecvMsg(RecvMsgEvent &evt) 
{
	return;
}

/*
 * onCollision
 */
void MyController::onCollision(CollisionEvent &evt) 
{
	return;
}

/*
 * Export this function
 */
extern "C" EXPORT_FUNC Controller * createController() 
{
	return new MyController;
}

