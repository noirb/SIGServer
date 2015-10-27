/* $Id: Capture.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/Logger.h>
#include <sigverse/commonlib/ViewImage.h>
#include <math.h>


class RobotController : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};
 

double RobotController::onAction(ActionEvent &evt)
{
	ViewImage *img = captureView(COLORBIT_ANY, IMAGE_320X240);
	if (img) {
		LOG_MSG(("(%d, %d)", img->getWidth(), img->getHeight()));

		int len = img->getBufferLength();
		char *buf =  img->getBuffer();
		// ...

		img->saveAsWindowsBMP("view.bmp");
		delete img;
	}

	return 2.0;
}
		
extern "C"  Controller * createController ()
{
	return new RobotController;
}


