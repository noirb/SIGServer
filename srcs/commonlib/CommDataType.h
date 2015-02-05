/* 
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Change GET/SET_VELOCITY to GET/SET_LINEAR_VELOCITY
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change COMM_REQUEST_SET_JOINT_VELOCITY  to COMM_REQUEST_SET_ANGULAR_VELOCITY_JOINT
 *    Change COMM_REQUEST_SET_OBJECT_VELOCITY to COMM_REQUEST_SET_ANGULAR_VELOCITY_PARTS
 * Modified by Tetsunari Inamura on 2014-01-06
 *    enum for new communication protocal was moved from WorldSimulator.h
 * Added addTorque by Tetsunari Inamura on 2014-02-26
 */

#ifndef CommDataType_h
#define CommDataType_h

enum {
	COMM_DATA_TYPE_NOT_SET = -1,

	// server request and result
	COMM_REQUEST_SIM_CTRL,  // 0
 
	COMM_REQUEST_GET_ALL_ENTITIES,
	COMM_RESULT_GET_ALL_ENTITIES,

	COMM_REQUEST_GET_ENTITY,
	COMM_RESULT_GET_ENTITY,

	COMM_REQUEST_UPDATE_ENTITIES,
	//	COMM_RESULT_UPDATE_ENTITY,

	COMM_REQUEST_CAPTURE_VIEW_IMAGE,
	COMM_RESULT_CAPTURE_VIEW_IMAGE,

	COMM_REQUEST_DETECT_ENTITIES,
	COMM_RESULT_DETECT_ENTITIES, // 

	// System Command
	
	COMM_REQUEST_ATTACH_CONTROLLER, // 10
	COMM_RESULT_ATTACH_CONTROLLER,

	COMM_REQUEST_ATTACH_VIEW,
	COMM_RESULT_ATTACH_VIEW,

	COMM_REQUEST_CONNECT_DATA_PORT,
	COMM_RESULT_CONNECT_DATA_PORT,

	COMM_REQUEST_PROVIDE_SERVICE,
	COMM_RESULT_PROVIDE_SERVICE,

	COMM_REQUEST_GET_ATTRIBUTES,
	COMM_RESULT_GET_ATTRIBUTES, 

	COMM_REQUEST_SET_ATTRIBUTES, // 20
	//COMM_RESULT_SET_ATTRIBUTES,

	COMM_REQUEST_SET_JOINT_ANGLE,

	COMM_REQUEST_SOUND_RECOG,
	COMM_RESULT_SOUND_RECOG,

	COMM_REQUEST_CONNECT_JOINT,
	COMM_REQUEST_RELEASE_JOINT,

	COMM_REQUEST_GET_JOINT_FORCE,
	COMM_RESULT_GET_JOINT_FORCE,

	// model method call
	COMM_INVOKE_CONTROLLER_ON_INIT,
	COMM_INVOKE_CONTROLLER_ON_ACTION,
	COMM_INVOKE_CONTROLLER_ON_RECV_TEXT,
	COMM_INVOKE_CONTROLLER_ON_RECV_SOUND,
	COMM_INVOKE_CONTROLLER_ON_RECV_MESSAGE,

	COMM_LOG_MSG,

	COMM_CONTROLLER_COMMAND,

	COMM_NS_QUERY_REQUEST,
	COMM_NS_QUERY_RESULT,
	COMM_NS_PINGER_REQUEST,
	COMM_NS_PINGER_RESULT,

	COMM_REQUEST_X3DDB,
	COMM_RESULT_X3DDB,

	COMM_REQUEST_GET_OBJECT_NAMES,
	COMM_RESULT_GET_OBJECT_NAMES,

	COMM_INVOKE_CONTROLLER_ON_COLLISION,

	//added by okamoto@tome 2011/2/18
	COMM_REQUEST_SET_JOINT_QUATERNION,

	//added by okamoto@tome 2011/3/3
	COMM_REQUEST_ADD_JOINT_TORQUE,

	//added by okamoto@tome 2011/3/9
	//modified by inamura on 2013-12-30
	COMM_REQUEST_SET_ANGULAR_VELOCITY_JOINT,
	COMM_REQUEST_SET_ANGULAR_VELOCITY_PARTS,

	//added by okamoto@tome 2011/3/9
	// TODO: the name of following two lines should be change
	COMM_REQUEST_GET_JOINT_ANGLE,
	COMM_RESULT_GET_JOINT_ANGLE,

	//added by okamoto@tome 2011/8/1
	COMM_DISPLAY_TEXT,

	//added by okamoto@tome 2011/9/8
	COMM_REQUEST_DISTANCE_SENSOR,
	COMM_RESULT_DISTANCE_SENSOR,

	//added by okamoto@tome 2011/12/19
	COMM_REQUEST_ADD_FORCE,           // 53
	COMM_REQUEST_ADD_FORCE_ATPOS,
	COMM_REQUEST_ADD_FORCE_ATRELPOS,
	COMM_REQUEST_SET_MASS,
	COMM_REQUEST_GET_ANGULAR_VELOCITY,
	COMM_RESULT_GET_ANGULAR_VELOCITY,
	COMM_REQUEST_GET_LINEAR_VELOCITY,
	COMM_RESULT_GET_LINEAR_VELOCITY,
	COMM_REQUEST_ADD_FORCE_TOPARTS,
	COMM_REQUEST_SET_GRAVITY_MODE,
	COMM_REQUEST_GET_GRAVITY_MODE,
	COMM_RESULT_GET_GRAVITY_MODE,
	COMM_REQUEST_SET_DYNAMICS_MODE,

	//added by noma@tome 2012/02/20
	COMM_REQUEST_GET_POINTED_OBJECT,
	COMM_RESULT_GET_POINTED_OBJECT,

	COMM_REQUEST_SET_LINEAR_VELOCITY, // Added by inamura on 2014-01-06
	COMM_REQUEST_ADD_TORQUE,          // Added by inamura on 2014-02-26
	COMM_DATA_TYPE_NUM,
};

enum {
	COMM_RESULT_OK = 0,
	COMM_RESULT_ERROR,
	COMM_RESULT_NO_AGENT,
};

typedef unsigned short CommDataType;
typedef unsigned short CommResultType;


/*
 *
 * COMM_RESULT_SIM_CTRL
 *
 */

enum {
	SIM_CTRL_COMMAND_NOT_SET = -1,
	SIM_CTRL_COMMAND_START,
	SIM_CTRL_COMMAND_STOP,
	SIM_CTRL_COMMAND_RESET,
	SIM_CTRL_COMMAND_RESTART,  /* RESET & START */
};

typedef short SimCtrlCmdType;

enum {
	SERVICE_CAPTURE_VIEW = 0x01,
	SERVICE_DISTANCE_SENSOR = 0x05,
};

typedef short ServiceType;



//
// Moved from WorldSimulator.h on 2014-01-08
//
// Data type for new data communication: added by Okamoto on 2012-07-26
// 10 is removed, because some troubles might happen.
enum {
	// Request from SIGViewer and Controller
	REQUEST_GET_ALL_ENTITIES_FIRST = 1,  // Get all of the entity data (from controller)
	REQUEST_START_SIMULATION       = 2,  // Start the simulation
	REQUEST_STOP_SIMULATION        = 3,  // Stop the simulation
	REQUEST_GET_MOVE_ENTITIES      = 4,  // Get entity data which has moved
	REQUEST_DOWNLOAD_SHAPE         = 5,  // Download of shape file
	REQUEST_DISCONNECT             = 6,  // Disconnect from the server
	REQUEST_QUIT                   = 7,  // Quit the simulation process
	REQUEST_SENDMSG_FROM_CONTROLLER= 8,  // sendMsg data from controller
	REQUEST_CONNECT_SERVICE        = 9,  // connectToSerivce from controller
	REQUEST_GET_JOINT_POSITION     = 11, // Request to get joint position
	REQUEST_SET_JOINT_QUATERNION   = 12, // Request to set joint quaternion
	REQUEST_GET_POINTING_VECTOR    = 13, // Request to get a vector made from two joints
	REQUEST_GET_PARTS_POSITION     = 14, // Request to get position of parts
	REQUEST_SET_ENTITY_POSITION    = 15, // Request to set position of entity
	REQUEST_CHECK_SERVICE          = 16, // Check whether the service is available
	REQUEST_GET_ENTITY_POSITION    = 17, // Request to get position of entity
	REQUEST_SET_ENTITY_ROTATION    = 18, // Request to set orientation of entity
	REQUEST_GET_ENTITY_ROTATION    = 19, // Request to get orientation of entity
	REQUEST_CHECK_ENTITY           = 20, // Check whether the entity exist
	REQUEST_SET_CAMERA_POSITION    = 21, // Request to set position of camera
	REQUEST_GET_CAMERA_POSITION    = 22, // Request to get position of camera
	REQUEST_SET_CAMERA_DIRECTION   = 23, // Request to set direction of camera
	REQUEST_GET_CAMERA_DIRECTION   = 24, // Request to get direction of camera
	REQUEST_SET_CAMERA_FOV         = 25, // Request to set FOV of camera
	REQUEST_SET_CAMERA_ASPECTRATIO = 26, // Request to set aspect ratio of camera
	REQUEST_SET_WHEEL              = 27, // Request to set wheel
	REQUEST_SET_WHEEL_VELOCITY     = 28, // Request to set wheel angular velocity, TODO: not unified description
	REQUEST_GET_JOINT_ANGLE        = 29, // Request to set joint angle
	REQUEST_SET_JOINT_VELOCITY     = 30, // Request to set angular velocity of joint
	REQUEST_GRASP_OBJECT           = 31, // Request to grasp a target object
	REQUEST_RELEASE_OBJECT         = 32, // Request to release a grasping object
	REQUEST_GET_ALL_JOINT_ANGLES   = 33, // Request to get all the joint angle of the entity
	REQUEST_WORLD_STEP             = 34, // Progress the time count in the simulation
	REQUEST_WORLD_QUICK_STEP       = 35, // Progress the time count in the simulation, via Quick Mode of ODE
	REQUEST_GET_ISGRASPED          = 36, // Check whether the object is grasped
	REQUEST_SET_COLLISIONABLE      = 37, // Request to set collision detect flag
	REQUEST_GET_SIMULATION_TIME    = 38, // Reference of time count in the simulation
	REQUEST_GET_COLLISION_STATE    = 39, // Check whether the objects collide
	REQUEST_SIZE                   = 40, // 
	REQUEST_GET_PARTS_QUATERNION   = 10, // Request to get quaternion of parts 10 is used in order to avoid modification on SIGViewer  // added by Guezout (2015/1/28)
};

#endif /* CommDataType_h */
 
