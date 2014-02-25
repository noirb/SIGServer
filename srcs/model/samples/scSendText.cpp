/* $Id: scSendText.cpp,v 1.2 2011-05-12 08:33:53 msi Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/Logger.h>


class RobotController : public Controller
{
public:
	void onInit(InitEvent &evt);
	double onAction(ActionEvent &evt);
	void onRecvMessage(RecvMessageEvent &evt);
	void onRecvText(RecvTextEvent &evt);
};

void RobotController::onInit(InitEvent &evt)
{
	LOG_MSG(("controller started"));
}

double RobotController::onAction(ActionEvent &evt)
{
	return 1.0;
}

void RobotController::onRecvMessage(RecvMessageEvent &evt)
{
	int n = evt.getSize();

	if (n>0)
	{
		std::string strCmd = evt.getString(0);
		const char *cmd = strCmd.c_str();
		LOG_MSG(("cmd : %s", cmd));

		if (strcmp(cmd, "text") == 0)
		{
			std::string strTarget = evt.getString(1);
			const char *target = strTarget.c_str();
			LOG_MSG(("target : (%s)", target));

			std::string strText = evt.getString(2);
			const char *text = strText.c_str();
			LOG_MSG(("text : (%s)", text));

			sendText(0, target, text);
		}
	}
}

void RobotController::onRecvText(RecvTextEvent &evt)
{
	const char *text = evt.getText();

	const char *caller = evt.getCaller();

	LOG_MSG(("received (%s) from (%s)", text, caller));
}

extern "C" Controller *createController()
{
	return new RobotController;
}

