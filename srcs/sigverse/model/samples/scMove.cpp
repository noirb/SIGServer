#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/modelerr.h>
#include <sigverse/SimObj.h>
#include <sigverse/Logger.h>
#include <math.h>
#include <string>

class MoveController : public Controller
{
public:
	enum STATE
	{
		IDLE,
		WALKING
	};

	double onAction(ActionEvent &evt);
	void onRecvMessage(RecvMessageEvent &evt);
	STATE getState() { return m_state; }
	void setState(STATE state) { m_state = state; }

private:
	STATE m_state;
	double m_xDest, m_yDest, m_zDest;
};

#define PI 3.14159265359

double MoveController::onAction(ActionEvent &evt)
{
	static int cnt = 0;

	try {
		const char *name = myname();
		SimObj *obj = getObj(name);
		if (obj && !obj->dynamics()) {

			// my position
			double x = obj->x();
			double y = obj->y();
			double z = obj->z();

			switch (m_state)
			{
				case IDLE:
					break;

				case WALKING:
				{
					// calc direction to target
					double xDir = m_xDest - x;
					double yDir = m_yDest - y;
					double zDir = m_zDest - z;

					double distance = sqrt(xDir*xDir+yDir*yDir+zDir*zDir);

					if (fabs(distance) < 0.000001) {
						LOG_MSG(("*** arrived to destination(%f %f %f) ***", m_xDest, m_yDest, m_zDest));
						setState(IDLE);
						break;
					}
					if (fabs(distance) < 1) {
						// calc new position
						double speed = 0.5;
						x = x + (xDir * speed);
						y = y + (yDir * speed);
						z = z + (zDir * speed);
					} else {
						// normalize direction (only when distance >= 1)
						xDir = xDir / distance;
						yDir = yDir / distance;
						zDir = zDir / distance;

						// calc new position
						double speed = 0.5;
						x = x + (xDir * speed);
						y = y + (yDir * speed);
						z = z + (zDir * speed);
					}

					// set new position and angle
					obj->setPosition(x, y, z);
					LOG_MSG(("pos(%f %f %f) distance(%f)", x, y, z, distance))
				}
				break;
			}
		}
	} catch(SimObj::NoAttributeException &) {
	} catch(SimObj::AttributeReadOnlyException &) {
	} catch(SimObj::Exception &) {
	}

	cnt++;

	return 0.1;
}

void MoveController::onRecvMessage(RecvMessageEvent &evt)
{
	const char *name = myname();
	SimObj *obj = getObj(name);

	int n = evt.getSize();
	LOG_MSG(("n = %d", n));

	if (n > 0) {
		//	  std::string receiver = evt.getString(0);
		std::string cmd = evt.getString(0);
		LOG_MSG(("cmd = %s", cmd.c_str()));

		if (strcmp(cmd.c_str(), "move")==0) {
			// -----------------------------------------
			//  move to destination point
			// -----------------------------------------
			double x = atof(evt.getString(1));
			double y = atof(evt.getString(2));
			double z = atof(evt.getString(3));
			LOG_MSG(("goto (%f, %f, %f)", x, y, z));

			// set destination point
			m_xDest = x;
			m_yDest = y;
			m_zDest = z;

			// set new state
			m_state = WALKING;
		}
		else if (strcmp(cmd.c_str(), "print")==0) {
			if (obj) {
				double vpx = obj->vpx();
				double vpy = obj->vpy();
				double vpz = obj->vpz();

				double vvx = obj->vvx();
				double vvy = obj->vvy();
				double vvz = obj->vvz();

				LOG_MSG(("vp(%f, %f, %f) vv(%f, %f, %f)", vpx, vpy, vpz, vvx, vvy, vvz));
			}
		}
	}
}

extern "C" Controller * createController ()
{
	return new MoveController;
}
