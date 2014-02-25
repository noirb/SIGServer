/* $Id: JointTest.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/Controller.h>
#include <sigverse/modelerr.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>
#include <math.h>

class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

static double joint_angles[][20] = {
#include "motion.cpp"
};

static char *joint_names[] = {
	0, 0, "RLEG_JOINT3", "RLEG_JOINT4", "RLEG_JOINT5", 
	0, "RARM_JOINT0", 0, 0, "RARM_JOINT3",
	0, 0, "LLEG_JOINT3", "LLEG_JOINT4", "LLEG_JOINT5",
	0, "LARM_JOINT0", 0, 0, 0,
};

/*
static char *joint_names[] = {
	0, 0, 0, "RLEG_JOINT4", 0,
	0, 0, 0, 0, 0,
	0, 0, 0, "LLEG_JOINT4", 0,
	0, 0, 0, 0, 0,
};
*/
static int curr = 0;

#define ARRAY_SIZE(ARY) (int)(sizeof(ARY)/sizeof(ARY[0]))

#define PI 3.14159265359

#define DEG2RAD(DEG) ( ( (DEG)*(PI) ) / 180.0 )

double RobotController::onAction(ActionEvent &evt)
{
	LOG_MSG(("\ncurrent time : %f", evt.time()));

	try {
		if (curr < ARRAY_SIZE(joint_angles)) {
			const char *name = myname();
			SimObj *my = getObj(name);
			if (!my) { return 10.0; }

			for (int i=0; i<ARRAY_SIZE(joint_names); i++) {
				const char *jname = joint_names[i];
				if (!jname) { continue; }
				double v = joint_angles[curr][i];
				my->setJointAngle(jname, v);
			}
			curr++;
		}
		
	} catch(SimObj::NoAttributeException &) {
		
	} catch(SimObj::AttributeReadOnlyException &) {
		
	} catch(SimObj::Exception &) {
		
	}
	return 0.5;
}
		
extern "C"  Controller * createController ()
{
	return new RobotController;
}

