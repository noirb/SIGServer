#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/Logger.h>
#include <string>
#include <vector>

class TVController : public Controller
{
public:
	void onInit(InitEvent &evt);
	double onAction(ActionEvent &evt);
	void onRecvMessage(RecvMessageEvent &evt);

private:
	bool m_on;
	bool m_blinking;
	int m_rate;

	void setAttrToCurrState(SimObj *o);
};

void TVController::onInit(InitEvent &evt)
{
	m_on = true;
	m_blinking = false;
	m_rate = 30;
}

double TVController::onAction(ActionEvent &evt)
{
	static int cnt = 0;

	try {
		SimObj *o = getObj(myname());

		if (o && !o->dynamics()) {

			if (m_blinking) {
				// process TV blinking
				cnt++;
				if (cnt > m_rate) {
					cnt = 0;

					// toggle state of TV
					m_on = m_on ? false : true;
				}
			}
			// update attr value to current state
			setAttrToCurrState(o);
		}
	} catch(SimObj::NoAttributeException &) {
	} catch(SimObj::AttributeReadOnlyException &) {
	} catch(SimObj::Exception &) {
	}

	return 1.0;
}

void TVController::onRecvMessage(RecvMessageEvent &evt)
{
	LOG_MSG(("message from : %s", evt.getSender()));
	int n = evt.getSize();

	for (int i=0; i<n; i++) {
		LOG_MSG(("[%d] (%s)", i, evt.getString(i)));
	}

	if (n>0) {
		const char *cmd = evt.getString(0);

		if (cmd) {
			if (strcmp(cmd, "on")==0) {
				// ----------------------------
				//  TV switch on
				// ----------------------------
				m_on = true;
			} else if (strcmp(cmd, "off")==0) {
				// ----------------------------
				//  TV switch off
				// ----------------------------
				m_on = false;
			} else if (strcmp(cmd, "blink_start")==0) {
				// ----------------------------
				//  TV blinking start
				// ----------------------------
				m_blinking = true;
				if (n>1) {
					const char *rate = evt.getString(1);

					if (rate) {
						m_rate = atoi(rate);
					}
				}
			} else if (strcmp(cmd, "blink_stop")==0) {
				// ----------------------------
				//  TV blinking stop
				// ----------------------------
				m_blinking = false;
			}
		}
	}
}

void TVController::setAttrToCurrState(SimObj *o)
{
	o->setAttrValue("switch", m_on ? "on" : "off");
}

extern "C"  Controller * createController ()
{
	return new TVController;
}
