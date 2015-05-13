/*
 * Written by Atsushi Okamoto on 2011-12-14
 *
 * Modified by Yoshiaki Mizuchi on2014-01-16
 *    Add English comments
 */

#ifndef Controller_h
#define Controller_h

#include <string>
#include <vector>
#include <map>

#include "ControllerImpl.h"
#include "ViewImageInfo.h"
#include "SimObj.h"

//#include "DynamicsController.h"


class ViewImage;
class DepthImage;
class Text;
class RawSound;



class DynamicsController {
public:
	DynamicsController();
	/**
	 * @brief Sets Webots-compatible parameters.
	 *
	 * @param leftWheelName          Joint name of the left wheel
	 * @param leftMotorConsumption   Electricity consumption of the left wheel (W)
	 * @param leftWheelMaxSpeed      Maximum angular velocity of the left wheel (rad/s)
	 * @param leftWheelSpeedUnit     Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
	 * @param leftSlipNoise          Uniformly-distributed slip of the left wheel (0.0 - 1.0)
	 * @param leftEncoderResolution  Slip degree per rad for the left wheel
	 * @param leftMaxForce           Maximum torque of the left wheel
	 * @param rightWheelName         Joint name of the right wheel
	 * @param rightMotorConsumption  Electricity consumption of the right wheel (W)
	 * @param rightWheelMaxSpeed     Maximum angular velocity of the right wheel (rad/s)
	 * @param rightWheelSpeedUnit    Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
	 * @param rightSlipNoise         Uniformly-distributed slip of the right wheel (0.0 - 1.0)
	 * @param rightEncoderResolution Slip degree per rad for the right wheel
	 * @param rightMaxForce          Maximum torque of the right wheel
	 * @param axleLength Distance between the wheels
	 */
	void setWheelProperty(
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
	);

	/**
	 * Sets different velocity to each wheel
	 * @param left Angular velocity of the left wheel
	 * @param right Angular velocity of the right wheel
	 */
	void differentialWheelsSetSpeed(SimObj *my,double left,double right);

	/**
	 * Gets the axle length
	 */
	double getAxleLength();

	/**
	 * Gets the radius of the left wheel
	 */
	double getLeftWheelRadius(SimObj *my);

	/**
	 * Gets the radius of the right wheel
	 */
	double getRightWheelRadius(SimObj *my);

	/**
	 * Gets the accumulation of noise for the left wheel
	 */
	double getLeftEncoderNoise();

	/**
	 * Gets the accumulation of noise for the right wheel
	 */
	double getRightEncoderNoise();

	/**
	 * Gets slip of the left wheel
	 */
	double getLeftSlipNoise();

	/**
	 * Gets slip of the right wheel
	 */
	double getRightSlipNoise();

	/**
	 * Gets the current speed of the left wheel
	 */
	double getCurrentLeftWheelSpeed();

	/**
	 * Gets the current speed of the right wheel
	 */
	double getCurrentRightWheelSpeed();

private:

	#define ACCURACY (0.00000001)

	/** Name of the left wheel */
	char   *leftWheelName;
	/** Electricity consumption of the left wheel [W] */
	double leftMotorConsumption;
	/** Maximum angular velocity of the left wheel [rad/s] */
	double leftWheelMaxSpeed;
	/** Set of unit velocity of the left wheel */
	double leftWheelSpeedUnit;
	/** Slip of the left wheel */
	double leftSlipNoise;
	/** Accumulation of slip noise of the left wheel */
	//double leftCumulativeNoise;
	/** Slip degree per rad for the left wheel */
	double leftEncoderResolution;
	/** Maximum torque of the left wheel */
	double leftMaxForce;
	/** Joint name of the right wheel */
	char   *rightWheelName;
	/** Electricity consumption of the right wheel [W] */
	double rightMotorConsumption;
	/** Maximum angular velocity of the right wheel [rad/s] */
	double rightWheelMaxSpeed;
	/** Set of unit velocity of the right wheel */
	double rightWheelSpeedUnit;
	/** Slip of the right wheel */
	double rightSlipNoise;
	/** Accumulation of slip noise of the right wheel */
	//double rightCumulativeNoise;
	/** Slip degree per rad for the right wheel */
	double rightEncoderResolution;
	/** Maximum torque of the right wheel */
	double rightMaxForce;


	/** Distance of the wheels */
	double axleLength;
	/** Radius of the left wheel [m] */
	double leftWheelRadius;
	/** Radius of the right wheel [m] */
	double rightWheelRadius;
	/** Accumulation of slip noise of the left wheel */
	double leftEncoderNoise;
	/** Accumulation of slip noise of the right wheel */
	double rightEncoderNoise;

	/** Current velocity of the left wheel */
	double currentLeftWheelSpeed;
	/** Current velocity of the right wheel */
	double currentRightWheelSpeed;

};





/**
 * @brief Fundamental class for customer-defined controller
 *
 * Agent controller generates the class which is inherited this class, and descriptes actions of an agent in the inherited class
 */
class Controller : public ControllerImpl, public SimObj::RequestSender
{
private:
	typedef ControllerImpl Super;
	
private:
	std::map<std::string, SimObj*> m_objs;
	bool m_in;

private:
	SimObj * find(const char *name)
	{
		return m_objs[name];
	}
 public:
	void updateObjs();
	void clearObjs();
private:
	bool send(CommDataEncoder &);
	CommDataResult * recv(int bufsize);
	SOCKET getControllerSocket() { SOCKET sock; return sock; }

protected:
	//! Constructor
	Controller();

	/**
	 * Gets uniformly-distributed random values (0 - 1)
	 */
	double getRand();

	/**
	 * @brief Sets Webots-compatible parameters.
	 *
	 * @param leftWheelName Joint name of the left wheel
	 * @param leftMotorConsumption Electricity consumption of the left wheel (W)
	 * @param leftWheelMaxSpeed Maximum angular velocity of the left wheel (rad/s)
	 * @param leftWheelSpeedUnit Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
	 * @param leftSlipNoise Uniformly-distributed slip of the left wheel (0.0 - 1.0)
	 * @param leftEncoderResolution Slip degree per rad for the left wheel
	 * @param leftMaxForce Maximum torque of the left wheel
	 * @param rightWheelName Joint name of the right wheel
	 * @param rightMotorConsumption Electricity consumption of the right wheel (W)
	 * @param rightWheelMaxSpeed Maximum angular velocity of the right wheel (rad/s)
	 * @param rightWheelSpeedUnit Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
	 * @param rightSlipNoise Uniformly-distributed slip of the right wheel (0.0 - 1.0)
	 * @param rightEncoderResolution Slip degree per rad for the right wheel
	 * @param rightMaxForce Maximum torque of the right wheel
	 */
	void setWheelProperty(
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
	);

	/**
	 * @brief Sets Webots-compatible parameters.
	 *
	 * @param objectName             Agent name
	 * @param leftWheelName          Joint name of the left wheel
	 * @param leftMotorConsumption   Electricity consumption of the left wheel (W)
	 * @param leftWheelMaxSpeed      Maximum angular velocity of the left wheel (rad/s)
	 * @param leftWheelSpeedUnit     Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
	 * @param leftSlipNoise          Uniformly-distributed slip of the left wheel (0.0 - 1.0)
	 * @param leftEncoderResolution  Slip degree per rad for the left wheel
	 * @param leftMaxForce           Maximum torque of the left wheel
	 * @param rightWheelName         Joint name of the right wheel
	 * @param rightMotorConsumption  Electricity consumption of the right wheel (W)
	 * @param rightWheelMaxSpeed     Maximum angular velocity of the right wheel (rad/s)
	 * @param rightWheelSpeedUnit    Unit of velocity set at wb_differential_wheels_set_speed (rad/s)
	 * @param rightSlipNoise         Uniformly-distributed slip of the right wheel (0.0 - 1.0)
	 * @param rightEncoderResolution Slip degree per rad for the rihgt wheel
	 * @param rightMaxForce          Maximum torque of the right wheel
	 */
	void setWheelProperty(
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
	);

	/**
	 * Gets own axle length
	 */
	double getAxleLength();

	/**
	 * Gets axle length
	 */
	double getAxleLength(const char *simObjName);

	/**
	 * Gets the radius of own left wheel
	 */
	double getLeftWheelRadius();

	/**
	 * Gets the radius of the left wheel
	 */
	double getLeftWheelRadius(const char *simObjName);

	/**
	 * Gets the radius of own right wheel
	 */
	double getRightWheelRadius();

	/**
	 * Gets the radius of the right wheel
	 */
	double getRightWheelRadius(const char *simObjName);

	/**
	 * Gets the accumulation of the encoder noise of own left wheel
	 */
	double getLeftEncoderNoise();

	/**
	 * Gets the accumulation of the encoder noise of the left wheel
	 */
	double getLeftEncoderNoise(const char *simObjName);

	/**
	 * Gets the accumulation of the v noise of own right wheel
	 */
	double getRightEncoderNoise();

	/**
	 * Gets the accumulation of the encoder noise of the right wheel
	 */
	double getRightEncoderNoise(const char *simObjName);

	/**
	 * Sets different velocity to own each wheel
	 * @param left Angular velocity of the left wheel
	 * @param right Angular velocity of the right wheel
	 */
	void differentialWheelsSetSpeed(double left,double right);

	/**
	 * Sets different velocity to each wheel
	 * @param simObjName Name of SimObj
	 * @param left Angular velocity of the left wheel
	 * @param right Angular velocity of the right wheel
	 */
	void differentialWheelsSetSpeed(const char *simObjName,double left,double right);

	/**
	 * @brief Gets specified entity
	 * @param name Entity name
	 */
	SimObj * getObj(const char *name);

	/**
	 * @brief Gets simulation time
	 */
	double  getSimulationTime();

	/**
	 * @brief robotGets entity
	 * @param name Entity name
	 */
	RobotObj  * getRobotObj(const char *name);

	// old
	ViewImage * captureView(ColorBitType cbtype, ImageDataSize size);

	// old
	ViewImage * captureView(ColorBitType cbtype, ImageDataSize size, int id);

	// old
	bool detectEntities(std::vector<std::string> &v);

	// old
	bool detectEntities(std::vector<std::string> &v, int id);

	/**
	 * @brief Updates SIGVerse world to the next step
	 * @param stepsize Step size (sec)
	 */
	void worldStep(double stepsize);

	/**
	 * @brief Updates SIGVerse world to the next step (Its speed is higher than worldStep, but its accuracy is less)
	 * @param stepsize Step size(sec)
	 */
	void worldQuickStep(double stepsize);

	/**
	 * @brief Process voice data through the speech recognition service
	 *
	 * Sends voice data to the service provider for speech recognition, and recieves the result as text data
	 *
	 * @param sound Voice data received from recvSound method
	 *
	 * @retval !=NULL Success
	 * @retval NULL Failure
	 */
	Text * getText(RawSound &sound);

	/**
	 * @brief Gets the names of all existing entities in the simulation world
	 * 
	 * @param v Container for entity names
	 * @retval true Success
	 * @retval false Failure
	 */
	bool getAllEntities(std::vector<std::string> &v);

	/**
	 * @brief  Connects to service
	 * 
	 * @param  name Service name
	 * @retval true Success
	 * @retval false Failure
	 */
	//bool connectToService(std::string name);


	bool isProcessing()
	{
		return m_in;
	}
	void onPreEvent()
	{
		m_in = true;
	}
	void onPostEvent()
	{
		//updateObjs();
		//clearObjs();
		m_in = false;
	}

private:
	typedef std::map<std::string, DynamicsController*> DYNAMICS_CONTROLLER_LIST;
	DYNAMICS_CONTROLLER_LIST dynamicsDataList;

	void add(SimObj *obj);

	/**
	 * Whether to use or not dynamics for vehicles
	 */
	enum{
		MODE_NOT_USE_WHEEL = 0,	
		MODE_USE_WHEEL
	};
	/**
	 * Flag of the dynamics mode for vehicles
	 */
	int dynamicsMode;

	void slipWheel();

	//
	// Slips the left wheel
	// The slip is calculated from leftSlipNoise
	//
	//void slipLeftWheel();

	//
	// Slips the right wheel
	// The Slip is calculated from rightSlipNoise
	//
	//void slipRightWheel();

	//
	// Calculates torque from angular velocity and power
	// Equation is as follows
	// 
	// P = F*rw = -- * rw = 2*pi*Tf
	// 
	// P : Power consumption [W]
	// F : Force acting on wheel[N]
	// r : Radius of wheel[m]
	// w : Angular frequency [rad/sec]
	// T : Torque[N m]
	// f : Frequency(Number of rotation)[Hz]
	//
	// @param motorConsumption Moter consumption [W]
	// @param radius Radius of the wheel [m]
	// @param wheelSpeed Angular velocity of the wheel[rad/sec]
	// @return Torque of the wheel [N m]
	//
	//double getTorqueFromMotor(double motorConsumption,double radius,double wheelSpeed);

	//
	// @brief Moves motors
	// @param jointName        Joint name
	// @param motorConsumption Motor consumption [W]
	///
	//void moveMotor(char *jointName,double motorConsumption);

public:
	/**
	 * @brief Allocates controller to the agent
	 *
	 * Connects to the simulation server, and allocate the controller to the specified agent
	 *
	 * @param server  Simulation server (IP adress or host name)
	 * @param port    Port number of the simulation server
	 * @param myname  Agent name
	 */
	bool attach(const char *server, int port, const char *myname);
	//! Loop function for controller
	void loopMain();

	//!! Init function for controller
	static void init();
};


#endif // Controller_h


