/* $Id: GetJointForce.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>
#include <sigverse/JointForce.h>


class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

double RobotController::onAction(ActionEvent &evt)
{
	SimObj *obj = getObj(myname());
	if (obj) {
		JointForce jf1, jf2;
		if (obj->getJointForce("R_ELBOW", jf1, jf2)) {
			const Vector3d &f1 = jf1.getForce();
			const Vector3d &tq1 = jf1.getTorque();
			LOG_MSG(("jf1(%s), F=(%f, %f, %f), torque(%f, %f, %f)",
				 jf1.getPartsName(),
				 f1.x(), f1.y(), f1.z(),
				 tq1.x(), tq1.y(), tq1.z()
				 ));

			const Vector3d &f2 = jf2.getForce();
			const Vector3d &tq2 = jf2.getTorque();
			LOG_MSG(("jf2(%s), F=(%f, %f, %f), torque(%f, %f, %f)",
				 jf2.getPartsName(),
				 f2.x(), f2.y(), f2.z(),
				 tq2.x(), tq2.y(), tq2.z()
				 ));
		}
	}

	return 2.0;
}

extern "C"  Controller * createController ()
{
	return new RobotController;
}

