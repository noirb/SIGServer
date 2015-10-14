#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/ControllerEvent.h>
#include <sigverse/commonlib/Logger.h>

class AgentController: public Controller {
public:
	double onAction(ActionEvent &evt);
	void   onCollision(CollisionEvent &evt);
};


void AgentController::onCollision(CollisionEvent &evt)
{
	LOG_MSG(("%s : onCollision() ", myname()));

	const std::vector<std::string> & with = evt.getWith();

	for (std::vector<std::string>::const_iterator i=with.begin(); i!=with.end(); i++) {
		std::string s = *i;
		LOG_MSG(("\"%s\"", s.c_str()));
	}
}

double AgentController::onAction(ActionEvent &evt) {

	try {
		
	} catch (SimObj::NoAttributeException &) {
	} catch (SimObj::AttributeReadOnlyException &) {
	} catch (SimObj::Exception &) {
	}

	return 1.0;
}

extern "C" Controller * createController() {
	return new AgentController;
}

