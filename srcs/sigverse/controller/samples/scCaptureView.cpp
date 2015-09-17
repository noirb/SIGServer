#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/Logger.h>
#include <sigverse/ViewImage.h>
#include <math.h>

class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
	void onRecvMessage(RecvMessageEvent &evt);
};

double RobotController::onAction(ActionEvent &evt)
{
	static int count = 0;

	try
	{
		SimObj *o = getObj(myname());

		if (o && !o->dynamics())
		{
			double deg = count * 10.0;
			double rad = 3.141592/180*deg;

			o->setAxisAndAngle(0.0, 1.0, 0.0, rad);
		}
	} catch(SimObj::NoAttributeException &) {
	} catch(SimObj::AttributeReadOnlyException &) {
	} catch(SimObj::Exception &) {
	}

	count++;

	return 2.0;
}

void RobotController::onRecvMessage(RecvMessageEvent &evt)
{
	int n = evt.getSize();

	if (n>0)
	{
		std::string cmd = evt.getString(0);
		LOG_MSG(("cmd : %s", cmd.c_str()));

		if (strcmp(cmd.c_str(), "capture") == 0)
		{
			ViewImage *img = captureView(COLORBIT_ANY, IMAGE_320X240);
			if (img)
			{
				LOG_MSG(("(%d, %d)", img->getWidth(), img->getHeight()));

				// save as Windows BMP format
				img->saveAsWindowsBMP("view.bmp");

				delete img;
			}
		}
	}
}

extern "C" Controller * createController ()
{
	return new RobotController;
}

