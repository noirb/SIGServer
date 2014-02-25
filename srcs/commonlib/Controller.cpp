/*
 * Written by okamoto on 2011-09-28
 *
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change setObjectVelocity to setAngularVelocityToParts
 *    English comment translation is not finished yet
 * Modified by Tetsunari Inamura on 2014-01-08
 *    Delete major magic numbers using enum for new communication protocal
 *    Delete memory leak bugs
 * Modified by Yoshiaki Mizuchi on2014-01-16
 *    Add English comments
 */

#include "Controller.h"
#include "modelerr.h"

#include "CommDataDecoder.h"
#include "CommDataEncoder.h"
#include "CommUtil.h"
#include "Source.h"
#include "SimObj.h"
#include "ControllerEvent.h"
#include "Logger.h"
#include "ct/CTReader.h"
#include "ct/CTSimObj.h"
#include "binary.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <typeinfo>



DynamicsController::DynamicsController()
{
	leftWheelName          = NULL;
	leftMotorConsumption   = 0.0;
	leftWheelRadius        = 0.0;
	leftWheelMaxSpeed      = 0.0;
	leftWheelSpeedUnit     = 0.0;
	leftSlipNoise          = 0.0;
	leftEncoderNoise       = 0.0;
	leftEncoderResolution  = 0.0;
	leftMaxForce           = 0.0;
	rightWheelName         = NULL;
	rightMotorConsumption  = 0.0;
	rightWheelRadius       = 0.0;
	rightWheelMaxSpeed     = 0.0;
	rightWheelSpeedUnit    = 0.0;
	rightSlipNoise         = 0.0;
	rightEncoderNoise      = 0.0;
	rightEncoderResolution = 0.0;
	rightMaxForce          = 0.0;
	axleLength             = 0.0;

	currentLeftWheelSpeed  = 0.0;
	currentRightWheelSpeed = 0.0;
}



/**
 * @brief Sets Webots-compatible parameters.
 * @param leftWheelName Joint name of the left wheel
 * @param leftMotorConsumption Electricity consumption of the left wheel (W)
 * @param leftWheelRadius Radius of the left wheel (m)
 * @param leftWheelMaxSpeed Maximum angular velocity of the left wheel (rad/s)
 * @param leftWheelSpeedUnit Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
 * @param leftSlipNoise Uniformly-distributed slip of the left wheel (0.0 - 1.0)
 * @param leftEncoderNoise Accumulation of slip noise for the left wheel
 * @param leftEncoderResolution Slip degree per rad for left wheel
 * @param leftMaxForce Maximum torque of the left wheel
 * @param rightWheelName Joint name of the right wheel
 * @param rightMotorConsumption Electricity consumption of the right wheel (W)
 * @param rightWheelRadius Radius of the right wheel (m)
 * @param rightWheelMaxSpeed Maximum angular velocity of the right wheel (rad/s)
 * @param rightWheelSpeedUnit Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
 * @param rightSlipNoise Uniformly-distributed slip of the right wheel (0.0 - 1.0)
 * @param rightEncoderNoise Accumulation of slip noise for right wheel
 * @param rightEncoderResolution Slip degree per rad for the right wheel
 * @param rightMaxForce Maximum torque of the right wheel
 * @param axleLength Distance between the wheels
 */

void DynamicsController::setWheelProperty(
	SimObj *my,
	char   *leftWheelName,
	double leftMotorConsumption,
	double leftWheelMaxSpeed,
	double leftWheelSpeedUnit,
	double leftSlipNoise,
	double leftEncoderResolution,
	double leftMaxForce,
	char   *rightWheelName,
	double rightMotorConsumption,
	double rightWheelMaxSpeed,
	double rightWheelSpeedUnit,
	double rightSlipNoise,
	double rightEncoderResolution,
	double rightMaxForce
)
{
	this->leftWheelName = leftWheelName;
	this->leftMotorConsumption = leftMotorConsumption;
	this->leftWheelMaxSpeed = leftWheelMaxSpeed;
	this->leftWheelSpeedUnit = leftWheelSpeedUnit;
	this->leftSlipNoise = leftSlipNoise;
	//this->leftEncoderNoise = leftEncoderNoise;//[TODO]
	this->leftEncoderResolution = leftEncoderResolution;
	this->leftMaxForce = leftMaxForce;
	this->rightWheelName = rightWheelName;
	this->rightMotorConsumption = rightMotorConsumption;
	this->rightWheelMaxSpeed = rightWheelMaxSpeed;
	this->rightWheelSpeedUnit = rightWheelSpeedUnit;
	this->rightSlipNoise = rightSlipNoise;
	//this->rightEncoderNoise = rightEncoderNoise;//[TODO]
	this->rightEncoderResolution = rightEncoderResolution;
	this->rightMaxForce = rightMaxForce;

	// Finds positon of SimObj
	//Vector3d leftWheelPos;

	// Gets the axle length when both wheels are defined
	if (leftWheelName && rightWheelName)
	{
		//Parts *pLeft = pObject->getParts(leftWheelName);
		Parts *pLeft = my->getParts(leftWheelName);
		double lx,ly,lz;
		pLeft->givePosition(lx,ly,lz);

		//Parts *pRight = pObject->getParts(rightWheelName);
		Parts *pRight = my->getParts(rightWheelName);
		double rx,ry,rz;
		pRight->givePosition(rx,ry,rz);

		axleLength = sqrt((rx * rx) + (ry * ry) + (rz * rz));
	}

	getLeftWheelRadius(my);
}

/**
 * Sets different velocity to each wheel
 * @param left Angular velocity of the left wheel
 * @param right Angular velocity of the right wheel
 */

void DynamicsController::differentialWheelsSetSpeed(SimObj *my,double left,double right)
{

	// Refect the maximum angular values of left wheel
	if (leftWheelMaxSpeed < left) {
		left = leftWheelMaxSpeed;
	}
	else if (left < -leftWheelMaxSpeed){
		left = -leftWheelMaxSpeed;
	}

	// Round down the value smaller than the unit
	if (leftWheelSpeedUnit > ACCURACY) {
		double leftAbs = fabs(left);
		double residue = fmod(leftAbs,leftWheelSpeedUnit);
		if (left > 0) {
			left -= residue;
		}
		else {
			left += residue;
		}
	}

	// Refect maximum angular values of right wheel
	if (rightWheelMaxSpeed < right) {
		right = rightWheelMaxSpeed;
	}
	else if (right < -rightWheelMaxSpeed){
		right = -rightWheelMaxSpeed;
	}

	// Round down the value smaller than the unit
	if (rightWheelSpeedUnit > ACCURACY) {
		double rightAbs = fabs(right);
		double residue = fmod(rightAbs,leftWheelSpeedUnit);
		if (right > 0) {
			right -= residue;
		}
		else {
			right += residue;
		}
	}

	if (leftWheelName) {
#ifdef CONTROLLER
		my->setAngularVelocityToParts(leftWheelName, left, leftMaxForce);
		currentLeftWheelSpeed = left;
#endif
	}
	if (rightWheelName){
#ifdef CONTROLLER
		my->setAngularVelocityToParts(rightWheelName, right, rightMaxForce);
		currentRightWheelSpeed = right;
#endif
	}
}


/**
 * Gets the axle length
 */
double DynamicsController::getAxleLength()
{
	return axleLength;
}


/**
 * Gets the radius of the left wheel
 */
double DynamicsController::getLeftWheelRadius(SimObj *my)
{
	CParts *partsData = my->getParts(leftWheelName);

	double radius,length;

	if (  typeid( *partsData ) == typeid( CylinderParts )  ) {
		((CylinderParts*)partsData)->giveSize(radius,length);
	}
	else {
		radius = 0.0;
		length = 0.0;
	}

	leftWheelRadius = radius;

	return leftWheelRadius;
}


/**
 * Gets the radius of the rihgt wheel
 */
double DynamicsController::getRightWheelRadius(SimObj *my)
{
	CParts *partsData = my->getParts(rightWheelName);

	double radius,length;

	if (  typeid( *partsData ) == typeid( CylinderParts )  ) {
		((CylinderParts*)partsData)->giveSize(radius,length);
	}
	else {
		radius = 0.0;
		length = 0.0;
	}

	rightWheelRadius = radius;

	return rightWheelRadius;
}


/**
 * Gets the accumulation of noise for the left wheel
 */
double DynamicsController::getLeftEncoderNoise()
{
	return leftEncoderNoise;
}


/**
 * Gets the accumulation of noise for the left wheel
 */
double DynamicsController::getRightEncoderNoise()
{
	return rightEncoderNoise;
}


/**
 * Gets slip noise of the left wheel
 */
double DynamicsController::getLeftSlipNoise()
{
	return leftSlipNoise;
}


/**
 * Gets slip noise of the right wheel
 */
double DynamicsController::getRightSlipNoise()
{
	return rightSlipNoise;
}


/**
 * Gets the current speed of the left wheel
 */
double DynamicsController::getCurrentLeftWheelSpeed()
{
	return currentLeftWheelSpeed;
}


/**
 * Gets the current speed of the right wheel
 */
double DynamicsController::getCurrentRightWheelSpeed()
{
	return currentRightWheelSpeed;
}


void Controller::init()
{
	CommDataEncoder::setSendProc(CommUtil::sendData);
}

bool Controller::attach(const char *server, int port, const char *myname)
{
	bool b = Super::attach(server, port, myname);
	if (b) {
		LOG_SOCKET(m_cmdSock);
	}
	return b;
}


bool Controller::send(CommDataEncoder &enc)
{
	return enc.send(m_dataSock) > 0? true: false;
}


typedef CTReader Reader;

CommDataResult * Controller::recv(int bufsize)
{
	CommDataDecoder d;
	Reader reader(m_dataSock, d, bufsize);
	return reader.readSync();
}

typedef CommDataDecoder::Result Result;

//! Constructor
Controller::Controller() : m_in(false)
{
	// Initial value of random numbers
	dynamicsMode = MODE_NOT_USE_WHEEL;

	srand((unsigned) time(NULL));

}


/**
 * Gets uniformly-distributed random values (0 - 1)
 */
double Controller::getRand()
{
	double numerator = (double)rand();
	double denominator = (double)(RAND_MAX);
	return numerator / (denominator + 1.0);
}


/**
 * @brief Sets Webots-compatible parameters.
 * @param leftWheelName Joint name of the left wheel
 * @param leftMotorConsumption Electricity consumption of the left wheel (W)
 * @param leftWheelRadius Radius of the left wheel (m)
 * @param leftWheelMaxSpeed Maximum angular velocity of the left wheel (rad/s)
 * @param leftWheelSpeedUnit Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
 * @param leftSlipNoise Uniformly-distributed slip of the left wheel (0.0 - 1.0)
 * @param leftEncoderNoise Accumulation of slip noise for the left wheel
 * @param leftEncoderResolution Slip degree per rad for the left wheel
 * @param leftMaxForce Maximum torque of the left wheel
 * @param rightWheelName Joint name of the right wheel
 * @param rightMotorConsumption Electricity consumption of the right wheel (W)
 * @param rightWheelRadius Radius of the right wheel (m)
 * @param rightWheelMaxSpeed Maximum angular velocity of the right wheel (rad/s)
 * @param rightWheelSpeedUnit Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
 * @param rightSlipNoise Uniformly-distributed slip of the right wheel (0.0 - 1.0)
 * @param rightEncoderNoise Accumulation of slip noise for the right wheel
 * @param rightEncoderResolution Slip degree per rad for the right wheel
 * @param rightMaxForce Maximum torque of the right wheel
 */
void Controller::setWheelProperty(
	char   *leftWheelName,
	double leftMotorConsumption,
	double leftWheelMaxSpeed,
	double leftWheelSpeedUnit,
	double leftSlipNoise,
	double leftEncoderResolution,
	double leftMaxForce,
	char   *rightWheelName,
	double rightMotorConsumption,
	double rightWheelMaxSpeed,
	double rightWheelSpeedUnit,
	double rightSlipNoise,
	double rightEncoderResolution,
	double rightMaxForce
)
{
	setWheelProperty(
		myname(),
		leftWheelName,
		leftMotorConsumption,
		leftWheelMaxSpeed,
		leftWheelSpeedUnit,
		leftSlipNoise,
		leftEncoderResolution,
		leftMaxForce,
		rightWheelName,
		rightMotorConsumption,
		rightWheelMaxSpeed,
		rightWheelSpeedUnit,
		rightSlipNoise,
		rightEncoderResolution,
		rightMaxForce
	);
}


/**
 * @brief Sets Webots-compatible parameters.
 *
 * @param objectName             Agent name
 * @param leftWheelName          Joint name of the left wheel
 * @param leftMotorConsumption   Electricity consumption of the left wheel (W)
 * @param leftWheelRadius        Radius of the left wheel (m)
 * @param leftWheelMaxSpeed      Maximum angular velocity of the left wheel (rad/s)
 * @param leftWheelSpeedUnit     Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
 * @param leftSlipNoise          Uniformly-distributed slip of the left wheel (0.0 - 1.0)
 * @param leftEncoderNoise       Accumulation of slip noise for the left wheel
 * @param leftEncoderResolution  Slip degree per rad for the left wheel
 * @param leftMaxForce           Maximum torque of the left wheel
 * @param rightWheelName         Joint name of the right wheel
 * @param rightMotorConsumption  Electricity consumption of the right wheel (W)
 * @param rightWheelRadius       Radius of the right wheel (m)
 * @param rightWheelMaxSpeed     Maximum angular velocity of the right wheel (rad/s)
 * @param rightWheelSpeedUnit    Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
 * @param rightSlipNoise         Uniformly-distributed slip of the right wheel (0.0 - 1.0)
 * @param rightEncoderNoise      Accumulation of slip noise for the right wheel
 * @param rightEncoderResolution Slip degree per rad for the rihgt wheel
 * @param rightMaxForce          Maximum torque of the right wheel
 */
void Controller::setWheelProperty(
	const char   *objectName,
	char   *leftWheelName,
	double leftMotorConsumption,
	double leftWheelMaxSpeed,
	double leftWheelSpeedUnit,
	double leftSlipNoise,
	double leftEncoderResolution,
	double leftMaxForce,
	char   *rightWheelName,
	double rightMotorConsumption,
	double rightWheelMaxSpeed,
	double rightWheelSpeedUnit,
	double rightSlipNoise,
	double rightEncoderResolution,
	double rightMaxForce
)
{
	SimObj *my = getObj(objectName);
	DynamicsController *controller = new DynamicsController();

	dynamicsDataList[objectName] = controller;
	DynamicsController *dynamicsData = dynamicsDataList[objectName];

	dynamicsData->setWheelProperty(
		my,
		leftWheelName,
		leftMotorConsumption,
		leftWheelMaxSpeed,
		leftWheelSpeedUnit,
		leftSlipNoise,
		leftEncoderResolution,
		leftMaxForce,
		rightWheelName,
		rightMotorConsumption,
		rightWheelMaxSpeed,
		rightWheelSpeedUnit,
		rightSlipNoise,
		rightEncoderResolution,
		rightMaxForce
	);

}


/**
 * Gets own axle length
 */
double Controller::getAxleLength()
{
	return getAxleLength(myname());
}


/**
 * Gets axle length
 */
double Controller::getAxleLength(const char *simObjName)
{
	DynamicsController *dynamicsData = dynamicsDataList[simObjName];
	return dynamicsData->getAxleLength();
}


/**
 * Gets the radius of own left wheel
 */
double Controller::getLeftWheelRadius()
{
	return getLeftWheelRadius(myname());
}


/**
 * Gets the radius of the left wheel
 */
double Controller::getLeftWheelRadius(const char *simObjName)
{
	SimObj *my = getObj(simObjName);
	DynamicsController *dynamicsData = dynamicsDataList[simObjName];
	return dynamicsData->getLeftWheelRadius(my);
}


/**
 * Gets the radius of own right wheel
 */
double Controller::getRightWheelRadius()
{
	return getRightWheelRadius(myname());
}


/**
 * Gets the radius of the right wheel
 */
double Controller::getRightWheelRadius(const char *simObjName)
{
	SimObj *my = getObj(simObjName);
	DynamicsController *dynamicsData = dynamicsDataList[simObjName];
	return dynamicsData->getRightWheelRadius(my);
}


/**
 * Gets the accumulation of the enocder noise of own left wheel
 */
double Controller::getLeftEncoderNoise()
{
	return getLeftEncoderNoise(myname());
}


/**
 * Gets the accumulation of the enocder noise of the left wheel
 */
double Controller::getLeftEncoderNoise(const char *simObjName)
{
	DynamicsController *dynamicsData = dynamicsDataList[simObjName];
	return dynamicsData->getLeftEncoderNoise();
}


/**
 * Gets the accumulation of the enocder noise of own right wheel
 */
double Controller::getRightEncoderNoise()
{
	return getRightEncoderNoise(myname());
}


/**
 * Gets the accumulation of the enocder noise of the right wheel
 */
double Controller::getRightEncoderNoise(const char *simObjName)
{
	DynamicsController *dynamicsData = dynamicsDataList[simObjName];
	return dynamicsData->getRightEncoderNoise();
}


/**
 * Sets different velocity to own each wheel
 * @param left Angular veloccity of the left wheel
 * @param right Angular veloccity of the right wheel
 */
void Controller::differentialWheelsSetSpeed(double left,double right)
{
	differentialWheelsSetSpeed(myname(),left,right);
}


/**
 * Sets different velocity to each wheel
 * @param simObjName Name of SimObj
 * @param left Angular velocity of the left wheel
 * @param right Angular velocity of the right wheel
 */
void Controller::differentialWheelsSetSpeed(const char *simObjName,double left,double right)
{
	SimObj *my = getObj(simObjName);
	DynamicsController *dynamicsData = dynamicsDataList[simObjName];
	dynamicsData->differentialWheelsSetSpeed(my,left,right);
}


void Controller::slipWheel()
{
	DYNAMICS_CONTROLLER_LIST::iterator it = dynamicsDataList.begin();

	while( it != dynamicsDataList.end() )
	{
		DynamicsController *dynamicsData = (*it).second;
		std::string simObjName = (*it).first;
		SimObj *my = getObj(simObjName.c_str());

		// Wheel slips when noise is set
		if (dynamicsData->getLeftSlipNoise() > 0.0 || dynamicsData->getRightSlipNoise() > 0.0) {
			double leftNoise = dynamicsData->getCurrentLeftWheelSpeed() * dynamicsData->getLeftSlipNoise() * getRand();
			double rightNoise = dynamicsData->getCurrentRightWheelSpeed() * dynamicsData->getRightSlipNoise() * getRand();
			dynamicsData->differentialWheelsSetSpeed(my,leftNoise,rightNoise);
		}

		++it;
	}

}



SimObj *Controller::getObj(const char *name)
{

	if (!name || strlen(name) <= 0) { 
		return 0;
	}

	SimObj *obj = find(name);

	// Refer to the server when object is not found
	if (!obj) {

		// For future use
		/*
		std::string sendMsg = name;
		sendMsg += ",";

		// Size of the send message (including header)
		int sendSize = sendMsg.size() + sizeof(unsigned short) * 2;

		LOG_MSG(("msg = %s, size = %d", sendMsg.c_str(), sendSize));

		// Allocate send buffer
		char *sendBuff = new char[sendSize];
		char *p = sendBuff;

		// Set the size of header and data
		BINARY_SET_DATA_S_INCR(p, unsigned short, 20); // getObj has 20 data types
		BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);      

		// Sets name
		memcpy(p, sendMsg.c_str(), sendMsg.size());

		// Send deta
		if(!sendData(m_cmdSock, sendBuff, sendSize)) {
			LOG_ERR(("getObj: Failed to send request"));
			delete [] sendBuff;
			return NULL;
		}
		delete [] sendBuff;

		int recvSize = sizeof(unsigned short);
		char *recvBuff = new char[recvSize];

		// Send data
		if(!SocketUtil::recvData(m_cmdSock, recvBuff, recvSize)) {
			LOG_ERR(("getObj: cannot get result from server"));
			delete [] recvBuff;
			return NULL;
		}	  
		LOG_MSG(("recv"));	  
		p = recvBuff;
		// Sets the size of header and data
		unsigned short result = BINARY_GET_DATA_S_INCR(p, unsigned short); // connectServiceのデータタイプは9

		if(result) {
			LOG_MSG(("get!"));
			obj->setAttrValue("name", name);
			LOG_MSG(("before get!"));
		}

		delete [] recvBuff;
		*/

		int s = m_dataSock;
		CommRequestGetEntityEncoder enc(name);
		enc.send(s);

		CommDataDecoder d;

		Reader *r = new Reader(s, d, 8192); //TODO: Magic number
		int wait = 1000;
		int retry = 0;
		while  (retry < 5) {
			Result *result = r->readSync();
			if (!result) {
				retry++;
				usleep(wait);
				wait *= 2;
				continue;
			}
			
			if (result->type() == COMM_RESULT_GET_ENTITY) {
				ResultGetEntityEvent *evt = (ResultGetEntityEvent*)result->data();
				obj = evt->release();
				break;
			}
		}
		if (obj) {
			add(obj);
		}
		else{
		}
		delete r;

	  
	}
	return obj;
}

RobotObj *Controller::getRobotObj(const char *name)
{

	// Refer to the server when object is not found
	if (!name || strlen(name) <= 0) { 
		return 0;
	}

	SimObj *obj = find(name);

	if (!obj) {
	  
	  int s = m_dataSock;
		CommRequestGetEntityEncoder enc(name);
		enc.send(s);

		CommDataDecoder d;

		Reader *r = new Reader(s, d, 8192); //TODO: Magic number
		int wait = 1000;
		int retry = 0;
		while  (retry < 5) {
			Result *result = r->readSync();
			if (!result) {
				retry++;
				usleep(wait);
				wait *= 2;
				continue;
			}
			
			if (result->type() == COMM_RESULT_GET_ENTITY) {
				ResultGetEntityEvent *evt = (ResultGetEntityEvent*)result->data();
				obj = evt->release();
				break;
			}
		}
		if (obj) {
			add(obj);
		}
		else{
		}
		delete r;

	  
	}
	RobotObj* robj = dynamic_cast<RobotObj*>(obj);
	return robj;

}


ViewImage * Controller::captureView(ColorBitType cbtype, ImageDataSize size)
{
	CTSimObj & obj = getCTSimObj();
	return obj.captureView(cbtype, size);
}
//added by okamoto@tome(2011/8/26)
ViewImage * Controller::captureView(ColorBitType cbtype, ImageDataSize size, int id)
{
	if (id <= 0 || id >= 10)
	{
		LOG_ERR(("captureView: Camera ID %d does not exist",id));
		return false;
	}
	CTSimObj & obj = getCTSimObj();
	return obj.captureView(cbtype, size, id);
}


bool Controller::detectEntities(std::vector<std::string> &v)
{
	CTSimObj & obj = getCTSimObj();
	return obj.detectEntities(v);
}

//added by okamoto@tome(2011/8/15)
bool Controller::detectEntities(std::vector<std::string> &v, int id)
{
	if (id <= 0 || id >= 10)
	{
		LOG_ERR(("detectEntities: Camera ID %d does not exist",id));
		return false;
	}
	CTSimObj & obj = getCTSimObj();
	return obj.detectEntities(v,id);

}


void Controller::worldStep(double stepWidth)
{
	int sendSize = sizeof(unsigned short) * 2 + sizeof(double);

	// Allocate send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Set the size of header and data
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_WORLD_STEP);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DOUBLE_INCR(p, stepWidth);

	// Send data
	if (!sendData(m_cmdSock, sendBuff, sendSize)) {
		LOG_ERR(("worldStep: Failed to send request"));
	}
	delete [] sendBuff;
	return;
}


void Controller::worldQuickStep(double stepWidth)
{
	int sendSize = sizeof(unsigned short) * 2 + sizeof(double);

	// Allocate send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Set the size of header and data
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_WORLD_QUICK_STEP);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);
	BINARY_SET_DOUBLE_INCR(p, stepWidth);

	// Send data
	if (!sendData(m_cmdSock, sendBuff, sendSize)) {
		LOG_ERR(("worldStep: Failed to send request"));
	}
	delete [] sendBuff;
	return;
}


bool Controller::getAllEntities(std::vector<std::string> &v)
{
	CTSimObj & obj = getCTSimObj();
	return obj.getAllEntities(v);
}

Text * Controller::getText(RawSound &sound)
{
	CTSimObj & obj = getCTSimObj();
	return obj.getText(sound);
}


void Controller::add(SimObj *obj)
{
	LOG_DEBUG2(("SimObj %s added", obj->name()));
	assert(obj);
	SimObj::RequestSender *sender = this;
	obj->setRequestSener(sender);
	m_objs[obj->name()] = obj;
}


double Controller::getSimulationTime()
{
	int sendSize = sizeof(unsigned short) * 2 + sizeof(double);

	// Allocate send buffer
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Set the size of header and data
	BINARY_SET_DATA_S_INCR(p, unsigned short, REQUEST_GET_SIMULATION_TIME);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	// Send data
	if (!sendData(m_cmdSock, sendBuff, sendSize)) {
		LOG_ERR(("getSimulationTime: Failed to send request"));
		delete [] sendBuff;
		return 0.0;
	}
	delete [] sendBuff;

	char recvBuff[16]; //TODO: Magic number, danger
	int recvSize = sizeof(double);
	if (!SocketUtil::recvData(m_cmdSock, recvBuff, recvSize)) {
		LOG_ERR(("getSimulationTime: cannot get result from server"));
		return 0.0;
	}	  
	p = recvBuff;
	double time = BINARY_GET_DOUBLE_INCR(p); 
  
	return time;
}


void Controller::updateObjs()
{
	// Slip of wheels
	slipWheel();

	if (m_objs.size() > 0) {

		double t = 0;
		CommRequestUpdateEntitiesEncoder enc(t);
		int cnt = 0;
		for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
			SimObj *obj = i->second;
			if (obj) {
				enc.push(obj);
				cnt++;
			} else {
				LOG_DEBUG1(("null obj"));
			}
		}
		if (cnt > 0) {
			enc.send(m_dataSock);
		}

#ifdef DEPRECATED
		for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
			SimObj *obj = i->second;
			if (!obj) { continue; }
			if (Command *cmd = obj->createJointControlCommand()) {
				Super::send(*cmd);
			}
		}
#endif

	}
}


void Controller::clearObjs()
{
	for (M::iterator i=m_objs.begin(); i!=m_objs.end(); i++) {
		SimObj *obj = i->second;
		delete obj;
	}
	m_objs.clear();
}


static bool s_loop = false;

static void quit(int)
{
	s_loop = false;
}

void Controller::loopMain()
{
	SOCKET sock = m_cmdSock;

	CommDataDecoder d;
	d.setController(this);

	CTReader read(sock, d, 30000); // TODO: Magic number should be removed

	fd_set	rfds;
	struct timeval tv;
	s_loop = true;
	signal(SIGINT, quit);

	while (s_loop) {

		try {
			read.read();
		} catch(CTReader::ConnectionClosedException &e) {
			break;
		} 
		/*
		 * Wait a fixed time to reduce the load of CPU
		 */
		FD_ZERO(&rfds);
		tv.tv_sec = 0;
		//tv.tv_usec = 1000;
		tv.tv_usec = 10;
		select(0, &rfds, NULL, NULL, &tv);
	}
	LOG_SYS(("disconnected"));
	shutdown(m_cmdSock, 1);
	shutdown(m_dataSock,1);
	m_cmdSock = -1;
	m_dataSock = -1;
}

