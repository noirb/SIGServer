/* $Id: Capture.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/Controller.h>
#include <sigverse/Logger.h>
#include <sigverse/ViewImage.h>
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


