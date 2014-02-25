#include "Controller.h"
#include "ControllerEvent.h"
#include "Logger.h"

class AgentController: public Controller {
public:
	double	onAction(ActionEvent &evt);
	void	onCollision(CollisionEvent &evt);
};


void AgentController::onCollision(CollisionEvent &evt) {
	LOG_MSG(("%s : onCollision() ", myname()));
	typedef CollisionEvent::WithC C;
	const C & with = evt.getWith();
	for (C::const_iterator i=with.begin(); i!=with.end(); i++) {
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

