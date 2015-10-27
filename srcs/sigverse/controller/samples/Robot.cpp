/* $Id: Robot.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */ 
#include <sigverse/commonlib/Controller.h>
#include <sigverse/commonlib/modelerr.h>
#include <sigverse/commonlib/ControllerEvent.h>
#include <sigverse/commonlib/SimObj.h>
#include <sigverse/commonlib/Logger.h>
#include <sigverse/commonlib/Vector3d.h>
#include <sigverse/commonlib/ViewImage.h>
#include <math.h>

class RobotController : public Controller
{
public:
	double onAction(ActionEvent &evt);
};

#define ARRARY_SIZE(ARY) (int)(sizeof(ARY)/sizeof(ARY[0]))

double RobotController::onAction(ActionEvent &evt)
{
	LOG_MSG(("\ncurrent time : %f", evt.time()));

	static int cnt = 0;

	try {
		const char *name = myname();
		SimObj *my = getObj(name);
		if (my) {

			double x = my->x();
			double y = my->y();
			double z = my->z();
			LOG_MSG(("dynamics = %s", my->dynamics()? "true": "false"));
			LOG_MSG(("P (%f, %f, %f)", my->x(), my->y(), my->z()));
			LOG_MSG(("F (%f, %f, %f)", my->fx(), my->fy(), my->fz()));
			LOG_MSG(("Q (%f, %f, %f, %f)", my->qw(), my->qx(), my->qy(), my->qz()));

			if (!my->dynamics()) {
				double angle = 2*M_PI*cnt*10/360;
				double xx = 5.0*sin(angle);
				double yy = 1.1;
				double zz = 5.0*cos(angle);

				Vector3d vv(xx, yy, zz);
				my->setPosition(vv);

				my->setAxisAndAngle(0.0, 1.0, 0.0, angle);


				Rotation r;
				r.setQuaternion(my->qw(), my->qx(), my->qy(), my->qz());
		
				SimObj *target = getObj("Robot2");
				if (target) {
					Vector3d v(3.0, 0.0, 0.0);
					Vector3d rv;
					r.apply(v, rv);
					//		v.rotate(r);
					rv  += vv;

					target->setPosition(rv);
					Rotation targetR;
					targetR.setAxisAndAngle(0.0, 1.0, 0.0, 2*angle);
					targetR *=r;

					target->setRotation(targetR);
				}
			}
		}

		
	} catch(SimObj::NoAttributeException &) {
		
	} catch(SimObj::AttributeReadOnlyException &) {
		
	} catch(SimObj::Exception &) {
		
	}

	cnt++;
	return 0.1;
}
		
extern "C"  Controller * createController ()
{
	return new RobotController;
}

