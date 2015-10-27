#include <string>
#include <vector>

#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/ControllerEvent.h>
#include <sigverse/commonlib/SimObj.h>
#include <sigverse/commonlib/Logger.h>

 

class Watcher : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

double Watcher::onAction(ActionEvent &evt)
{
	static int cnt = 0;
	
	try {
		SimObj *my = getObj(myname());

		if (!my || my->dynamics()) { return 5.0; }

		typedef std::vector<std::string> C;
		C names;
		bool detected = detectEntities(names);

		if (detected) { 
			LOG_MSG(("detected %d", names.size()));
			for(C::iterator i=names.begin(); i!=names.end(); i++) {
				std::string name = *i;
				LOG_MSG(("%s detected", name.c_str()));
				sendText(evt.time(), NULL, name.c_str());
			}
		} else {
			LOG_MSG(("not detected"));
		}

		const double R = 5.0;
		double angle = 2*M_PI*cnt*0.01;
		double xx = R*sin(angle);
		double yy = my->y();
		double zz = R*cos(angle);
		
		LOG_MSG(("pos (%f, %f, %f)", xx, yy, zz));
		my->setPosition(xx, yy, zz);
		my->setAxisAndAngle(0.0, 1.0, 0.0, angle+M_PI/2);

		
	} catch(SimObj::Exception &e) {
		LOG_ERR(("%s : ERROR %s", myname(), e.msg()));
	}

	cnt++;
	return 0.1; 
}

extern "C"  Controller * createController ()
{
	return new Watcher;
}

