/* $Id: NiiRobotOkonomi.cpp,v 1.1.1.1 2011-03-25 02:18:46 okamoto Exp $ */
#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/SimObj.h>



class NiiRobotOkonomi : public Controller
{
public:
	double onAction(ActionEvent &evt);
};


const int STEP_NUM = 8;
enum {
	robo_body = 0, robo_chest, robo_face, robo_hip,
	
	robo_leftarm1,	robo_leftarm2,	robo_leftfood,	robo_lefthand1, robo_lefthand2,
	robo_leftleg1,	robo_leftleg2,

	robo_rightarm1,	robo_rightarm2,	robo_rightfood,	robo_righthand1,
	robo_righthand2, robo_rightleg1, robo_rightleg2,
	JOINT_NUM,
};


static char *jointNames[] = {
	"WAIST_JOINT2", "CHEST", "HEAD_JOINT0", "WAIST_JOINT0",

	"LARM_JOINT1", "LARM_JOINT2", "LARM_JOINT4", NULL, NULL,
	"LLEG_JOINT2", "LLEG_JOINT4",

	"RARM_JOINT1", "RARM_JOINT2", "RARM_JOINT4", NULL, NULL,
	"RLEG_JOINT2", "RLEG_JOINT4",
};


static double jointAngles[STEP_NUM][JOINT_NUM] = {
	// STEP00
	{ 0, 0, 0, 0,
	  0, 0, 0, 0, 0,
	 -90, 90,
	  0, 0, 0, 0, 0,
	 -90, 90 },

	// STEP01
	{ 0, 0, 0, 0,
	  0, 0, 0, 0, 0,
	 -90, 90,
	  0, 0, 0, 0, 0,
	 -90, 90 },

	// STEP02
	{ 25, 0, 0, 0,
	 -50, -41, 0, 47, -56,
	 -90, 90,
	  0, 0, 0, 0, 0,
	 -90, 90 },

	// STEP03
	{ 0, 0, 0, 0,
	 -71, -12, 0, 47, -56,
	 -90, 90,
	  0, 0, 0, 0, 0,
	 -90, 90 },

	// STEP04
	{ 0, 0, 0, 0,
	 -85, -44, 0, 47, -56,
	 -90, 90,
	  0, 0, 0, 0, 0,
	 -90, 90 },

	// STEP05
	{ 0, 14, 0, 0,
	  -84, -11, 0, 0, 0,
	 -90, 90,
	  -84, 11, 0, 0, 0,
	 -90, 90 },

	// STEP06
	{ -54, 0, 10, 0,
	  -93, -29, 0, 3, -4,
	  -90, 90,
	  -93, 21, 0, -3, 4,
	 -90, 90 },

	// STEP07
	{0, 0, 5, -10,
	 -76, -24, 0, 3, -4,
	 -80, 90,
	 -76,  24, 0, -3, 4,
	 -80, 90 },
};

#define DEG2RAD(D) ( 3.1415926536*(D)/180.0)

double NiiRobotOkonomi::onAction(ActionEvent &evt)
{

	SimObj *my = getObj(myname());

	static int s_cnt = 0;

	int step = s_cnt % STEP_NUM;
	for (int i=0; i<JOINT_NUM; i++) {
		const char * jointName = jointNames[i];
		if (!jointName) { continue; }
		double angle = jointAngles[step][i];
		double rad = DEG2RAD(angle);
		my->setJointAngle(jointName, rad); 
	}
	s_cnt++;

	/*
	double zz = cos(s_cnt*5*3.14/180);
	double xx = sin(s_cnt*5*3.14/180);
	my->setPosition(xx, 0.5, zz);

	my->setAxisAndAngle(0.0, 1.0, 0.0, s_cnt*5*3.14/180);
	*/

	return 0.1;
}

extern "C"  Controller * createController ()
{
	return new NiiRobotOkonomi;
}


