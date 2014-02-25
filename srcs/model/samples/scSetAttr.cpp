#include "Controller.h"
#include "ControllerEvent.h"
#include "Logger.h"
#include <string>
#include <vector>

class SetAttrController : public Controller
{
public:
	void onInit(InitEvent &evt);
	double onAction(ActionEvent &evt);
	void onRecvMessage(RecvMessageEvent &evt);

private:
	bool parseVector3(const char *str, double &x, double &y, double &z);
};

void SetAttrController::onInit(InitEvent &evt)
{
}

double SetAttrController::onAction(ActionEvent &evt)
{
	return 1.0;
}

bool SetAttrController::parseVector3(const char *str, double &x, double &y, double &z)
{
	x = y = z = 0.0;

	if (!str) return false;

	char *buf = (char *)strdup(str);
	if (!buf) return false;

	int c = 0;
	char *token = strtok(buf, ",");
	while (token)
	{
	switch (c)
	{
	case 0:
		x = atof(token);
		break;

	case 1:
		y = atof(token);
		break;

	case 2:
		z = atof(token);
		break;
	}

	c++;
	token = strtok(NULL, ",");
	}

	free(buf);
	return true;

error_return:
	free(buf);
	return false;
}

void SetAttrController::onRecvMessage(RecvMessageEvent &evt)
{
	double x, y, z;

	LOG_MSG(("message from : %s", evt.getSender()));
	int n = evt.getSize();
	for (int i=0; i<n; i++) {
		LOG_MSG(("[%d] (%s)", i, evt.getString(i)));
	}

	SimObj *o = getObj(myname());

	if (n>0 && o && !o->dynamics()) {
		const char *cmd = evt.getString(0);
		if (cmd) {
			if (strcmp(cmd, "p")==0) {
				// ----------------------------
				//	position set (x, y, z)
				// ----------------------------
				const char *param = evt.getString(1);
				if (param) {
					LOG_MSG(("set position (param=%s)", param));
					if (parseVector3(param, x, y, z)) {
						LOG_MSG(("(%f, %f, %f)", x, y, z));
						o->setPosition(x, y, z);
					}
				}
			} else if (strcmp(cmd, "vp")==0) {
				// ----------------------------
				//	view pos
				// ----------------------------
				const char *param = evt.getString(1);
				if (param) {
					LOG_MSG(("set view position (param=%s)", param));
					if (parseVector3(param, x, y, z)) {
						LOG_MSG(("(%f, %f, %f)", x, y, z));
						o->vpx(x);
						o->vpy(y);
						o->vpz(z);
					}
				}
			} else if (strcmp(cmd, "vv")==0) {
				// ----------------------------
				//	view vector
				// ----------------------------
				const char *param = evt.getString(1);
				if (param) {
					LOG_MSG(("set view vector (param=%s)", param));
					if (parseVector3(param, x, y, z)) {
						LOG_MSG(("(%f, %f, %f)", x, y, z));
						o->vvx(x);
						o->vvy(y);
						o->vvz(z);
					}
				}
			} else if (strcmp(cmd, "lep")==0) {
				// ----------------------------
				//	left eye pos
				// ----------------------------
				const char *param = evt.getString(1);
				if (param) {
					LOG_MSG(("set left eye position (param=%s)", param));
					if (parseVector3(param, x, y, z)) {
						LOG_MSG(("(%f, %f, %f)", x, y, z));
						o->lepx(x);
						o->lepy(y);
						o->lepz(z);
					}
				}
			} else if (strcmp(cmd, "lev")==0) {
				// ----------------------------
				//	left eye vector
				// ----------------------------
				const char *param = evt.getString(1);
				if (param) {
					LOG_MSG(("set left eye vector (param=%s)", param));
					if (parseVector3(param, x, y, z)) {
						LOG_MSG(("(%f, %f, %f)", x, y, z));
						o->levx(x);
						o->levy(y);
						o->levz(z);
					}
				}
			} else if (strcmp(cmd, "rep")==0) {
				// ----------------------------
				//	right eye pos
				// ----------------------------
				const char *param = evt.getString(1);
				if (param) {
					LOG_MSG(("set right eye position (param=%s)", param));
					if (parseVector3(param, x, y, z)) {
						LOG_MSG(("(%f, %f, %f)", x, y, z));
						o->repx(x);
						o->repy(y);
						o->repz(z);
					}
				}
			} else if (strcmp(cmd, "rev")==0) {
				// ----------------------------
				//	right eye vector
				// ----------------------------
				const char *param = evt.getString(1);
				if (param) {
					LOG_MSG(("set right eye vector (param=%s)", param));
					if (parseVector3(param, x, y, z)) {
						LOG_MSG(("(%f, %f, %f)", x, y, z));
						o->revx(x);
						o->revy(y);
						o->revz(z);
					}
				}
			}
		}
	}
}

extern "C"	Controller * createController ()
{
	return new SetAttrController;
}
