/*
 * Modified by okamoto on 2011-03-25
 */

#include <sigverse/Controller.h>
#include <sigverse/ControllerEvent.h>
#include <sigverse/SimObj.h>



class NiiManOkonomi : public Controller
{
public:
	double onAction(ActionEvent &evt);
};


enum {
	man_hip = 0, man_body, man_head,
	
	man_leftarm0, man_leftarm1, man_leftarm2, man_lefthand,
	man_leftfinter1, man_leftfinter2, 

	man_rightarm0, man_rightarm1, man_rightarm2,	man_righthand,
	man_rightfinter1, man_rightfinter2, 

	man_leftleg1, man_leftleg2, man_leftleg3, man_leftleg4,
	
	man_rightleg1, man_rightleg2, man_rightleg3, man_rightleg4, 

	JOINT_NUM,
};

static char *jointNames[] = {
	"WAIST_JOINT0", "WAIST_JOINT1", "HEAD_JOINT0",

	"LARM_JOINT2", "LARM_JOINT1", "LARM_JOINT4", "LARM_JOINT5", NULL, NULL,
	"RARM_JOINT2", "RARM_JOINT1", "RARM_JOINT4", "RARM_JOINT5", NULL, NULL,

	"LLEG_JOINT2", "LLEG_JOINT4", "LLEG_JOINT6", NULL,
	"RLEG_JOINT2", "RLEG_JOINT4", "RLEG_JOINT6", NULL,
};

static double step01[] = {
	16.0, 20.0, 0.0,
	
	0.0, -91.0, 0.0, 0.0, 0.0, 0.0,
	90.0, 0.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

static double step02[] = {
	-23.0, 12.0, 0.0,

	-85.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 87.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

#define step05 step02

static double step03[] = {
	0.0, 12.0, 10.0,

	-85.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 80.0, 10.0, 50.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

#define step06 step03

static double step04[] = {
	0.0, 20.0, 1.72,

	-85.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 81.0, 9.28, 50.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

#define step07 step04

static double step08[] = {
	0.0, 20.0, -6.0,

	0.0, -92.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 92.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

static double step09[] = {
	0.0, 15.0, 5.0,

	0.0, -87.0, 4.13, 0.0, 0.0, 0.0,
	0.0, 87.0, 4.13, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

static double step10[] = {
	0.0, 11.0, -6.0,

	0.0, -82.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 82.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};


static double step11[] = {
	24.0, 15.0, 0.0,

	0.0, -93.0, 0.0, 0.0, 0.0, 0.0,
	85.0, 0.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

#define step14 step11
#define step17 step11
#define step20 step11

static double step12[] = {
	0.0, 15.0, 0.0,

	0.0, -85.0, 0.0, 37.0, 0.0, 0.0,
	85.0, 0.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

#define step15 step12
#define step18 step12
#define step21 step12

static double step13[] = {
	0.0, 15.0, 5.0,

	0.0, -88.0, 0.0, 100.0, 0.0, 0.0,
	85.0, 0.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

#define step16 step13
#define step19 step13
#define step22 step13

static double step23[] = {
	0.0, 10.0, 5.0,

	0.0, -90.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 90.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

static double step24[] = {
	-50.0, 24.0, -9.543,

	0.0, -104.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 90.0, 0.0, 0.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};

static double step25[] = {
	0.0, 0.0, 0.0, 

	0.0, -73.855, -14.0, -26.0, 0.0, 0.0,
	0.0, 73.855, 14.0, -26.0, 0.0, 0.0,

	-90.0, 85.0, 16.0, 0.0, 
	-90.0, 85.0, 16.0, 0.0, 
};


static double * jointAngles[] = {
	step01, step02, step03, step04, step05, 
	step06, step07, step08, step09, step10, 
	step11, step12, step13, step14, step15, 
	step16, step17, step18, step19, step20, 
	step21, step22, step23, step24, step25, 
};

#define ARY_SIZE(ARY) (sizeof(ARY)/sizeof(ARY[0]))

#define DEG2RAD(D) ( 3.1415926536*(D)/180.0)

double NiiManOkonomi::onAction(ActionEvent &evt)
{

	SimObj *my = getObj(myname());

	static int s_cnt = 0;
	int N = ARY_SIZE(jointAngles);
	int step = s_cnt % N;
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

	return 0.5;
}

extern "C"  Controller * createController ()
{
	return new NiiManOkonomi;
}


