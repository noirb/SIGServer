/*
 * Moved from WorldSimulator.cpp.
 */

#ifndef SIGVERSE_DECODER_LISTENER_H
#define SIGVERSE_DECODER_LISTENER_H

#include "ServerAcceptProc.h"
#include "CommDataEncoder.h"
#include "SSimWorld.h"
#include "CommUtil.h"
#include "SimWorldProvider.h"

#include "SimObj.h"
#include "Joint.h"
#include "ServiceNameServer.h"

static bool s_restart = false;


class DecoderListener : public CommDataDecoder::Listener
{
private:
	ServerAcceptProc  &m_accept;
	SimWorldProvider  &m_provider;
	ServiceNameServer &m_ns;
public:
	DecoderListener(ServerAcceptProc &a, SimWorldProvider &p, ServiceNameServer &ns);

	void notifyInit();
private:
	void recvRequestSimCtrl(Source &, RequestSimCtrlEvent &evt);

	// Send all of the entity data to Viewer
	void recvRequestGetAllEntities(Source &from, RequestGetAllEntitiesEvent &evt);

	void recvResultGetAllEntities(Source &, ResultGetAllEntitiesEvent &evt);

	void recvRequestGetEntity(Source &from, RequestGetEntityEvent &evt);

	void recvRequestUpdateEntities(Source &from, RequestUpdateEntitiesEvent &evt);
	void recvResultGetEntity         (Source &from, ResultGetEntityEvent &evt);

	void recvRequestAttachController (Source &from, RequestAttachControllerEvent &evt);
	void recvRequestProvideService   (Source &from, RequestProvideServiceEvent &evt);
	void recvRequestConnectDataPort  (Source &from, RequestConnectDataPortEvent &evt);
	void recvRequestAttachView       (Source &from, RequestAttachViewEvent &evt);

	void recvResultAttachController  (Source &from, ResultAttachControllerEvent &evt);
	void recvResultAttachView        (Source &from, ResultAttachViewEvent &evt);

	void recvRequestCaptureViewImage (Source &from, RequestCaptureViewImageEvent &evt);
	void recvResultCaptureViewImage  (Source &from, ResultCaptureViewImageEvent &eve);

	void recvRequestDetectEntities   (Source &from, RequestDetectEntitiesEvent &evt);
	void recvResultDetectEntities    (Source &from, ResultDetectEntitiesEvent &eve);

	//added by okamoto@tome (2011/9/9)
	void recvRequestDistanceSensor   (Source &from, RequestDistanceSensorEvent &evt);
	void recvResultDistanceSensor    (Source &from, ResultDistanceSensorEvent &evt);

	void recvRequestAddForce         (Source &from, RequestAddForceEvent &evt);


	void recvRequestAddForceAtPos(Source &from, RequestAddForceAtPosEvent &evt);

	void recvRequestAddTorque (Source &from, RequestAddTorqueEvent &evt);

	void recvRequestSetMass(Source &from, RequestSetMassEvent &evt);

	// Modified by inamura on 2014-01-13
	void recvRequestGetAngularVelocity(Source &from, RequestGetAngularVelocityEvent &evt);

	// TODO: write the contents
	void recvResultGetAngularVelocity(Source &from, ResultGetAngularVelocityEvent &evt);


	void recvRequestGetLinearVelocity(Source &from, RequestGetLinearVelocityEvent &evt);

	// TODO: write the contents
	// change to LinearVelocity: by inamura on 2013-12-29
	void recvResultGetLinearVelocity(Source &from, ResultGetLinearVelocityEvent &evt);

	// Added by Tetsunari Inamura on 2014-01-06
	void recvRequestSetLinearVelocity(Source &from, RequestSetLinearVelocityEvent &evt);

	void recvRequestAddForceToParts(Source &from, RequestAddForceToPartsEvent &evt);

	void recvRequestSetGravityMode(Source &from, RequestSetGravityModeEvent &evt);

	void recvRequestGetGravityMode(Source &from, RequestGetGravityModeEvent &evt);

	void recvResultGetGravityMode(Source &from, ResultGetGravityModeEvent &evt);

	void recvRequestSetDynamicsMode(Source &from, RequestSetDynamicsModeEvent &evt);

	void recvLogMsg(Source &from, LogMsgEvent &evt);

	void recvDisplayText(Source &from, DisplayTextEvent &evt);

	void recvRequestNSQuery(Source &from, RequestNSQueryEvent &evt);

	void recvResultNSQuery(Source &from, ResultNSQueryEvent &);

	void recvRequestNSPinger(Source &from, RequestNSPingerEvent &);
	void recvResultNSPinger(Source &from, ResultNSPingerEvent &);
	void recvRequestSetJointAngle(Source &from, RequestSetJointAngleEvent &evt);

	// added by okamoto (2011/2/18)
	void recvRequestSetJointQuaternion(Source &from, RequestSetJointQuaternionEvent &evt);

	// added by okamoto (2011/3/4)
	void recvRequestAddJointTorque(Source &from, RequestAddJointTorqueEvent &evt);

	//! Set Angular Velocity of joint. Joint name, angular velocity and max torque should be included in the message
	// Set angular velocity of joint, added by okamoto (2011/3/9)
	// modified by inamura: change from recvRequestSetJointVelocity
	void recvRequestSetAngularVelocityToJoint(Source &from, RequestSetAngularVelocityToJointEvent &evt);

	//! Set Angular Velocity of joint. Parts name and scalar of angular velocity should be included in the message
	// The angular velocity of the parts from parent part coordinate (using joint) will be set
	// modified by inamura: change from recvRequestSetObjectVelocity, on 2013-12-30
	void recvRequestSetAngularVelocityToParts(Source &from, RequestSetAngularVelocityToPartsEvent &evt);

	void recvResultGetJointAngle(Source &from, ResultGetJointAngleEvent &);

	// added by okamoto (2011/3/9)
	void recvRequestGetJointAngle(Source &from, RequestGetJointAngleEvent &evt);

	void recvRequestGetObjectNames(Source &from, RequestGetObjectNamesEvent &evt);

	void recvResultGetObjectNames(Source &from, ResultGetObjectNamesEvent &);

	SParts * getSimObjSParts(SSimObj &obj, const char *parts);

	SParts * getSimObjSParts(const char *name, const char *parts);

	Vector3d & calcAnchor (SParts &p1, SParts &p2, Vector3d &v);

	void recvRequestConnectJoint(Source &from, RequestConnectJointEvent &evt);

	void recvRequestReleaseJoint(Source &from, RequestReleaseJointEvent &evt);

	void recvRequestSoundRecog(Source &from, RequestSoundRecogEvent &);
	void recvResultSoundRecog (Source &from, ResultSoundRecogEvent  &);

	void recvRequestGetJointForce(Source &from, RequestGetJointForceEvent &evt);

	void recvResultGetJointForce(Source &from, ResultGetJointForceEvent &);

	//added by noma@tome (2012/02/20)
	void recvRequestGetPointedObject(Source &from, RequestGetPointedObjectEvent &evt);
	void recvResultGetPointedObject(Source &from, ResultGetPointedObjectEvent &);
};


#endif // SIGVERSE_DECODER_LISTENER_H
