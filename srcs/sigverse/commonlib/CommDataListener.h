/* 
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change Set/GetVelocity to Set/GetLinearVelocity
 *    Change ObjectVelocity to AngularVelocityToParts
 *    Change JointVelocity  to AngularVelocityToJoint
 * Add SetLinearVelocity by Tetsunari Inamura on 2014-01-06
 * Modidied by Yoshiaki Mizuchi on 2014-01-16
 *    Add English comments
 */
   

#ifndef CommDataListener_h
#define CommDataListener_h

#include <sigverse/commonlib/RecvEvent.h>

class Source;

/**
 * @brief Listener interface to get the result data reconstructed by CommDataDecoder
 *
 * Corresponding methods are called back when data is reconstructed
 * <ol>
 *    <li>first augment (from) is source adress of data</li>
 *    <li>second augment (evt) includes reconstructed data (Some callback include not the data but only the signal)</li>
 * </ol>
 */
class CommDataListener
{
public:
	virtual ~CommDataListener() {;}
#ifdef STRICT_INTERFACE
	virtual void recvRequestSimCtrl            (Source &from, RequestSimCtrlEvent &evt) = 0;
	virtual void recvRequestGetAllEntities     (Source &from, RequestGetAllEntitiesEvent &evt) = 0;
	virtual void recvResultGetAllEntities      (Source &from, ResultGetAllEntitiesEvent &evt) = 0;
	virtual void recvRequestUpdateEntities     (Source &from, RequestUpdateEntitiesEvent &evt) = 0;
	virtual void recvRequestGetEntity          (Source &from, RequestGetEntityEvent &evt) = 0;
	virtual void recvResultGetEntity           (Source &from, ResultGetEntityEvent &evt) = 0;
	virtual void recvRequestAttachController   (Source &from, RequestAttachControllerEvent &evt) = 0;
	virtual void recvResultAttachController    (Source &from, ResultAttachControllerEvent &evt) = 0;
	virtual void recvRequestCaptureViewImage   (Source &from, RequestCaptureViewImageEvent &evt) = 0;
	virtual void recvResultCaptureViewImage    (Source &from, ResultCaptureViewImageEvent &eve) = 0;
	virtual void recvRequestDetectEntities     (Source &from, RequestDetectEntitiesEvent &evt) = 0;
	virtual void recvResultDetectEntities      (Source &from, ResultDetectEntitiesEvent &evt) = 0;
	virtual void recvRequestDistanceSensor     (Source &from, RequestDistanceSensorEvent &evt) = 0;
	virtual void recvResultDistanceSensor      (Source &from, ResultDistanceSensorEvent &evt) = 0;

	virtual void recvRequestProvideService     (Source &from, RequestProvideServiceEvent &evt) = 0;
	//virtual void recvResultProvideService    (Source &from, ResultProvideServiceEvent &evt) = 0;

	virtual void recvRequestConnectDataPort    (Source &from, RequestConnectDataPortEvent &evt) = 0;
	//virtual void recvResultConnectDataProt   (Source &from, ResultConnectDataPortEvent &evt) = 0;

	virtual void recvRequestAttachView         (Source &from, RequestAttachViewEvent &evt) = 0;
	virtual void recvResultAttachView          (Source &from, ResultAttachViewEvent &evt) = 0;
		
	virtual void recvLogMsg                    (Source &from, LogMsgEvent &evt) = 0;


	virtual void recvRequestNSQuery            (Source &from, RequestNSQueryEvent &) = 0;
	virtual void recvResultNSQuery             (Source &from, ResultNSQueryEvent &) = 0;

	virtual void recvRequestNSPinger           (Source &from, RequestNSPingerEvent &) = 0;
	virtual void recvResultNSPinger            (Source &from, ResultNSPingerEvent &) = 0;

	virtual void recvRequestSetJointAngle      (Source &from, RequestSetJointAngleEvent &) = 0;

	//added by okamoto@tome (2011/2/18)
	virtual void recvRequestSetJointQuaternion (Source &from, RequestSetJointQuaternionEvent &) = 0;
	//added by okamoto@tome (2011/3/3)
	virtual void recvRequestAddJointTorque     (Source &from, RequestAddJointTorqueEvent &) = 0;
	//added by okamoto@tome (2011/3/9)
	//modified by inamura on 2013-12-30
	virtual void recvRequestSetAngularVelocityToJoint  (Source &from, RequestSetAngularVelocityToJointEvent &) = 0;
	virtual void recvRequestSetAngularVelocityToParts  (Source &from, RequestSetAngularVelocityToPartsEvent &) = 0;
	//added by okamoto@tome (2011/3/9)
	virtual void recvRequestGetJointAngle      (Source &from, RequestGetJointAngleEvent &) = 0;
	virtual void recvResultGetJointAngle       (Source &from, ResultGetJointAngleEvent &) = 0;
	//added by okamoto@tome (2011/8/2)
	virtual void recvDisplayText               (Source &from, DisplayTextEvent &) = 0;
	//added by okamoto@tome (2011/12/19)
	virtual void recvRequestAddForce           (Source &from, RequestAddForceEvent &) = 0;
	virtual void recvRequestAddForceAtPos      (Source &from, RequestAddForceAtPosEvent &) = 0;
	virtual void recvRequestSetMass            (Source &from, RequestSetMassEvent &) = 0;
	virtual void recvRequestGetAngularVelocity (Source &from, RequestGetAngularVelocityEvent &) = 0;
	virtual void recvResultGetAngularVelocity  (Source &from, ResultGetAngularVelocityEvent &) = 0;
	virtual void recvRequestGetLinearVelocity  (Source &from, RequestGetLinearVelocityEvent &) = 0;
	virtual void recvResultGetLinearVelocity   (Source &from, ResultGetLinearVelocityEvent &) = 0;
	virtual void recvRequestSetLinearVelocity  (Source &from, RequestSetLinearVelocityEvent &) = 0; // added by inamura on 2014-01-06
	virtual void recvRequestAddForceToParts    (Source &from, RequestAddForceToPartsEvent &) = 0;
	virtual void recvRequestAddTorque          (Source &from, RequestAddTorqueEvent &) = 0;         // added by inamura on 2014-02-26

	virtual void recvRequestSetGravityMode     (Source &from, RequestSetGravityModeEvent &) = 0;
	virtual void recvRequestGetGravityMode     (Source &from, RequestGetGravityModeEvent &) = 0;
	virtual void recvResultGetGravityMode      (Source &from, ResultGetGravityModeEvent &) = 0;
	virtual void recvRequestSetDynamicsMode    (Source &from, RequestSetDynamicsModeEvent &) = 0;

	virtual void recvRequestSoundRecog         (Source &from, RequestSoundRecogEvent &) = 0;
	virtual void recvResultSoundRecog          (Source &from, ResultSoundRecogEvent &) = 0;

	virtual void recvRequestGetJointForce      (Source &from, RequestGetJointForceEvent &) = 0;
	virtual void recvResultGetJointForce       (Source &from, ResultGetJointForceEvent &) = 0;

	virtual void recvRequestConnectJoint       (Source &from, RequestConnectJointEvent &) = 0;
	virtual void recvRequestReleaseJoint       (Source &from, RequestReleaseJointEvent &) = 0;

	virtual void recvRequestGetObjectNames     (Source &from, RequestGetObjectNamesEvent &) = 0;
	virtual void recvResultGetObjectNames      (Source &from, ResultGetObjectNamesEvent &) = 0;

	//added by noma@tome (2012/02/20)
	virtual void recvRequestGetPointedObject   (Source &from, RequestGetPointedObjectEvent &) = 0;
	virtual void recvResultGetPointedObject    (Source &from, ResultGetPointedObjectEvent &) = 0;
	
#else
	//! COMM_REQUEST_SIM_CTRL
	virtual void recvRequestSimCtrl            (Source &from, RequestSimCtrlEvent &evt) {};

	//! COMM_REQUEST_GET_ALL_ENTITIES
	virtual void recvRequestGetAllEntities     (Source &from, RequestGetAllEntitiesEvent &evt) {};
	//! COMM_RESULT_GET_ALL_ENTITIES
	virtual void recvResultGetAllEntities      (Source &from, ResultGetAllEntitiesEvent &evt) {};
	//! COMM_REQUEST_UPDATE_ENTITIES
	virtual void recvRequestUpdateEntities     (Source &from, RequestUpdateEntitiesEvent &evt) {};
	//! COMM_REQUEST_GET_ENTITY
	virtual void recvRequestGetEntity          (Source &from, RequestGetEntityEvent &evt) {};
	//! COMM_RESULT_GET_ENTITY
	virtual void recvResultGetEntity           (Source &from, ResultGetEntityEvent &evt) {};
	//! COMM_REQUEST_ATTACH_CONTROLLER
	virtual void recvRequestAttachController   (Source &from, RequestAttachControllerEvent &evt) {};
	//! COMM_RESULT_ATTACH_CONTROLLER
	virtual void recvResultAttachController    (Source &from, ResultAttachControllerEvent &evt) {};
	//! COMM_REQUEST_ATTACH_VIEW
	virtual void recvRequestAttachView         (Source &from, RequestAttachViewEvent &evt) {};
	//! COMM_RESULT_ATTACH_VIEW
	virtual void recvResultAttachView          (Source &from, ResultAttachViewEvent &evt) {};
	//! COMM_REQUEST_PROVIDE_SERVICE
	virtual void recvRequestProvideService     (Source &from, RequestProvideServiceEvent &evt) {};
	//		virtual void recvResultProvideService(Source &from, ResultProvideServiceEvent &evt) {};
	//! COMM_REQUEST_CONNECT_DATA_PORT
	virtual void recvRequestConnectDataPort    (Source &from, RequestConnectDataPortEvent &evt) {};
	//		virtual void recvResultConnectDataProt(Source &from, ResultConnectDataPortEvent &evt) {};
	//! COMM_REQUEST_CAPTURE_VIEW_IMAGE
	virtual void recvRequestCaptureViewImage   (Source &from, RequestCaptureViewImageEvent &evt) {};

	//! COMM_RESULT_CAPTURE_VIEW_IMAGE
	virtual void recvResultCaptureViewImage    (Source &from, ResultCaptureViewImageEvent &evt) {};

	//! COMM_REQUEST_DETECT_ENTITIES
	virtual void recvRequestDetectEntities     (Source &from, RequestDetectEntitiesEvent &evt) {};
	
	//! COMM_RESULT_DETECT_ENTITIES
	virtual void recvResultDetectEntities      (Source &from, ResultDetectEntitiesEvent &evt) {};

	//! COMM_REQUEST_DISTANCE_SENSOR
	virtual void recvRequestDistanceSensor (Source &from, RequestDistanceSensorEvent &evt) {};

	//! COMM_RESULT_DISTANCE_SENSOR
	virtual void recvResultDistanceSensor      (Source &from, ResultDistanceSensorEvent &evt) {};

	//! COMM_LOG_MSG
	virtual void recvLogMsg                    (Source &from, LogMsgEvent &evt) {};

	//! COMM_LOG_MSG
	virtual void recvDisplayText               (Source &from, DisplayTextEvent &evt) {};

	//! COMM_NS_QUERY_REQUEST
	virtual void recvRequestNSQuery            (Source &from, RequestNSQueryEvent &) {};
	//! COMM_NS_QUERY_RESULT
	virtual void recvResultNSQuery             (Source &from, ResultNSQueryEvent &) {};

	//! COMM_NS_PINGER_REQUEST
	virtual void recvRequestNSPinger           (Source &from, RequestNSPingerEvent &) {};
	//! COMM_NS_PINGER_REQUEST
	virtual void recvResultNSPinger            (Source &from, ResultNSPingerEvent &) {};

	//! COMM_REQUEST_SET_JOINT_ANGLE
	virtual void recvRequestSetJointAngle      (Source &from, RequestSetJointAngleEvent &) {};

	//! COMM_REQUEST_SET_JOINT_QUATERNION added by okamoto@tome (2011/2/18)
	virtual void recvRequestSetJointQuaternion (Source &from, RequestSetJointQuaternionEvent &) {};

	//! COMM_REQUEST_ADD_JOINT_TORQUE added by okamoto@tome (2011/3/3)
	virtual void recvRequestAddJointTorque     (Source &from, RequestAddJointTorqueEvent &) {};

	//! COMM_REQUEST_SET_JOINT_VELOCITY added by okamoto@tome (2011/3/9)
	//modified by inamura on 2013-12-30
	virtual void recvRequestSetAngularVelocityToJoint  (Source &from, RequestSetAngularVelocityToJointEvent &) {};
	virtual void recvRequestSetAngularVelocityToParts  (Source &from, RequestSetAngularVelocityToPartsEvent &) {};

	//! COMM_RESULT_GET_JOINT_ANGLE added by okamoto@tome (2011/3/9)
	virtual void recvRequestGetJointAngle      (Source &from, RequestGetJointAngleEvent &) {};
	virtual void recvResultGetJointAngle       (Source &from, ResultGetJointAngleEvent &) {};

	//! COMM_REQUEST_SOUND_RECOG
	virtual void recvRequestSoundRecog         (Source &from, RequestSoundRecogEvent &) {};
	//! COMM_RESULT_SOUND_RECOG
	virtual void recvResultSoundRecog          (Source &from, ResultSoundRecogEvent &) {};
	//! COMM_REQUEST_GET_JOINT_FORCE
	virtual void recvRequestGetJointForce      (Source &from, RequestGetJointForceEvent &) {};
	//! COMM_RESULT_GET_JOINT_FORCE
	virtual void recvResultGetJointForce       (Source &from, ResultGetJointForceEvent &) {};

	//! COMM_REQUEST_CONNECT_JOINT
	virtual void recvRequestConnectJoint       (Source &from, RequestConnectJointEvent &) {};
	//! COMM_REQUEST_RELEASE_JOINT
	virtual void recvRequestReleaseJoint       (Source &from, RequestReleaseJointEvent &) {};

	//! COMM_REQUEST_GET_OBJECT_NAMES
	virtual void recvRequestGetObjectNames     (Source &from, RequestGetObjectNamesEvent &) {};
	//! COMM_RESULT_GET_JOINT_FORCE
	virtual void recvResultGetObjectNames      (Source &from, ResultGetObjectNamesEvent &) {};

	//added by okamoto@tome (2011/12/19)
	//! COMM_REQUEST_ADD_FORCE
	virtual void recvRequestAddForce           (Source &from, RequestAddForceEvent &) {};
	//! COMM_REQUEST_ADD_FORCE_ATPOS
	virtual void recvRequestAddForceAtPos      (Source &from, RequestAddForceAtPosEvent &) {};
	//! COMM_REQUEST_SET_MASS
	virtual void recvRequestSetMass            (Source &from, RequestSetMassEvent &) {};
	//! COMM_REQUEST_GET_ANGULAR_VELOCITY
	virtual void recvRequestGetAngularVelocity (Source &from, RequestGetAngularVelocityEvent &) {};
	//! COMM_RESULT_GET_ANGULAR_VELOCITY
	virtual void recvResultGetAngularVelocity  (Source &from, ResultGetAngularVelocityEvent &) {};
	//! COMM_REQUEST_GET_LINEAR_VELOCITY
	virtual void recvRequestGetLinearVelocity  (Source &from, RequestGetLinearVelocityEvent &) {};
	//! COMM_RESULT_GET_LINEAR_VELOCITY
	virtual void recvResultGetLinearVelocity   (Source &from, ResultGetLinearVelocityEvent &) {};
	//! COMM_RESULT_SET_LINEAR_VELOCITY
	virtual void recvRequestSetLinearVelocity  (Source &from, RequestSetLinearVelocityEvent &) {}; // added by inamura on 2014-01-06
	//! COMM_REQUEST_ADD_FORCE_TOPARTS
	virtual void recvRequestAddForceToParts    (Source &from, RequestAddForceToPartsEvent &) {};
	//! COMM_REQUEST_ADD_TORQUE
	virtual void recvRequestAddTorque          (Source &from, RequestAddTorqueEvent &) {};         // added by inamura on 2014-02-26

	//! COMM_REQUEST_SET_GRAVITY_MODE
	virtual void recvRequestSetGravityMode     (Source &from, RequestSetGravityModeEvent &) {};
	//! COMM_REQUEST_GET_GRAVITY_MODE
	virtual void recvRequestGetGravityMode     (Source &from, RequestGetGravityModeEvent &) {};
	//! COMM_RESULT_GET_GRAVITY_MODE
	virtual void recvResultGetGravityMode      (Source &from, ResultGetGravityModeEvent &) {};
	//! COMM_REQUEST_SET_DYNAMICS_MODE
	virtual void recvRequestSetDynamicsMode    (Source &from, RequestSetDynamicsModeEvent &) {};

	//added by noma@tome (2012/02/20)
	//! COMM_REQUEST_GET_POINTED_OBJECT
	virtual void recvRequestGetPointedObject   (Source &from, RequestGetPointedObjectEvent &) {};
	//! COMM_RESULT_GET_POINTED_OBJECT
	virtual void recvResultGetPointedObject    (Source &from, ResultGetPointedObjectEvent &) {};


#endif
};


#endif // CommDataListener_h
 


