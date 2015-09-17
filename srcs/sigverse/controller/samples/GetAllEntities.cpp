#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/Logger.h>

class GetAllEntities : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};


double 	 GetAllEntities::onAction(ActionEvent &evt)
{
	typedef std::vector<std::string> C;
	C agents;
	if (getAllEntities(agents)) { 
		LOG_MSG(("detect %d entities", agents.size()));
		int cnt = 0;
		for (C::iterator i=agents.begin(); i!=agents.end(); i++) {
			std::string name = *i;
			LOG_MSG(("[%d] : %s", cnt, name.c_str()));
			cnt++;
		}
	} else { 
		LOG_MSG(("get no entity"));
	}
}

extern "C"  Controller * createController ()
{
	return new GetAllEntities;
}

