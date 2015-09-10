#include <sigverse/Controller.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>


class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

double RobotController::onAction(ActionEvent &evt)
{
	try {
		typedef std::vector<std::string> C;
		C agents;

		bool b = detectEntities(agents);
		if (b && agents.size() > 0) {
			for (C::iterator i=agents.begin(); i!=agents.end(); i++) {
				std::string name = *i;
				LOG_MSG(("detected %s\n", name.c_str()));
				SimObj *obj = getObj(name.c_str());
				if (!obj) {
					LOG_ERR(("cannot get object"));
				}
				
				// ...;

			}

		} else {
			LOG_MSG(("no object in front of me"));
		}
		
	} catch(SimObj::Exception &) {
		LOG_ERR(("Exception"));
	}

	return 2.0;
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}

