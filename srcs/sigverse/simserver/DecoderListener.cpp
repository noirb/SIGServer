/* 
 * Moved from WorldSimulator.cpp.
 */

#include "DecoderListener.h"

DecoderListener::DecoderListener(ServerAcceptProc &a, SimWorldProvider &p, ServiceNameServer &ns)
		: m_accept(a), m_provider(p), m_ns(ns)
{
}

void DecoderListener::notifyInit()
{
	CommInvokeMethodOnInitEncoder enc;

	const ServerAcceptProc::ConC clients = m_accept.clients();

	for (ServerAcceptProc::ConC::const_iterator i=clients.begin(); i!=clients.end(); i++) {

		Connection *con = *i;
		Source *c = con->source;
		if (c->isControllerCmd() || c->isView()) {
			c->send(enc);
		}
	}
}

void DecoderListener::recvRequestSimCtrl(Source &, RequestSimCtrlEvent &evt)
{
	SimCtrlCmdType cmd = evt.getCtrlCmd();
	SSimWorld *w = m_provider.get();

	switch(cmd) {
		case SIM_CTRL_COMMAND_START:
			if (w && !w->isRunning()) {
				w->start();
				notifyInit();
			}
			break;
		case SIM_CTRL_COMMAND_STOP:
			if (w && w->isRunning()) {
			  w->stop();
			}
			break;
		case SIM_CTRL_COMMAND_RESTART:
		/*
			070804 yoshi
			Segmentation fault. Fix later.
		*/
		/*
			createWorld();
			if (s_world) {
			s_world->start();
			notifyInit();
			}
		*/
			s_restart = true;
			break;
		default:
			assert(0);
			break;
	}
}

// Send all of the entity data to Viewer
void DecoderListener::recvRequestGetAllEntities(Source &from, RequestGetAllEntitiesEvent &evt)
{
	SSimWorld *w = m_provider.get();
	if (w) {
		//Described in commonlib/comm/encoder/Entities.h
		CommData::GetAllEntitiesResult enc(*w);
		from.send(enc);
	}
}

void DecoderListener::recvResultGetAllEntities(Source &, ResultGetAllEntitiesEvent &evt)
{
}

void DecoderListener::recvRequestGetEntity(Source &from, RequestGetEntityEvent &evt)
{
	const char *name = evt.name();
	LOG_DEBUG1(("recvRequestGetEntity from %s", name));
	SSimWorld *w = m_provider.get();
	double t = w->time();
	SSimObj *obj = w->getSObj(name);
	if (!obj) {
		LOG_ERR(("%s : no object", name));
	}
	CommResultGetEntityEncoder enc(t, obj);
	from.send(enc);
}

void DecoderListener::recvRequestUpdateEntities(Source &from, RequestUpdateEntitiesEvent &evt)
{
	SSimWorld *w = m_provider.get();
	LOG_DEBUG1(("update request"));

	const std::vector<SimObj *> &objs = evt.objs();

	for (std::vector<SimObj *>::const_iterator i=objs.begin(); i!=objs.end(); i++)
	{
		SimObj *changed = *i;
		SSimObj *obj = w->getSObj(changed->name());
		assert(obj);
		obj->copy(*changed);
	}
}

void DecoderListener::recvResultGetEntity         (Source &from, ResultGetEntityEvent &evt) {}

void DecoderListener::recvRequestAttachController (Source &from, RequestAttachControllerEvent &evt) {}
void DecoderListener::recvRequestProvideService   (Source &from, RequestProvideServiceEvent &evt) {}
void DecoderListener::recvRequestConnectDataPort  (Source &from, RequestConnectDataPortEvent &evt) {}
void DecoderListener::recvRequestAttachView       (Source &from, RequestAttachViewEvent &evt) {}

void DecoderListener::recvResultAttachController  (Source &from, ResultAttachControllerEvent &evt) {}
void DecoderListener::recvResultAttachView        (Source &from, ResultAttachViewEvent &evt) {}

void DecoderListener::recvRequestCaptureViewImage (Source &from, RequestCaptureViewImageEvent &evt) {}
void DecoderListener::recvResultCaptureViewImage  (Source &from, ResultCaptureViewImageEvent &eve) {}

void DecoderListener::recvRequestDetectEntities   (Source &from, RequestDetectEntitiesEvent &evt) {}
void DecoderListener::recvResultDetectEntities    (Source &from, ResultDetectEntitiesEvent &eve) {}

//added by okamoto@tome (2011/9/9)
void DecoderListener::recvRequestDistanceSensor   (Source &from, RequestDistanceSensorEvent &evt) {}
void DecoderListener::recvResultDistanceSensor    (Source &from, ResultDistanceSensorEvent &evt) {}

void DecoderListener::recvRequestAddForce         (Source &from, RequestAddForceEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_MSG(("addForce : no agent \"%s\"", agentName));
		return;
	}
	// Add force is not permitted in dynamic off mode
	if (!obj->dynamics()) {
		LOG_MSG(("cannot add force. Dynamics is OFF")) ;
		return;
	}
	SParts *mainParts = obj->getSBody();
	if (!mainParts) {
		LOG_MSG(("addForce : cannot get main body "));
		return;
	}
	// Set force vector in the Entity coordinate
	if (evt.rel()){
		mainParts->addRelForce(evt.x(), evt.y(), evt.z());
	// Set force vector in the global coordinate
	}else{
		mainParts->addForce(evt.x(), evt.y(), evt.z());
	}
}


void DecoderListener::recvRequestAddForceAtPos(Source &from, RequestAddForceAtPosEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_MSG(("addForce : no agent \"%s\"", agentName));
		return;
	}
	// Add torque is not permitted in dynamic off mode
	if (!obj->dynamics()) {
		LOG_MSG(("cannot add force. Dynamics is OFF")) ;
		return;
	}
	SParts *mainParts = obj->getSBody();
	if (!mainParts) {
		LOG_MSG(("addForce : cannot get main body "));
		return;
	}
	// Set relative position of the point of action
	if (evt.rel())
	{
		// Set force vector in the Entity coordinate
		if (evt.relf())
		{
			mainParts->addRelForceAtRelPos(evt.x(), evt.y(), evt.z(), evt.px(), evt.py(), evt.pz());
		}
		// Set force vector in the global coordinate
		else{
			mainParts->addForceAtRelPos(evt.x(), evt.y(), evt.z(), evt.px(), evt.py(), evt.pz());
		}
	}
	// Set global position of the point of action
	else
	{
		// Set force vector in the Entity coordinate
		if (evt.relf())
		{
			mainParts->addRelForceAtPos(evt.x(), evt.y(), evt.z(), evt.px(), evt.py(), evt.pz());
		}
		// Set force vector in the global coordinate
		else{
			mainParts->addForceAtPos(evt.x(), evt.y(), evt.z(), evt.px(), evt.py(), evt.pz());
		}
	}
}

void DecoderListener::recvRequestAddTorque (Source &from, RequestAddTorqueEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_MSG(("addTorque : no agent \"%s\"", agentName));
		return;
	}
	// Add torque is not permitted in dynamic off mode
	if (!obj->dynamics()) {
		LOG_MSG(("cannot add torque. Dynamics is OFF")) ;
		return;
	}
	SParts *mainParts = obj->getSBody();
	if (!mainParts) {
		LOG_MSG(("addTorque : cannot get main body"));
		return;
	}
	// Set torque vector in the Entity coordinate
	if (evt.rel()) {
		//mainParts->addRelTorque(evt.x(), evt.y(), evt.z());  //No addRelTorque yet
		LOG_MSG(("addRelTorque is not implemented yet"));
	}
	// Set torque vector in the global coordinate
	else{
		mainParts->addTorque(evt.x(), evt.y(), evt.z());
	}
}

void DecoderListener::recvRequestSetMass(Source &from, RequestSetMassEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_ERR(("setMass : no agent \"%s\"", agentName));
		return;
	}
	SParts *mainParts = obj->getSBody();
	if (!mainParts) {
		LOG_ERR(("setMass : cannot get main body "));
		return;
	}
	mainParts->setMass(evt.mass());
}

// Modified by inamura on 2014-01-13
void DecoderListener::recvRequestGetAngularVelocity(Source &from, RequestGetAngularVelocityEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_ERR(("getAngularVelocity : no agent \"%s\"", agentName));
		return;
	}
	SParts *mainParts = obj->getSBody();
	if (!mainParts) {
		LOG_ERR(("getAngularVelocity : cannot get main body "));
		return;
	}
	Vector3d v;
	mainParts->getAngularVelocity(v);
	CommResultGetAngularVelocityEncoder enc(agentName, v.x(), v.y(), v.z());
	from.send(enc);
}


// TODO: write the contents
void DecoderListener::recvResultGetAngularVelocity(Source &from, ResultGetAngularVelocityEvent &evt) {}


void DecoderListener::recvRequestGetLinearVelocity(Source &from, RequestGetLinearVelocityEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);

	if (!obj) {
		LOG_ERR(("getLinearVelocity : no agent \"%s\"", agentName));
		return;
	}
	SParts *mainParts = obj->getSBody();
	if (!mainParts) {
		LOG_ERR(("getLinearVelocity : cannot get main body "));
		return;
	}
	Vector3d v;
	mainParts->getLinearVelocity(v); 	// changed  by inamura on 2013-12-29
	CommResultGetLinearVelocityEncoder enc(agentName, v.x(), v.y(), v.z());
	from.send(enc);
}

// TODO: write the contents
// change to LinearVelocity: by inamura on 2013-12-29
void DecoderListener::recvResultGetLinearVelocity(Source &from, ResultGetLinearVelocityEvent &evt) {}

// Added by Tetsunari Inamura on 2014-01-06
void DecoderListener::recvRequestSetLinearVelocity(Source &from, RequestSetLinearVelocityEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);

	if (!obj) {
		LOG_MSG(("DecoderListener.cpp: SetLinearVelocity : no agent \"%s\"", agentName));
		return;
	}
	SParts *mainParts = obj->getSBody();
	if (!mainParts) {
		LOG_MSG(("DecoderListener.cpp: SetLinearVelocity : addForce : cannot get main body "));
		return;
	}

	mainParts->setLinearVelocity(evt.x(), evt.y(), evt.z());
}


void DecoderListener::recvRequestAddForceToParts(Source &from, RequestAddForceToPartsEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);

	if (!obj) {
		LOG_ERR(("addForceToParts : no agent \"%s\"", agentName));
		return;
	}
	// Add torque is not permitted in dynamic off mode
	if (!obj->dynamics()) {
		LOG_MSG(("cannot add force. Dynamics is OFF")) ;
		return;
	}
	SParts *parts = obj->getSParts(evt.getPartsName());
	if (!parts) {
		LOG_ERR(("addForceToParts : cannot get parts."));
		return;
	}
	parts->addForce(evt.x(), evt.y(), evt.z());
}


void DecoderListener::recvRequestSetGravityMode(Source &from, RequestSetGravityModeEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);

	if (!obj) {
		LOG_ERR(("setGravityMode : no agent \"%s\"", agentName));
		return;
	}
	if (!obj->dynamics()) {
		LOG_MSG(("cannot set gravity mode. Dynamics is OFF"));
		return;
	}

	int jsize = obj->getJointSize();
	if (jsize == 0) {
		SParts *mainParts = obj->getSBody();
		if (!mainParts) {
			LOG_ERR(("setGravityMode : cannot get main body "));
			return;
		}
		mainParts->setGravityMode(evt.grav());
	}
	else {
		SSimObj::PartsM partsm = obj->getAllParts();
		SSimObj::PartsM::iterator it = partsm.begin();
		while(it != partsm.end()) {
			(*it).second->setGravityMode(evt.grav());
			it++;
		}
	}
	obj->setGMode(evt.grav());
}

void DecoderListener::recvRequestGetGravityMode(Source &from, RequestGetGravityModeEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_ERR(("setGravityMode : no agent \"%s\"", agentName));
		return;
	}
	bool gmode = obj->getGMode();
	CommResultGetGravityModeEncoder enc(agentName, gmode);
	from.send(enc);
}

void DecoderListener::recvResultGetGravityMode(Source &from, ResultGetGravityModeEvent &evt) {}

void DecoderListener::recvRequestSetDynamicsMode(Source &from, RequestSetDynamicsModeEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_ERR(("setDynamicsMode : no agent \"%s\"", agentName));
		return;
	}

	int jsize = obj->getJointSize();
	if (jsize == 0) {
		SParts *mainParts = obj->getSBody();
		if (!mainParts) {
			LOG_ERR(("setDynamicsMode : cannot get main body "));
			return;
		}
		mainParts->enableDynamics(evt.dynamics());
	}
	else {
		SSimObj::PartsM partsm = obj->getAllParts();
		SSimObj::PartsM::iterator it = partsm.begin();
		while(it != partsm.end()) {
			(*it).second->enableDynamics(evt.dynamics());
			it++;
		}
	}
}


void DecoderListener::recvLogMsg(Source &from, LogMsgEvent &evt)
{
	static char buf[1024];
	sprintf(buf, "%s(%s) %s", from.name(), from.hostname(), evt.msg());

	LOG_PRINT(evt.level(), (buf));
}


void DecoderListener::recvDisplayText(Source &from, DisplayTextEvent &evt)
{
	CommDisplayTextEncoder enc(evt.getFontSize(), evt.getColor(), evt.msg());

	const ServerAcceptProc::ConC clients = m_accept.clients();

	for (ServerAcceptProc::ConC::const_iterator i=clients.begin(); i!=clients.end(); i++) {
		Connection *con = *i;
		Source *c = con->source;
		if ( c->isView()) {
			c->send(enc);
		}
	}
}

void DecoderListener::recvRequestNSQuery(Source &from, RequestNSQueryEvent &evt)
{
	LOG_DEBUG1(("DecoderListener::recvRequestNSQuery"));
	Service * service = m_ns.lookup(evt.kind());
	CommResultNSQueryEncoder enc(service);
	from.send(enc);
}

void DecoderListener::recvResultNSQuery(Source &from, ResultNSQueryEvent &) {}

void DecoderListener::recvRequestNSPinger(Source &from, RequestNSPingerEvent &) {}
void DecoderListener::recvResultNSPinger(Source &from, ResultNSPingerEvent &) {}
void DecoderListener::recvRequestSetJointAngle(Source &from, RequestSetJointAngleEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_MSG(("SetJointAngle : no agent \"%s\"", agentName));
		return;
	}
	const char *jName = evt.getJointName();
	Joint *j = obj->getJoint(jName);
	if (!j) {
		LOG_MSG(("SetJointAngle : %s does NOT have joint \"%s\"", agentName, jName));
		return;
	}
	j->setAngle(evt.getAngle());
}


// added by okamoto (2011/2/18)
void DecoderListener::recvRequestSetJointQuaternion(Source &from, RequestSetJointQuaternionEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName(); //Get Agent's name
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_MSG(("SetJointQuaternion : no agent \"%s\"", agentName));
		return;
	}

	const char *jName = evt.getJointName(); //Get the joint name specified by the event.
	Joint *j = obj->getJoint(jName);		  //Get the joint from the joint name.
	if (!j) {
		LOG_MSG(("SetJointQuaternion : %s does NOT have joint \"%s\"", agentName, jName));
		return;
	}

	// It is not permitted in dynamics on mode
	if (obj->dynamics()) {
		LOG_MSG(("Cannot Set Joint Quaternion. dynamics is true")) ;
		return;
	}

	//set OffsetQuaternion
	bool offset = (evt.getoffset() != false);
	if (offset == true) {
		Vector3d ini;
		j->setOffsetQuaternion(evt.getqw(), evt.getqx(), evt.getqy(), evt.getqz(),ini);
	}
	//set Quaternion
	else {
		j->setQuaternion(evt.getqw(), evt.getqx(), evt.getqy(), evt.getqz());
	}
}

// added by okamoto (2011/3/4)
void DecoderListener::recvRequestAddJointTorque(Source &from, RequestAddJointTorqueEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);

	if (!obj) {
		LOG_MSG(("addJointTorque : no agent \"%s\"", agentName));
		return;
	}
	if (!obj->dynamics()) {
		LOG_MSG(("Cannot Add Joint Torque. Dynamics is OFF")) ;
		return;
	}
	const char *jName = evt.getJointName(); // Get name of joint
	Joint *j = obj->getJoint(jName);        // Get joint object
	if (!j) {
		LOG_MSG(("addJointTorque : %s does NOT have joint \"%s\"", agentName, jName));
		return;
	}
	if (j->type() != Joint::TYPE_HINGE) {
		LOG_MSG(("addJointTorque : %s is NOT hinge joint", jName));
		return;
	}
	double torque = evt.getTorque();
	j->addTorque(torque);
}


//! Set Angular Velocity of joint. Joint name, angular velocity and max torque should be included in the message
// Set angular velocity of joint, added by okamoto (2011/3/9)
// modified by inamura: change from recvRequestSetJointVelocity
void DecoderListener::recvRequestSetAngularVelocityToJoint(Source &from, RequestSetAngularVelocityToJointEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName(); // get agent's name
	SSimObj *obj = w->getSObj(agentName);       // get object in simulation server

	if (!obj) {
		LOG_MSG(("setAngularVelocityToJoint : no agent \"%s\"", agentName));
		return;
	}
	// set angular velocity is only available in dynamics on mode
	if (!obj->dynamics()) {
		LOG_MSG(("Cannot Set Joint Velocity. Dynamics is OFF")) ;
		return;
	}
	const char *jName = evt.getJointName(); // Get joint name
	Joint *j = obj->getJoint(jName);        // Get joint object
	if (!j) {
		LOG_MSG(("setAngularVelocityToJoint : %s does NOT have joint \"%s\"", agentName, jName));
		return;
	}
	if (j->type() == Joint::TYPE_HINGE) {
		double velocity = evt.getAngularVelocity(); // modified by inamura: change from getVelocity
		double max      = evt.getMaxTorque();       // Get max joint torque
		HingeJoint* hj  = (HingeJoint*)j;
		hj->setAngularVelocityMaxTorque(velocity,max);       // Set angular velocity and max torque
		// modified by inamura on 2013-12-29: change from setVelocity
	}
	else {
		LOG_MSG(("setJointVelocity : %s is NOT hinge joint", jName));
		return;
	}
}


//! Set Angular Velocity of joint. Parts name and scalar of angular velocity should be included in the message
// The angular velocity of the parts from parent part coordinate (using joint) will be set
// modified by inamura: change from recvRequestSetObjectVelocity, on 2013-12-30
void DecoderListener::recvRequestSetAngularVelocityToParts(Source &from, RequestSetAngularVelocityToPartsEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);
	if (!obj) {
		LOG_MSG(("setAngularVelocityToParts : no agent \"%s\"", agentName));
		return;
	}

	// Angular velocity of wheel should be set in dynamics ON mode
	if (!obj->dynamics()) {
		LOG_MSG(("Cannot set Velocity to the parts. Dynamics is OFF")) ;
		return;
	}

	const char *partsName = evt.getPartsName();       // Get parts name
	SParts *partsData     = obj->getSParts(partsName);

	if (!partsData) {
		LOG_MSG(("setAngularVelocityToParts : %s does NOT have parts \"%s\"",agentName, partsName));
		return;
	}

	double velocity = evt.getAngularVelocity(); // modified by inamura: change from getVelocity
	//double max      = evt.getMax();      // Get maximum angular velocity (not implemented yet)

	// Get parent joint of the target parts
	Joint *j = partsData->getParentJoint();
	//partsData->printJointDataList();

	if (j->type() == Joint::TYPE_HINGE) {
		double xAx =  ((HingeJoint*)j)->getAxisX();
		double yAx =  ((HingeJoint*)j)->getAxisY();
		double zAx =  ((HingeJoint*)j)->getAxisZ();
		double vax = xAx * velocity;
		double vay = yAx * velocity;
		double vaz = zAx * velocity;
		//partsData->setAngularMaxVelocity(max);

		// Set Angular Velocity in ODE world
		partsData->setAngularVelocity(vax,vay,vaz);
	}
	else {
		LOG_MSG(("parts don't have hinge joint.\n"));
	}
}


void DecoderListener::recvResultGetJointAngle(Source &from, ResultGetJointAngleEvent &) {};

// added by okamoto (2011/3/9)
void DecoderListener::recvRequestGetJointAngle(Source &from, RequestGetJointAngleEvent &evt)
{
	SSimWorld *w = m_provider.get();
	const char *agentName = evt.getAgentName();
	SSimObj *obj = w->getSObj(agentName);

	if (!obj) {
		LOG_MSG(("getJointAngle : no agent \"%s\"", agentName));
		return;
	}

	const char *jName = evt.getJointName();
	Joint *j = obj->getJoint(jName);
	if (!j) {
		LOG_MSG(("getJointAngle : %s does NOT have joint \"%s\"", agentName, jName));
		return;
	}
	if (j->type() == Joint::TYPE_HINGE) {
		HingeJoint* hj = (HingeJoint*)j;
		double angle = hj->getAngle();		  // Get angle
		CommResultGetJointAngleEncoder enc(angle);
		from.send(enc);
	}
	else {
		LOG_MSG(("getJointAngle : %s is NOT hinge joint", jName));
		return;
	}
}


void DecoderListener::recvRequestGetObjectNames(Source &from, RequestGetObjectNamesEvent &evt)
{
	SSimWorld *w = m_provider.get();
	SSimWorld::NameC names;
	w->copyNames(evt.getEntities(), names);

	CommResultGetObjectNamesEncoder enc;
	enc.push(names);

	from.send(enc);
}


void DecoderListener::recvResultGetObjectNames(Source &from, ResultGetObjectNamesEvent &) {}

SParts * DecoderListener::getSimObjSParts(SSimObj &obj, const char *parts)
{
	if (parts) {
		return obj.getSParts(parts);
	} else {
		return obj.getSBody();
	}
}

SParts * DecoderListener::getSimObjSParts(const char *name, const char *parts)
{
	SSimWorld *w = m_provider.get();
	SSimObj *obj = w->getSObj(name);
	if (!obj) { return NULL; }
	return getSimObjSParts(*obj, parts);
}

Vector3d & DecoderListener::calcAnchor (SParts &p1, SParts &p2, Vector3d &v)
{
	const dReal *v1 = p1.getPosition();
	const dReal *v2 = p2.getPosition();
	v.set(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
	v /= 2.0;
	return v;
}

void DecoderListener::recvRequestConnectJoint(Source &from, RequestConnectJointEvent &evt)
{
	const char *jointName       = evt.getJointName();
	const char *myName          = evt.getAgentName();
	const char *myPartsName     = evt.getAgentPartsName();
	const char *targetName      = evt.getTargetName();
	const char *targetPartsName = evt.getTargetPartsName();

	SSimWorld *w = m_provider.get();
	SSimObj *my  = w->getSObj(myName);

	if (my->getJoint(jointName) != NULL) {
		return;
	}

	SParts *myParts = getSimObjSParts(*my, myPartsName);
	if (!myParts) { return; }

	SParts *targetParts = getSimObjSParts(targetName, targetPartsName);
	if (!targetParts) { return; }
	//if (myParts->isConnected(*targetParts)) { return; }

	FixedJoint *j = new FixedJoint(jointName);
	j->setWorld(w->odeWorld()->world());
	j->pushBody(myParts->odeobj().body());
	j->pushBody(targetParts->odeobj().body());

	Vector3d anchor;
	calcAnchor(*myParts, *targetParts, anchor);
	j->setAnchor(anchor.x(), anchor.y(), anchor.z());

	{
		Vector3d v(my->x(), my->y(), my->z());
		Rotation r;
		r.setQuaternion(my->qw(), my->qx(), my->qy(), my->qz());
		j->build(v, r, my->dynamics());
	}
	my->push(j);
	myParts->pushChild(j, targetParts);
}

void DecoderListener::recvRequestReleaseJoint(Source &from, RequestReleaseJointEvent &evt)
{
	const char *myName = evt.getAgentName();
	const char *jointName = evt.getJointName();

	SSimWorld *w = m_provider.get();
	SSimObj *my = w->getSObj(myName);
	Joint *joint = my->getJoint(jointName);
	if (joint == NULL) {
		return;
	}

	SParts *p = joint->getParts(0);
	assert(p != NULL);
	bool b = p->removeChild(joint);
	assert(b);
	p = joint->getParts(1);
	assert(p != NULL);
	b = p->removeChild(joint);
	assert(!b);
	my->removeJoint(joint);
	delete joint;
}


void DecoderListener::recvRequestSoundRecog(Source &from, RequestSoundRecogEvent &) {};
void DecoderListener::recvResultSoundRecog (Source &from, ResultSoundRecogEvent  &) {};

void DecoderListener::recvRequestGetJointForce(Source &from, RequestGetJointForceEvent &evt)
{
	const char *agentName = evt.getAgentName();
	SSimWorld *w = m_provider.get();
	SSimObj *obj = w->getSObj(agentName);

	bool success = false;
	CommResultGetJointForceEncoder enc;
	if (!obj) {
		LOG_ERR(("getJointForce : \"%s\" no agent", agentName));
		goto jferror;
	}
	if (!obj->dynamics()) {
		LOG_ERR(("getJointForce : \"%s\" dynamics is off", agentName));
		goto jferror;
	}

	{
		const char *jointName = evt.getJointName();
		Joint *j = obj->getJoint(jointName);
		if (!j) {
			LOG_ERR(("getJointForce : \"%s(%s)\" : no joint", agentName, jointName));
			goto jferror;
		}

		JointForce * jfs = enc.getJointForces();
		if (!j->getJointForces(jfs)) {
			LOG_ERR(("getJointForce : \"%s(%s)\" cannot get force", agentName, jointName));
			goto jferror;
		}
	}
	success = true;
jferror:
	enc.success(success);
	from.send(enc);
}


void DecoderListener::recvResultGetJointForce(Source &from, ResultGetJointForceEvent &) {};

//added by noma@tome (2012/02/20)
void DecoderListener::recvRequestGetPointedObject(Source &from, RequestGetPointedObjectEvent &evt)
{
	using namespace std;

	SSimWorld *w = m_provider.get();
	SSimObj *speakerObj = w->getSObj(evt.getSpeakerName());

	// Get starting and ending point
	Joint* tmpParentJoint = speakerObj->getSParts(evt.getPartsName1().c_str())->getParentJoint();
	Vector3d startPointVec = tmpParentJoint->getCurrentAnchorPosition();
	tmpParentJoint = speakerObj->getSParts(evt.getPartsName2().c_str())->getParentJoint();
	Vector3d endPointVec = tmpParentJoint->getCurrentAnchorPosition();

	// Set of pointing vector
	Vector3d pointingVector = Vector3d(endPointVec.x() - startPointVec.x(), endPointVec.y() - startPointVec.y(), endPointVec.z() - startPointVec.z());

	// Loop for all of the entity
	// Get map of entity
	const std::map<std::string, SimObjBase*> & objMap = w->objs();
	std::map<std::string, SimObjBase*>::const_iterator mapp;

	// Map of candidate entity (Default is ascending-order)
	std::multimap<double, const char*> candidateMap;

	for (mapp = objMap.begin(); mapp!=objMap.end(); mapp++) {
		SSimObj* tmpSSimobj = (SSimObj*)mapp->second;
		// Get position of entity
		SParts *tmpSparts = tmpSSimobj->getSBody();
		const dReal* entityPosition = tmpSparts->getPosition();
		Vector3d entityPositionVector = Vector3d(entityPosition[0] - startPointVec.x(), entityPosition[1] - startPointVec.y(), entityPosition[2] - startPointVec.z());

		// Get typical length of the entity
		double typicalRadius;
		switch(evt.getTypicalType()) {
			case 0:
				// Do nothing
				typicalRadius = 0;
				break;
			case 1:
				// Radius of circumscribed sphere
				typicalRadius = tmpSparts->getCircumRadius();
				break;
			case 2:
				// Cube root of volume
				typicalRadius = tmpSparts->getCubicRootOfVolume();
				break;
			default:
				// For safety
				typicalRadius = 1024;
		}

		// Calculation of minimum distance between the entity and half-line
		// Calculation of inner product
		double cosine;
		if (pointingVector.length()<0.00001) {
			cosine = 0;
		} else {
			cosine = pointingVector.angle(entityPositionVector);
		}

		// If the cosine is negative, it should be terminated from candidates
		if (cosine <=0) {
		  continue;
		}

		// Minimum distance is sine * distance
		double deviation;
		deviation = sqrt(1-cosine*cosine) * entityPositionVector.length();

		// subtract of typical length
		double modifiedDeviation = deviation - typicalRadius;
		if (modifiedDeviation <0) {
			modifiedDeviation = 0;
		}

		// Insert to the candidate map
		candidateMap.insert(pair<double, const char*>(modifiedDeviation, tmpSSimobj->name()));

	}
	// END of all of the entity

	// preparation of string as return value
	std::vector<std::string> returnVector;
	multimap<double, const char*>::iterator p;
	for (p = candidateMap.begin(); p != candidateMap.end(); p++) {
		returnVector.push_back(p->second);
	}

	// Encoder class for response
	CommResultGetPointedObjectEncoder enc;
	// push the name into the child class of the encoder
	enc.push(returnVector);
	// execute the sending
	from.send(enc);

	return;
}

void DecoderListener::recvResultGetPointedObject(Source &from, ResultGetPointedObjectEvent &)
{
}

