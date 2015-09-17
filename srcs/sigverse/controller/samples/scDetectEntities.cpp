#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/Logger.h>
#include <string>
#include <vector>

class DetectEntitiesController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

double DetectEntitiesController::onAction(ActionEvent &evt)
{
	std::vector<std::string> agents;
	std::vector<std::string>::iterator i;
	static int count = 0;

	//	LOG_MSG(("bear onAction(count=%d)\n", count));

	bool b = detectEntities(agents);
	if (b)
	{
		int n = agents.size();
		if (n>0)
		{
			LOG_MSG(("%d entities detected", n));
			for (int i=0; i<n; i++)
			{
				std::string name = agents[i];
				LOG_MSG(("[%d] (%s)", i, name.c_str()));
			}
		}
	}
	else
	{
		LOG_MSG(("detectEntities failed"));
	}

	try {
		SimObj *o = getObj(myname());
		if (o && !o->dynamics())
		{
			double deg = count * 10.0;
			double rad = -3.141592/180*deg;

			o->setAxisAndAngle(0.0, 1.0, 0.0, rad);
		}
	} catch(SimObj::NoAttributeException &) {
	} catch(SimObj::AttributeReadOnlyException &) {
	} catch(SimObj::Exception &) {
	}

	count++;

	return 2.0;
}

extern "C"	Controller * createController ()
{
	return new DetectEntitiesController;
}
