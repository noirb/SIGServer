/* $Id: MovingArm.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>


class AgentController : public Controller
{
public:
	double 	onAction(ActionEvent &evt);
};

#define PI 3.141592

#define DEG2RAD(DEG) ( (PI) * (DEG) / 180.0 )


double AgentController::onAction(ActionEvent &evt)
{
	try {
		static int deg = 0;
		SimObj *my = getObj(myname());
		Vector3d v;
		my->getPosition(v);
		LOG_MSG(("pos = (%f, %f, %f)", v.x(), v.y(), v.z()));
		//my->setJointAngle("R_SHOULDER", DEG2RAD(deg));
		my->setJointAngle("RARM_JOINT2", DEG2RAD(deg));
		//my->setJointAngle("RARM_JOINT2", DEG2RAD(90));
		my->setJointAngle("RARM_JOINT4", DEG2RAD(90));
		//my->setJointAngle("R_SHOULDER", DEG2RAD(deg));
		Parts *p = my->getParts("RU_ARM");
		if (p) {
			const double *pos = p->getPosition();;
			LOG_MSG(("RU_ARM(%f, %f, %f)", pos[0], pos[1], pos[2]));
			const double *q = p->getQuaternion();
			LOG_MSG(("      (%f, %f, %f, %f", q[0], q[1], q[2], q[3]));
		}

		p = my->getParts("RL_ARM");
		if (p) {
			const double *pos = p->getPosition();;
			LOG_MSG(("RL_ARM(%f, %f, %f)", pos[0], pos[1], pos[2]));
			const double *q = p->getQuaternion();
			LOG_MSG(("      (%f, %f, %f, %f", q[0], q[1], q[2], q[3]));
		}

		deg += 45;

		static int cnt = 0;
		double angle = 2*PI*cnt*0.01;

		double xx = 5*sin(angle);
		double yy = 0.5;
		double zz = 5*cos(angle);
		
		LOG_MSG(("pos (%f, %f, %f)", xx, yy, zz));
		//my->setPosition(xx, yy, zz);

		cnt++;

		


	} catch(SimObj::Exception &) {
		;
	}
	return 0.1; 
}


extern "C"  Controller * createController ()
{
	return new AgentController;
}

