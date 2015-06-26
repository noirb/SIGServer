/* 
 * Written by okamoto on 2012-06-14
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add English comments (not finished yet)
 *    set/getVelocity is changed to set/getLinearVelocity
 * Modified by Tetsunari Inamura on 2014-01-04
 *    Add English comments (Main important translation is finished)
 * Modified by Tetsunari Inamura on 2014-01-05
 *    Fixed memory leak bug
 * Add recvRequestSetLinearVelocity by Tetsunari Inamura on 2014-01-06
 * Modified by Tetsunari Inamura on 2014-01-08
 *    Using new enum for new communication protocal
 *    Fixed time over flow problem
 * Modified by Tetsunari Inamura on 2014-02-26
 *    Added addTorque
 */
   
#ifdef IRWAS_SIMSERVER

#include "WorldSimulator.h"
#include "ServerAcceptProc.h"
#include "PacketForward.h"
#include "LogTransfer.h"
#include "Logger.h"
#include "CommDataEncoder.h"
#include "SSimWorld.h"
#include "CommUtil.h"
#include "SimWorldProvider.h"
#include "X3DDB.h"
//#include "SimObjBase.h"

#include "SimObj.h"
#include "Joint.h"
#include "ServiceNameServer.h"

#include "ODEObj.h"
#include "binary.h"

#include <sys/types.h>
#include <errno.h>

#ifndef WIN32
#include <sys/time.h>
#else


void gettimeofday(struct timeval *tv, struct timezone *tz);

int wait_socket_to_read(SOCKET sock, int msec)
{
	int nfds;
	fd_set socks;
	int stat;
	struct timeval tv;

	FD_ZERO(&socks);
	FD_SET(sock, &socks);
	nfds = sock+1;

	tv.tv_sec = msec / 1000;
	tv.tv_usec = (msec - tv.tv_sec * 1000) * 1000;

	stat = select(nfds, &socks, 0, 0, &tv);

	return stat;
}

#endif

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT	0
#endif

static bool s_restart = false;

class DecoderListener : public CommDataDecoder::Listener
{
private:
	ServerAcceptProc  &m_accept;
	SimWorldProvider  &m_provider;
	ServiceNameServer &m_ns;
public:
	DecoderListener(ServerAcceptProc &a, SimWorldProvider &p, ServiceNameServer &ns)
		: m_accept(a), m_provider(p), m_ns(ns) {}
public:
	void notifyInit()
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
private:
	void recvRequestSimCtrl(Source &, RequestSimCtrlEvent &evt)
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
	void recvRequestGetAllEntities(Source &from, RequestGetAllEntitiesEvent &evt)
	{
		SSimWorld *w = m_provider.get();
		if (w) {
			//Described in commonlib/comm/encoder/Entities.h
			CommData::GetAllEntitiesResult enc(*w);
			from.send(enc);
		}
	}

	void recvResultGetAllEntities(Source &, ResultGetAllEntitiesEvent &evt) {;}

	void recvRequestGetEntity(Source &from, RequestGetEntityEvent &evt)
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

	void recvRequestUpdateEntities(Source &from, RequestUpdateEntitiesEvent &evt)
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
	void recvResultGetEntity         (Source &from, ResultGetEntityEvent &evt) {;}

	void recvRequestAttachController (Source &from, RequestAttachControllerEvent &evt) {;}
	void recvRequestProvideService   (Source &from, RequestProvideServiceEvent &evt) {;}
	void recvRequestConnectDataPort  (Source &from, RequestConnectDataPortEvent &evt) {;}
	void recvRequestAttachView       (Source &from, RequestAttachViewEvent &evt) {;}

	void recvResultAttachController  (Source &from, ResultAttachControllerEvent &evt) {;}
	void recvResultAttachView        (Source &from, ResultAttachViewEvent &evt) {;}

	void recvRequestCaptureViewImage (Source &from, RequestCaptureViewImageEvent &evt) {;}
	void recvResultCaptureViewImage  (Source &from, ResultCaptureViewImageEvent &eve) {;}

	void recvRequestDetectEntities   (Source &from, RequestDetectEntitiesEvent &evt) {};
	void recvResultDetectEntities    (Source &from, ResultDetectEntitiesEvent &eve) {};

	//added by okamoto@tome (2011/9/9)
	void recvRequestDistanceSensor   (Source &from, RequestDistanceSensorEvent &evt) {};
	void recvResultDistanceSensor    (Source &from, ResultDistanceSensorEvent &evt) {};

	void recvRequestAddForce         (Source &from, RequestAddForceEvent &evt) 
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
		if (evt.rel())
			mainParts->addRelForce(evt.x(), evt.y(), evt.z());
		// Set force vector in the global coordinate
		else
			mainParts->addForce(evt.x(), evt.y(), evt.z());
	}


	void recvRequestAddForceAtPos(Source &from, RequestAddForceAtPosEvent &evt) 
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
			else
				mainParts->addForceAtRelPos(evt.x(), evt.y(), evt.z(), evt.px(), evt.py(), evt.pz());
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
			else
				mainParts->addForceAtPos(evt.x(), evt.y(), evt.z(), evt.px(), evt.py(), evt.pz());
		}
	}


	void recvRequestAddTorque (Source &from, RequestAddTorqueEvent &evt) 
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
		else
			mainParts->addTorque(evt.x(), evt.y(), evt.z());
	}


	void recvRequestSetMass(Source &from, RequestSetMassEvent &evt) 
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
	void recvRequestGetAngularVelocity(Source &from, RequestGetAngularVelocityEvent &evt)
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
	void recvResultGetAngularVelocity(Source &from, ResultGetAngularVelocityEvent &evt) {};


	void recvRequestGetLinearVelocity(Source &from, RequestGetLinearVelocityEvent &evt) 
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
	void recvResultGetLinearVelocity(Source &from, ResultGetLinearVelocityEvent &evt) {};

	// Added by Tetsunari Inamura on 2014-01-06
	void recvRequestSetLinearVelocity(Source &from, RequestSetLinearVelocityEvent &evt) 
	{
		SSimWorld *w = m_provider.get();
		const char *agentName = evt.getAgentName();
		SSimObj *obj = w->getSObj(agentName);

		if (!obj) {
			LOG_MSG(("WorldSimulator.cpp: SetLinearVelocity : no agent \"%s\"", agentName));
			return;
		}
		SParts *mainParts = obj->getSBody();
		if (!mainParts) {
			LOG_MSG(("WorldSimulator.cpp: SetLinearVelocity : addForce : cannot get main body "));
			return;
		}

		mainParts->setLinearVelocity(evt.x(), evt.y(), evt.z());
	}


	void recvRequestAddForceToParts(Source &from, RequestAddForceToPartsEvent &evt) 
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


	void recvRequestSetGravityMode(Source &from, RequestSetGravityModeEvent &evt)
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

	void recvRequestGetGravityMode(Source &from, RequestGetGravityModeEvent &evt)
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

	void recvResultGetGravityMode(Source &from, ResultGetGravityModeEvent &evt) {};

	void recvRequestSetDynamicsMode(Source &from, RequestSetDynamicsModeEvent &evt) 
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


	void recvLogMsg(Source &from, LogMsgEvent &evt)
	{
		static char buf[1024];
		sprintf(buf, "%s(%s) %s", from.name(), from.hostname(), evt.msg());

		LOG_PRINT(evt.level(), (buf));
	}


	void recvDisplayText(Source &from, DisplayTextEvent &evt)
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

	void recvRequestNSQuery(Source &from, RequestNSQueryEvent &evt)
	{
		LOG_DEBUG1(("WorldSimulator::recvRequestNSQuery"));
		Service * service = m_ns.lookup(evt.kind());
		CommResultNSQueryEncoder enc(service);
		from.send(enc);
	}

	void recvResultNSQuery(Source &from, ResultNSQueryEvent &) {}

	void recvRequestNSPinger(Source &from, RequestNSPingerEvent &) {}
	void recvResultNSPinger(Source &from, ResultNSPingerEvent &) {}
	void recvRequestSetJointAngle(Source &from, RequestSetJointAngleEvent &evt)
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
	void recvRequestSetJointQuaternion(Source &from, RequestSetJointQuaternionEvent &evt)
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
	void recvRequestAddJointTorque(Source &from, RequestAddJointTorqueEvent &evt)
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
	void recvRequestSetAngularVelocityToJoint(Source &from, RequestSetAngularVelocityToJointEvent &evt)
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
	void recvRequestSetAngularVelocityToParts(Source &from, RequestSetAngularVelocityToPartsEvent &evt)
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


	void recvResultGetJointAngle(Source &from, ResultGetJointAngleEvent &) {};

	// added by okamoto (2011/3/9)
	void recvRequestGetJointAngle(Source &from, RequestGetJointAngleEvent &evt)
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


	void recvRequestGetObjectNames(Source &from, RequestGetObjectNamesEvent &evt)
	{
		SSimWorld *w = m_provider.get();
		SSimWorld::NameC names;
		w->copyNames(evt.getEntities(), names);

		CommResultGetObjectNamesEncoder enc;
		enc.push(names);

		from.send(enc);
	}


	void recvResultGetObjectNames(Source &from, ResultGetObjectNamesEvent &) {}

	SParts * getSimObjSParts(SSimObj &obj, const char *parts)
	{
		if (parts) {
			return obj.getSParts(parts);
		} else {
			return obj.getSBody();
		}
	}

	SParts * getSimObjSParts(const char *name, const char *parts)
	{
		SSimWorld *w = m_provider.get();
		SSimObj *obj = w->getSObj(name);
		if (!obj) { return NULL; }
		return getSimObjSParts(*obj, parts);
	}

	Vector3d & calcAnchor (SParts &p1, SParts &p2, Vector3d &v)
	{
		const dReal *v1 = p1.getPosition();
		const dReal *v2 = p2.getPosition();
		v.set(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
		v /= 2.0;
		return v;
	}

	void recvRequestConnectJoint(Source &from, RequestConnectJointEvent &evt)
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

	void recvRequestReleaseJoint(Source &from, RequestReleaseJointEvent &evt)
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


	void recvRequestSoundRecog(Source &from, RequestSoundRecogEvent &) {};
	void recvResultSoundRecog (Source &from, ResultSoundRecogEvent  &) {};

	void recvRequestGetJointForce(Source &from, RequestGetJointForceEvent &evt)
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


	void recvResultGetJointForce(Source &from, ResultGetJointForceEvent &) {};

	//added by noma@tome (2012/02/20)
	void recvRequestGetPointedObject(Source &from, RequestGetPointedObjectEvent &evt)
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

	void recvResultGetPointedObject(Source &from, ResultGetPointedObjectEvent &) {};
};

//////////////////////////////////////////////////////

WorldSimulator::WorldSimulator(ServerAcceptProc &accept, SimWorldProvider &provider, ServiceNameServer &ns)
	: m_accept(accept), m_provider(provider), m_ns(ns), m_l(0), m_log(0), m_loop(false), stepCount(0)
{
	// Create an instance to decode the data
	m_l = new DecoderListener(accept, provider, ns);

	// Instance of packet transport class
	m_forward = new PacketForward(accept, provider);

	// Registration to request acception procedures
	accept.setDecoderListener(m_l);
	accept.setPacketSender(m_forward);

	// Instance of log transport class
	m_log = new LogTransfer(accept);
	LOG_LISTENER(m_log);
}



#define FREE(P) if (P) { delete P; P = 0; }

void WorldSimulator::free_()
{
	FREE(m_l);
	FREE(m_forward);
	FREE(m_log);
}



static void wait(int usec)
{
	// 070807 yoshi
	// sleep based on select. usleep is not accurate
#ifndef WIN32
	fd_set	fds;
	FD_ZERO(&fds);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = usec;
	select(0, &fds, 0, 0, &tv);
#else
	int msec = usec/1000;
	if(msec > 0){ Sleep(msec); }
	return;
#endif
}

std::string WorldSimulator::DoubleToString(double x)
{
	// Caution!: max is 32 bytes
	char tmp[32];
	sprintf(tmp,"%.3f",x);
	std::string str = std::string(tmp);
	str += ",";
	return str;
}

std::string WorldSimulator::IntToString(int x)
{
	char tmp[32];
	sprintf(tmp,"%d",x);
	std::string str = std::string(tmp);
	str += ",";
	return str;
}

bool WorldSimulator::sendData(SOCKET sock, const char* msg, int size)
{
	int sended = 0;

	// Response to a case in which the data could not be sent once
	while(1) {
#ifndef WIN32
		int r = send(sock, msg + sended, size - sended, MSG_DONTWAIT);
		// Sending failed
		if (r < 0) {
			if ( errno == EAGAIN || errno == EWOULDBLOCK ) {

				usleep(100);

				continue;
			}
			LOG_ERR(("Failed to send data. erro[%d] [%s:%d]", r,  __FILE__, __LINE__ ));
			return false;
		}
#else
		int r = send(sock, msg + sended, size - sended, 0);
		// Sending failed
		if ( r ==  SOCKET_ERROR) {
			int err = WSAGetLastError();
			fprintf(stderr, "Error in sendData with error ; %d\n", err);
			LOG_ERR(("Failed to send data. erro[%d] [%s:%d]", r,  __FILE__, __LINE__ ));
			return false;
		}
#endif
		// The number of bytes already sent
		sended += r;

		// Check whether all of the data is sent
		if (size == sended) break;
	}
	return true;
}

int recv_nonblock(SOCKET sock, char *msg, int size)
{
#ifndef WIN32
	return recv(sock, msg, size, MSG_DONTWAIT);
#else
	int ret = wait_socket_to_read(sock, 1);

	if(ret == 0){
		return -1;
	}else if (ret == SOCKET_ERROR){
		return 0;
	}else if (ret > 0){
		return recv(sock, msg, size, 0);
	}else{
		fprintf(stderr, "== Unknown error ==\n");
	}
	return 0;
#endif
}

bool WorldSimulator::recvData(SOCKET sock, char* msg, int size)
{
	int recieved = 0;

	// Response to a case in which the data could not be sent once
	while(1) {
		// Sending
		int r = recv_nonblock(sock, msg + recieved, size - recieved);

#ifndef WIN32
		if (r < 0) {

			// In case the socket is not available
			if ( errno == EAGAIN || errno == EWOULDBLOCK ) {

				usleep(1000);

				continue;
			}

			LOG_ERR(("Failed to recieve data. erro[%d]",r));
			return false;
		}
#else
		if (r < 0) {
			Sleep(1);

			continue;
//			LOG_ERR(("Failed to recieve data. erro[%d]",r));
//			return false;
		}
#endif
		// The number of bytes already sent
		recieved += r;
		// Check whether all of the data is sent
		if (size == recieved) break;
	}
	return true;
}

bool WorldSimulator::sendOnMsg(SOCKET sock, std::string data, std::string from)
{
	int strPos1 = 0;
	int strPos2;
	std::string tmpss;
	std::string msg;

	// Get size of the message
	strPos2 = data.find(",", strPos1);
	tmpss.assign(data, strPos1, strPos2-strPos1);
	int msgSize = atoi(tmpss.c_str());

	strPos1 = strPos2 + 1;

	// Add size of the message 
	msg = from + "," + data;

	// Prepare the buffer for message sending
	int sendSize = msg.size() + sizeof(unsigned short) * 2;
	char *sendBuff = new char[sendSize];
	char *p = sendBuff;

	// Add header and data size
	BINARY_SET_DATA_S_INCR(p, unsigned short, SEND_MESSAGE);
	BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);

	// Prepare the sending message
	memcpy(p, msg.c_str(), msg.size());

	// Get the number of sending entites
	strPos1 = strPos1 + msgSize + 1;
	strPos2 = data.find(",", strPos1);
	tmpss.assign(data, strPos1, strPos2-strPos1);

	// The maximum distance the message could be reached
	double distance = atof(tmpss.c_str());

	// Get the number of sending entites
	strPos1 = strPos2 + 1;
	strPos2 = data.find(",", strPos1);
	tmpss.assign(data, strPos1, strPos2-strPos1);

	// Flag to execute the check whether the entity is located within a reaching distance
	bool isDistance = false;

	// Position of source of sending (It is not used if the avobe check is not executed)
	Vector3d fpos; 
	SSimWorld *w;

	// Check the reaching area
	if (distance >= 0) {

		w = m_provider.get();

		// Get position of the source
		SimObj *fobj = (SimObj*)w->getObj(from.c_str());
		fobj->getPosition(fpos);
		// Execute the check of reaching area
		isDistance = true;
	}
	// The number of entites to be sent
	int entNum = atoi(tmpss.c_str());

	// broadcast
	if (entNum == -1) {
		std::vector<Source*> srcs = m_accept.getAllClients();
		int size = srcs.size();

		for (int i = 0; i < size; i++) {
			// In a case that reaching area is checked with controller
			if (isDistance && srcs[i]->type() == SOURCE_TYPE_CONTROLLER_CMD) {
				// Get destination entity
				SimObj *tobj = (SimObj*)w->getObj(srcs[i]->name());
				if (!tobj) {
					LOG_ERR(("sendMsg(): cannot find entity \"%s\" [%s,%d]", tmpss.c_str(), __FILE__, __LINE__));
					continue;
				}
	
				Vector3d tpos;
				tobj->getPosition(tpos);

				// Calc. of distance between source and destination
				double dx = tpos.x() - fpos.x();
				double dy = tpos.y() - fpos.y();
				double dz = tpos.z() - fpos.z();
				double ftos = sqrt(dx*dx + dy*dy + dz*dz);
				// If the destination if out of reaching area, the message is not sent
				if (ftos > distance) {
					continue;
				}
			}  // if (isDistance && srcs[i]->type() == SOURCE_TYPE_CONTROLLER_CMD) {
			// The message is not sent to itself
			if (strcmp(srcs[i]->name(), from.c_str()) == 0) continue;

			// Sending the message to target entity
			if (!sendData(srcs[i]->socket(), sendBuff, sendSize)) {
				LOG_ERR(("SendMsg to \"%s\" failed [%s,%d]", srcs[i]->name(), __FILE__, __LINE__));
				continue;
			}
		} // for (int i = 0; i < size; i++) {
	} // if (entNum == -1) {
	// Broadcast to only services
	else if (entNum == -2) {
		std::vector<Source*> srcs = m_accept.getAllSrvClients();
		int size = srcs.size();

		for (int i = 0; i < size; i++) {
			// Sending the message to entity
			if (!sendData(srcs[i]->socket(), sendBuff, sendSize)) {
				LOG_ERR(("SendMsg to \"%s\" failed [%s,%d]", srcs[i]->name(), __FILE__, __LINE__));
				continue;
			}
		}
	}
	else if (entNum == -3) {
		std::vector<Source*> srcs = m_accept.getAllCtlClients();
		int size = srcs.size();

		for (int i = 0; i < size; i++) {
			// In a case that reaching area is checked with controller
			if (isDistance) {

				// Get target entity for sending
				SimObj *tobj = (SimObj*)w->getObj(srcs[i]->name());
				if (!tobj) {
					LOG_ERR(("sendMsg(): cannot find entity \"%s\" [%s,%d]", tmpss.c_str(), __FILE__, __LINE__));
					continue;
				}
	
				Vector3d tpos;
				tobj->getPosition(tpos);
				// Distance between source and destination
				double dx   = tpos.x() - fpos.x();
				double dy   = tpos.y() - fpos.y();
				double dz   = tpos.z() - fpos.z();
				double ftos = sqrt(dx*dx + dy*dy + dz*dz);
	
				// Don't send if the destination is outside of the reaching area 
				if (ftos > distance) {
					continue;
				}
			}  // if (isDistance && srcs[i]->type() == SOURCE_TYPE_CONTROLLER_CMD) {
			// Do not send to itself
			if (strcmp(srcs[i]->name(), from.c_str()) == 0) continue;

			// Send the message to entity
			if (!sendData(srcs[i]->socket(), sendBuff, sendSize)) {
			  LOG_ERR(("SendMsg to %s failed", srcs[i]->name()));
			  continue;
			}
		} // for (int i = 0; i < size; i++) {
	} // if (entNum == -3) {

	// When the destination is determined
	else {
		// loop for all of the target entities
		for (int i = 0; i < entNum; i++) {
			// Name of entity
			strPos1 = strPos2 + 1;
			strPos2 = data.find(",", strPos1);
			tmpss.assign(data, strPos1, strPos2-strPos1);

			// Get the target 
			Source *target = m_accept.get(tmpss.c_str(), SOURCE_TYPE_CONTROLLER_CMD);
			if (!target) {
				// Search from message service is the target controller is not found
				target = m_accept.get(tmpss.c_str(), SOURCE_TYPE_MESSAGE);
				if (!target) {
					LOG_ERR(("sendMsg(): cannot find target [%s]", tmpss.c_str()));
					continue;
				}
			}
			if (isDistance && target->type() == SOURCE_TYPE_CONTROLLER_CMD) {
				// Get the destination entity
				SimObj *tobj = (SimObj*)w->getObj(tmpss.c_str());
				if (!tobj) {
					LOG_ERR(("sendMsg(): cannot find entity [%s]", tmpss.c_str()));
					continue;
				}
	
				Vector3d tpos;
				tobj->getPosition(tpos);
				// Calculation of distance between source and destination
				double dx = tpos.x() - fpos.x();
				double dy = tpos.y() - fpos.y();
				double dz = tpos.z() - fpos.z();
				double ftos = sqrt(dx*dx + dy*dy + dz*dz);
				
				// Don't send if the destination is outside of the reaching area 
				if (ftos > distance) {
				  continue;
				}
			}
			// Send the message to entity
			if (!sendData(target->socket(), sendBuff, sendSize)) {
				LOG_ERR(("sendMsg to %s failed", target->name()));
				return false;
			}
		} // for (int i = 0; i < entNum; i++) {  
	} // else if {
	delete [] sendBuff;
	return true;
}


bool WorldSimulator::sendAllEntities(SOCKET sock, const char* buf)
{
	SSimWorld *w = m_provider.get();

	// Version check
	bool ver = w->getV21();
	// Old version
	if (w && !ver) {
	  // Get the map for object name and SimObjBase
		typedef std::map<std::string, SimObjBase*> EMap;
		EMap emap = w->objs(); 

		// Prepare sending message buffer
		std::string msg = "";

		// Whether the simulation is already started
		if (w->isRunning()) msg += "1,";
		else                msg += "0,";

		// Get the message clients which is connected
		const std::vector<Connection*> &messages = m_accept.messages();
		int msg_size = messages.size();

		// Add the number of other services
		msg += IntToString(msg_size);

		for (int i = 0; i < msg_size; i++) {
			Source *message = messages[i]->source;
			// Add the service name
			std::string name = std::string(message->name());
			msg += name + ",";
		}
		// Add the number of entities
		msg += IntToString(emap.size());

		// Get the iterator of entity map
		EMap::iterator it = emap.begin();

		// Get x3d file name management database
		X3DDB &db = m_provider.getX3DDB();

		/////////////////////////////////
		// Loop for entities
		/////////////////////////////////
		while(it != emap.end()) {
			// Get SIGVerse object 
			SimObjBase *obj = (*it).second;

			// Add ID number
			msg += IntToString(obj->id());

			// Add scale
			msg += DoubleToString(obj->scalex());
			msg += DoubleToString(obj->scaley());
			msg += DoubleToString(obj->scalez());

			// Add SSimObj
			const char *agentName = (*it).first.c_str();
			SSimObj *sobj = w->getSObj(agentName);

			// Add information whether the joint is included
			int jsize = sobj->getJointSize();
			if (jsize == 0) msg += "0,";
			else            msg += "1,";

			// Add entity name
			msg += (*it).first + ","; 

			// shape file name
			std::string filename;

			// Whether the shape file is configured by world file directly
			bool dUse = obj->isAttr("shapeFile");
			if (obj->isAttr("shapeFile")) {
				filename = obj->shapeFile();
				msg += "1," + filename + ",";
			}
			else {
				// Get class name
				std::string classname = obj->classname();
				std::string cname = classname + ":::";

				// In case of object which change the shape file accordnig to its state
				std::string attrname = obj->getAttr("visStateAttrName").value().getString();
				std::string tmp_shapes = "";

				if (obj->isAttr(attrname.c_str())) {
					// Get state name of shape file
					std::string state = obj->getAttr(attrname.c_str()).value().getString();

					// No shape file
					if (state == "nothing") {
						filename = "nothing";
					}
					else {
						// Get the current shape name
						std::string tmp = classname + "::" + attrname + ":" + state;
						filename = db.getS(tmp);
					}
					// Get other shape file which can transit the state
					std::vector<std::string> shapes = db.getShapeFromClass(classname+":");
					int size = shapes.size(); // Remove the current shape file which is already got
					// Add size
					if (filename == "nothing")
						msg += IntToString(size + 1);
					else
						msg += IntToString(size);

					for (int i = 0; i < size; i++) {
						if (filename == shapes[i]) continue;
						else {
						  tmp_shapes += shapes[i] + ",";
						}
					}
				}
				// There is no state transition
				else{
					filename= db.getS(cname);
					msg += "1,"; // the number of shape file
				}
				msg += filename + "," + tmp_shapes;
			}

			sobj->setShape(filename);

			// Position of entity
			std::string sx = DoubleToString(obj->x());
			std::string sy = DoubleToString(obj->y());
			std::string sz = DoubleToString(obj->z());
			msg += sx + sy + sz;

			// Set of entity position (Used by motion judgement)
			Vector3d pos(obj->x(), obj->y(), obj->z());
			sobj->setVPosition(pos);

			// Orientation data of entity
			std::string sqw = DoubleToString(obj->qw());
			std::string sqx = DoubleToString(obj->qx());
			std::string sqy = DoubleToString(obj->qy());
			std::string sqz = DoubleToString(obj->qz());

			// Set of entity orientation (Used by motion judgement)
			Rotation rot(obj->qw(), obj->qx(), obj->qy(), obj->qz());
			sobj->setVRotation(rot);
			msg += sqw + sqx + sqy + sqz;

			// The number of geometry (only for later than v2.1)
			int jSize = sobj->getJointSize();

			if (jSize == 0) {
				SParts *sparts = sobj->getSBody();
				ODEObj odeobj  = sparts->odeobj();
				dGeomID geom   = odeobj.geom();

				// The number of parts is one, because of no joint
				msg += "1,body,";

				// Send the gap from body
				// Get relative position from CoG
				//const dReal *opos = dGeomGetPosition(geom);
				Vector3d opos;
				sparts->getODEPos(opos);
				msg += DoubleToString(opos.x());
				msg += DoubleToString(opos.y());
				msg += DoubleToString(opos.z());

				// Rotation
				dQuaternion qua;
				dGeomGetQuaternion(geom, qua);
				//LOG_MSG(("quaternion (%f, %f, %f, %f)", qua[0], qua[1], qua[2], qua[3]));

				int geomtype = dGeomGetClass(geom);
				msg += IntToString(geomtype);
				//LOG_MSG(("type = %d", geomtype ));

				// sphere
				if (geomtype == 0) {
					double radius = dGeomSphereGetRadius(geom);
					msg += DoubleToString(radius);
				}

				// box
				if (geomtype == 1) {
					dReal size[3];
					dGeomBoxGetLengths(geom, size);
					msg += DoubleToString(size[0]);
					msg += DoubleToString(size[1]);
					msg += DoubleToString(size[2]);
				}

				// cylinder
				if (geomtype == 3) {
					dReal radius, length;
					dGeomCylinderGetParams(geom, &radius, &length);
					msg += DoubleToString(radius);
					msg += DoubleToString(length);
					//LOG_MSG(("cylinder (%f, %f)", radius, length));
				}
			}
			else {
				SSimObj::PartsM allparts = sobj->getAllParts();
				//int psize = allparts.size();
				//msg += IntToString(psize);

				int pNum = 0;
				SSimObj::PartsM::iterator it = allparts.begin();
				while(it != allparts.end()) {
					// Remove dummy
					if (strstr((*it).first.c_str(),"dummy_seg") != NULL || (*it).first == "body") {
						it++;
						continue;
					}
					it++;
					pNum++;
				}
				// The number of parts
				msg += IntToString(pNum);
				
				it = allparts.begin();
				while(it != allparts.end()) {
					// Remove dummy
					if (strstr((*it).first.c_str(),"dummy_seg") != NULL || (*it).first == "body") {
						it++;
						continue;
					}
					msg += (*it).first + ",";
	  
					ODEObj odeobj = (*it).second->odeobj();
					dGeomID geom = odeobj.geom();
 	 
					Vector3d opos;
					(*it).second->getODEPos(opos);
					msg += DoubleToString(opos.x());
					msg += DoubleToString(opos.y());
					msg += DoubleToString(opos.z());
					//LOG_MSG(("pos (%f, %f, %f)", opos.x(), opos.y(), opos.z()));

					int geomtype = dGeomGetClass(geom);
					msg += IntToString(geomtype);
					//LOG_MSG(("type = %d", geomtype ));
	  
					// sphere
					if (geomtype == 0) {
						double radius = dGeomSphereGetRadius(geom);
						msg += DoubleToString(radius);
					}

					// box
					if (geomtype == 1) {
						dReal size[3];
						dGeomBoxGetLengths(geom, size);
						msg += DoubleToString(size[0]);
						msg += DoubleToString(size[1]);
						msg += DoubleToString(size[2]);
						//LOG_MSG(("box (%f, %f, %f)", size[0], size[1], size[2]));
					}

					// cylinder
					if (geomtype == 3) {
						dReal radius, length;
						dGeomCylinderGetParams(geom, &radius, &length);
						msg += DoubleToString(radius);
						msg += DoubleToString(length);
					}
					it++;
					pNum++;
				}// while
				//msg += IntToString(pNum);

			}
			//const dReal *gpos = dGeomGetPosition(geom);
			//LOG_MSG(("pos = (%f, %f, %f)",gpos[0], gpos[1], gpos[2]));

			///////////////////////////////////////////////////////

			// If a controller is attached
			if (obj->isAttached()) {
				// Attach flag
				msg += "1,";
				SimObj *sim = (SimObj*)obj;
				int camID = 1;

				std::string tmp_camera = "";

				// Get camera ID on the entity
				std::vector<int> ids = sim->getCameraIDs();
				int camNum = ids.size(); // the # of cameras

				// For old version, max camera # is 2 (for safety)
				if (camNum == 0 && !dUse) {

					int id = 1;
					// Check the camera size
					while(1) {
						// Get link name
						std::string link = sim->getCameraLinkName(camID);
						// There is no camera
						if (link == "NoCam" || link == "" || link.empty()) {
							break;
						}
						// Camera is used
						else {

							// id
							tmp_camera += IntToString(id);
							// Link name
							tmp_camera += link + ",";
	  
							// Relative position of camera from link local coordinate
							Vector3d pos;
							sim->getCamPos(pos, camID, false);
							std::string spx = DoubleToString(pos.x());
							std::string spy = DoubleToString(pos.y());
							std::string spz = DoubleToString(pos.z());
							tmp_camera += spx + spy + spz;

							Vector3d vec;
							sim->getCamDir(vec, camID, false);
							std::string svx = DoubleToString(vec.x());
							std::string svy = DoubleToString(vec.y());
							std::string svz = DoubleToString(vec.z());
							tmp_camera += svx + svy + svz;

							// FOV(field of view) and aspect ratio of camera
							tmp_camera += DoubleToString(sim->FOV());
							tmp_camera += DoubleToString(sim->aspectRatio());

						} // else{
						camID++;

						// In a case that the body is not robot
						if (strcmp(link.c_str(), "body") == 0) break;
					} // while(1) {
				}

				// Check whether new version camera is used, when the old camera is not used
				else {
					for (int i = 0; i < camNum; i++) {
						int id = ids[i]; //id number

						Vector3d pos;
						Vector3d dir;

						double quw, qux, quy, quz;

						sim->getCamPos(pos,id, false);  // Position
						sim->getCamDir(dir,id, false);  // Orientation

						sim->getCamQuaternion(quw, qux, quy, quz, id);

						std::string link = sim->getCamLink(id); // link name
						double fov = sim->getCamFOV(id);        // Field of view
						double as  = sim->getCamAS(id);         // aspect ratio

						// Add the camera data to SSimObj instance
						Camera cam(link, pos, dir, as, fov);
						sobj->addCamera(id,cam);

						// id number
						tmp_camera += IntToString(id);
						// add link name
						tmp_camera += link + ",";

						// Relative position from camera position link
						std::string spx = DoubleToString(pos.x());
						std::string spy = DoubleToString(pos.y());
						std::string spz = DoubleToString(pos.z());
						tmp_camera += spx + spy + spz;

						// Orientation of camera
						std::string svx = DoubleToString(dir.x());
						std::string svy = DoubleToString(dir.y());
						std::string svz = DoubleToString(dir.z());
						tmp_camera += svx + svy + svz;

						// Rotation of camera
						std::string qw = DoubleToString(quw);
						std::string qx = DoubleToString(qux);
						std::string qy = DoubleToString(quy);
						std::string qz = DoubleToString(quz);
						tmp_camera += qw + qx + qy + qz;

						// Field of view and aspect ratio
						tmp_camera += DoubleToString(fov);
						tmp_camera += DoubleToString(as);

						camID++;
					}
				}
	
				// Add # of cameras
				msg += IntToString(camID - 1) + tmp_camera;
			} // if (isAttached()) {
			else {
				// Flag: not attached
				msg += "0,";
			}
			it++;
		}

		// Add the size of data at the header
		char bsize[6];
		int nbyte = (int)msg.size() + 6;
		sprintf(bsize, "%.6d", nbyte);
		msg = std::string(bsize) + msg;

		// Sending the data
		if (!sendData(sock, msg.c_str(), nbyte)) {
		  return false;
		}
	} // end of if (w && !ver)

	// New version (later than v2.2? TODO: to be confirmed by inamura on 2013-12-24)
	else if (w && ver) {
		LOG_MSG(("new!!"));

		// prepare of message
		std::string msg = "";

		// Add flag whether simulation is already started
		if (w->isRunning()) msg += "1,";
		else                msg += "0,";

		// Get message clients which is now connected
		const std::vector<Connection*> &messages = m_accept.messages();
		int msg_size = messages.size();

		// Add the number of other services
		msg += IntToString(msg_size);

		for (int i = 0; i < msg_size; i++) {
			Source *message = messages[i]->source;

			// Add service name
			std::string name = std::string(message->name());
			msg += name + ",";
		}
		////////////////////////////////////////////
		typedef std::map<std::string, SSimEntity*> ENMap;
		ENMap enmap = w->getAllSSimEntities();

		// Add the number of entities
		msg += IntToString(enmap.size());

		/////////////////////////////////
		// Loop for entities
		/////////////////////////////////

		ENMap::iterator enit;
		enit = enmap.begin();
		while(enit != enmap.end()) {

			SSimEntity *ent = (*enit).second;

			// Add ID number
			msg += IntToString(ent->getID());

			// Add scale
			Vector3d scale = ent->getScale();

			msg += DoubleToString(scale.x());
			msg += DoubleToString(scale.y());
			msg += DoubleToString(scale.z());

			// Add flag whether joints exist in the entity
			if (ent->isRobot()) msg += "1,";
			else                msg += "0,";

			// Add entity name
			msg += (*enit).first + ","; 

			// Shape file
			std::string filename = ent->getShapeFile();

			// Add file size,  TODO is dealing with multiple files
			msg += "1,";

			// Add file name
			msg += filename + ",";

			// Position data of entity
			Vector3d pos;
			ent->getPosition(pos, true);
			std::string sx = DoubleToString(pos.x());
			std::string sy = DoubleToString(pos.y());
			std::string sz = DoubleToString(pos.z());
			msg += sx + sy + sz;

			// Orientation of entity      
			msg += "1.0,0.0,0.0,0.0,";

			// The number of geometry (in ODE)
			int geomNum = ent->getGeomNum();
			msg += IntToString(geomNum);

			// Loop for geometry
			for (int i = 0; i < geomNum; i++) {
				dGeomID tmp_geom = ent->getPartsGeom(i);
				dGeomID geom = dGeomTransformGetGeom(tmp_geom);

				// Add relative position from CoG
				const dReal *pos = dGeomGetPosition(geom);
				msg += DoubleToString(pos[0]);
				msg += DoubleToString(pos[1]);
				msg += DoubleToString(pos[2]);

				dQuaternion qua;
				dGeomGetQuaternion(geom, qua);

				// Refer the type of parts
				int type = dGeomGetClass(geom);

				// Add orientation to the message to be sent
				msg += DoubleToString(qua[0]);
				msg += DoubleToString(qua[1]);
				msg += DoubleToString(qua[2]);
				msg += DoubleToString(qua[3]);

				msg += IntToString(type);
				// Sphere
				if (type == 0) {
					dReal r = dGeomSphereGetRadius(geom);
					msg += DoubleToString(r);
				}
				// box
				else if (type == 1) {
					dReal size[3];
					dGeomBoxGetLengths(geom, size);
					msg += DoubleToString(size[0]);
					msg += DoubleToString(size[1]);
					msg += DoubleToString(size[2]);
				}
				// cylinder
				else if (type == 3) {
					dReal radius, length;
					dGeomCylinderGetParams(geom, &radius, &length);
					msg += DoubleToString(radius);
					msg += DoubleToString(length);
				}
			}
			// Flag whether the entity has camera (temporary)
			msg += "0,";
			enit++;
		}
		// Add the number of bytes of the message
		char bsize[6];
		int nbyte = (int)msg.size() + 6;
		sprintf(bsize, "%.6d", nbyte);
		msg = std::string(bsize) + msg;

		// Sending data
		if (!sendData(sock, msg.c_str(), nbyte)) {
		  return false;
		}
		////////////////////////////////

	}
	return true;
}


//! Sending entity data which have moved
bool WorldSimulator::sendMoveEntities(SOCKET sock, bool update)
{
	SSimWorld *w = m_provider.get();

	bool ver = w->getV21();

	if (w && !ver) {

		typedef std::map<std::string, SimObjBase*> EMap;
		EMap emap = w->objs(); 

		EMap::iterator it = emap.begin();
    
		std::string msg = "";
    
		if (w->isRunning()) msg += "1,";
		else                msg += "0,";

		char tmp[32];
		sprintf(tmp,"%.2f",w->time());
		std::string str = std::string(tmp);
		str += ",";
    
		msg += str;
		int entSize = 0;
		/////////////////////////////////
		/////////////////////////////////
		while(it != emap.end()) {

			SimObjBase *obj = (*it).second;
			SSimObj *sobj = w->getSObj((*it).first.c_str());

			bool dynamics = obj->dynamics();
			//int num = obj->getCameraNum();

			Vector3d vpos;
			sobj->getVPosition(&vpos);

			// mejirusi
			SParts *body = sobj->getSBody();
			const dReal *tpos = body->getPosition();

			Vector3d npos(tpos[0], tpos[1], tpos[2]);
			//obj->getPosition(npos);

			Rotation vrot;
			sobj->getVRotation(&vrot);

			const dReal *trot = body->getQuaternion();
			Rotation nrot(trot[0], trot[1], trot[2], trot[3]);
			//Rotation nrot(obj->qw(), obj->qx(), obj->qy(), obj->qz());

			bool move           = false;
			bool rotation       = false;
			int  inijointRotNum = 0;
			int  jointRotNum    = 0;
			int  chcamNum       = 0;
			bool chShape        = false;

			int jsize = sobj->getJointSize();

			std::string tmp_inijoints = "";

			std::string tmp_joints = "";

			if (jsize > 0) {
				std::map<std::string, Joint*> *jmap;
				jmap = sobj->getJointMap();
	
				std::map<std::string, Joint*>::iterator it = jmap->begin();
				while( it != jmap->end() ) {
					Joint *joint = (*it).second;

					Rotation inirot  = joint->getIniRotation();

					Rotation inivrot = joint->getIniVRotation();

					if (inirot != inivrot) {

						tmp_inijoints += (*it).first + ",";

						std::string qw = DoubleToString(inirot.qw());
						std::string qx = DoubleToString(inirot.qx());
						std::string qy = DoubleToString(inirot.qy());
						std::string qz = DoubleToString(inirot.qz());

						tmp_inijoints += qw + qx + qy + qz;

						if (update)
							joint->setIniVQuaternion(inirot.qw(), inirot.qx(), inirot.qy(), inirot.qz());
						inijointRotNum++;
					}
					Rotation rot;
					if (dynamics) {
						if (joint->type() == Joint::TYPE_HINGE) {
							HingeJoint *hJoint = (HingeJoint*)joint;
							double angle = hJoint->getAngle();
							double x = hJoint->getAxisX();
							double y = hJoint->getAxisY();
							double z = hJoint->getAxisZ();
	      
							rot.setAxisAndAngle(x,y,z,angle);
						}
					}
					else
						// not dynamics mode
						rot  = joint->getRotation();

					Rotation vrot = joint->getVRotation();

					if (rot != vrot) {

						tmp_joints += (*it).first + ",";

						std::string qw = DoubleToString(rot.qw());
						std::string qx = DoubleToString(rot.qx());
						std::string qy = DoubleToString(rot.qy());
						std::string qz = DoubleToString(rot.qz());

						tmp_joints += qw + qx + qy + qz;

						if (update)
							joint->setVQuaternion(rot.qw(), rot.qx(), rot.qy(), rot.qz());
						jointRotNum++;
					}
					it++;
				}
			}

			SimObj *sim = (SimObj*)obj;
			std::vector<int> ids = obj->getCameraIDs();
			int camNum = ids.size();

			std::map<int, Camera> cams = sobj->getCamera();

			std::string tmp_camera = "";

			for (int i = 0; i < camNum; i++) {

				int id = ids[i];
				Vector3d pos;
				Vector3d dir;
				
				sim->getCamPos(pos,id, false);
				sim->getCamDir(dir,id, false);
				double fov = sim->getCamFOV(id);
				double as  = sim->getCamAS(id);
				std::string link= sim->getCamLink(id);
 
				std::map<int, Camera>::iterator it;
				it = cams.find(id);
				Vector3d vpos = (*it).second.pos;
				Vector3d vdir = (*it).second.dir;
				double   vfov = (*it).second.FOV;
				double   vas  = (*it).second.aspectRatio;
				std::string vlink = (*it).second.link;

				if (vpos  != pos ||
				   vdir  != dir ||
				   vfov  != fov ||
				   vas   != as  ||
				   vlink != link) {

					tmp_camera += IntToString(id);
					tmp_camera += link + ",";

					if (vpos != pos) {

						tmp_camera += "1,";

						std::string x = DoubleToString(pos.x());
						std::string y = DoubleToString(pos.y());
						std::string z = DoubleToString(pos.z());
						tmp_camera += x + y + z;
					}
					else {
						tmp_camera += "0,";
					}
	
					if (vdir != dir) {

						tmp_camera += "1,";

						std::string x = DoubleToString(dir.x());
						std::string y = DoubleToString(dir.y());
						std::string z = DoubleToString(dir.z());
						tmp_camera += x + y + z;
					}
					else {
						tmp_camera += "0,";
					}

					if (vfov != fov) {
						tmp_camera += "1,";
						tmp_camera +=  DoubleToString(fov);
					}
					else {
						tmp_camera += "0,";
					}

					if (vas != as) {
						tmp_camera += "1,";
						tmp_camera +=  DoubleToString(as);
					}
					else {
						tmp_camera += "0,"; 
					}

					if (vlink != link) {

						tmp_camera += "1,";
						tmp_camera +=  link;
					}
					else {
						tmp_camera += "0,";
					}

					if (update) {
						Camera cam(link, pos, dir, as, fov);
						sobj->setCamera(id, cam);
					}
					chcamNum++;
				} // if .....||  vlink != link) {
			} // for (int i = 0; i < camNum; i++) {
			
			std::string vshape = sobj->getShape();

			bool existvis = obj->isAttr("visStateAttrName");

			std::string filename = "";

			if (existvis) {
				std::string attrname = obj->getAttr("visStateAttrName").value().getString();

				if (obj->isAttr(attrname.c_str())) {
	
					std::string state = obj->getAttr(attrname.c_str()).value().getString();

					std::string classname = obj->classname();

					X3DDB &db = m_provider.getX3DDB();

					if (state == "nothing") {
						filename = "nothing";
					}
					else {
						std::string tmp = classname + "::" + attrname + ":" + state;
						filename = db.getS(tmp);
					}

					if (filename == vshape) {
						chShape = false;
					}
					else {
						chShape = true;
					}
				}
			} 

			// else
			else {
				chShape = false;
			}

			if (vpos != npos) move     = true;
			if (vrot != nrot) rotation = true;

			if (!move && !rotation && jointRotNum == 0 && inijointRotNum == 0 && chcamNum == 0 && !chShape) {
				it++;
				continue;
			}
			else {
				msg += (*it).first + ","; 

				entSize++;
			}

			if (move) {
				msg += "1,"; 
				/*
				  std::string sx = DoubleToString(obj->x());
				  std::string sy = DoubleToString(obj->y());
				  std::string sz = DoubleToString(obj->z());
				*/
				std::string sx = DoubleToString(npos.x());
				std::string sy = DoubleToString(npos.y());
				std::string sz = DoubleToString(npos.z());
				msg += sx + sy + sz;

				if (update)
					sobj->setVPosition(npos);
			}
			else {
				msg += "0,"; 
			}

			if (rotation) {
				msg += "1,"; 

				/*
				  std::string qw = DoubleToString(obj->qw());
				  std::string qx = DoubleToString(obj->qx());
				  std::string qy = DoubleToString(obj->qy());
				  std::string qz = DoubleToString(obj->qz());
				*/

				std::string qw = DoubleToString(nrot.qw());
				std::string qx = DoubleToString(nrot.qx());
				std::string qy = DoubleToString(nrot.qy());
				std::string qz = DoubleToString(nrot.qz());

				msg += qw + qx + qy + qz;

				if (update)
					sobj->setVRotation(nrot);
			}
			else {
				msg += "0,"; 
			}

			msg += IntToString(inijointRotNum);

			if (inijointRotNum > 0) {
				msg += tmp_inijoints;
			}

			msg += IntToString(jointRotNum);

			if (jointRotNum > 0) {
				msg += tmp_joints;
			}
			///////////////////////////////////////


			msg += IntToString(chcamNum);

			if (chcamNum > 0) {
				msg += tmp_camera;
			}

			if (chShape) {
				msg += "1,";
				msg += filename + ",";
			}
			else {
				msg += "0,";
			}
			it++;
		}
		//char bsize[6];

		msg = IntToString(entSize) + msg;

		// modify 10/29
		int sendSize = msg.size() + sizeof(unsigned int);
		char *sendBuff = new char[sendSize];
		char *tmp_p = sendBuff;

		BINARY_SET_DATA_S_INCR(tmp_p, unsigned int, sendSize);
		memcpy(tmp_p, msg.c_str(), msg.size());

		if (!sendData(sock, sendBuff, sendSize)) {
			LOG_ERR(("failed to send entities data."));
			delete [] sendBuff;
			return false;
		}
		delete [] sendBuff;
	} //   if (w) {

	else if (w && ver) {

		// new2
		////////////////////////////////////////////
		typedef std::map<std::string, SSimEntity*> ENMap;
		ENMap enmap = w->getAllSSimEntities();

		std::string msg = "";

		if (w->isRunning()) msg += "1,";
		else               msg += "0,"; 

		char tmp[32];
		sprintf(tmp,"%.2f",w->time());
		std::string str = std::string(tmp);
		str += ",";

		msg += str;

		int entSize = 0;

		/////////////////////////////////
		/////////////////////////////////

		ENMap::iterator enit;
		enit = enmap.begin();
		while(enit != enmap.end()) {

			SSimEntity *ent = (*enit).second;

			bool move           = false;
			bool rotation       = false;
			int  inijointRotNum = 0;
			int  jointRotNum    = 0;
			int  chcamNum       = 0;
			bool chShape        = false;

			Vector3d pos; 
			dQuaternion qua = {1.0, 0.0, 0.0, 0.0};

			bool isRobot = ent->isRobot();
			SSimRobotEntity *rent;
			if (isRobot) {

				rent = (SSimRobotEntity*)ent;
				//dBodyID body = rent->getRootBody();
				std::vector<SSimJoint*> joints = rent->getAllJoints();

				dBodyID body = joints[0]->robotParts.objParts.body;
				//dGeomID geom = joints[0]->robotParts.objParts.geoms[0];
				/*
				  dBodyID body1 = joints[1]->robotParts.objParts.body;
				  dBodyID body2 = joints[2]->robotParts.objParts.body;

				  const dReal *tpos1 = dBodyGetPosition(body1);
				  const dReal *tpos2 = dBodyGetPosition(body2);
				  LOG_MSG(("tpos1(%f, %f, %f)", tpos1[0], tpos1[1], tpos1[2])); 
				  LOG_MSG(("tpos2(%f, %f, %f)", tpos2[0], tpos2[1], tpos2[2])); 
				*/
				const dReal *tpos = dBodyGetPosition(body);
				//const dReal *gpos = dGeomGetPosition(geom);

				pos.set(tpos[0], tpos[1], tpos[2]);
				move = true;

				const dReal *tqua = dBodyGetQuaternion(body);
				qua[0] = tqua[0];
				qua[1] = tqua[1];
				qua[2] = tqua[2];
				qua[3] = tqua[3];
				
				LOG_MSG(("qua(%f, %f, %f, %f)", qua[0], qua[1], qua[2], qua[3])); 
				rotation = true;
			}
			else {
				if (update)
					// update
					move = ent->getPosition(pos, true);
				else
					move = ent->getPosition(pos);
	
				if (update)
					rotation = ent->getQuaternion(qua, true);
				else
					rotation = ent->getQuaternion(qua);
			}
			//int jsize = sobj->getJointSize();
			int jsize = 0;

			std::string tmp_joints = "";

			if (isRobot) {

				std::vector<SSimJoint*> joints = rent->getAllJoints();
	
				int jointSize = joints.size();
				for (int i = 0; i < jointSize; i++) {
	  
					dJointID joint = joints[i]->joint;
					dReal angle = 0.0;
					//dVector3 axis;

					if (!joints[i]->isRoot) {
						int type = dJointGetType(joint);
						if (type == dJointTypeHinge) {
							angle = -dJointGetHingeAngle(joint);
							Vector3d axis = joints[i]->iniAxis;

							std::string qw = DoubleToString(cos(angle/2));
							std::string qx = DoubleToString(axis.x()*sin(angle/2));
							std::string qy = DoubleToString(axis.y()*sin(angle/2));
							std::string qz = DoubleToString(axis.z()*sin(angle/2));

							tmp_joints += joints[i]->name + ",";
							tmp_joints += qw + qx + qy + qz;
							//dJointGetHingeAxis(joint, axis);
							//LOG_MSG(("(%f, %f, %f)", axis.x(), axis.y(), axis.z()));
							//LOG_MSG(("joint:%s angle = %f",joints[i]->name.c_str(),angle));
							jsize++;
						}
					}
				}
				jointRotNum =     jsize;
				// test
				//jsize = jointSize - 1;
			}
			//int camNum = ids.size();
			//int camNum = 0;

			if (!move && !rotation && jointRotNum == 0 && inijointRotNum == 0 && chcamNum == 0 && !chShape) {
				enit++;
				continue;
			}
			else {
				msg += (*enit).first + ",";

				entSize++;
			}

			if (move) {
				msg += "1,";

				std::string sx = DoubleToString(pos.x());
				std::string sy = DoubleToString(pos.y());
				std::string sz = DoubleToString(pos.z());
				msg += sx + sy + sz;
			}
			else {
				msg += "0,";
			}

			if (rotation) {
				msg += "1,"; 

				std::string qw = DoubleToString(qua[0]);
				std::string qx = DoubleToString(qua[1]);
				std::string qy = DoubleToString(qua[2]);
				std::string qz = DoubleToString(qua[3]);

				msg += qw + qx + qy + qz;

			}
			else {
				msg += "0,";
			}


			//test 
			//inijointRotNum = 1;
			msg += IntToString(inijointRotNum);

			if (inijointRotNum > 0) {
				// Nothing ??? TODO : confirm by inamura on 2013-12-24
			}


			msg += IntToString(jointRotNum);

			if (jointRotNum > 0) {
				msg += tmp_joints;
			}
			///////////////////////////////////////

			msg += IntToString(chcamNum);

			if (chcamNum > 0) {
				// Nothing ??? TODO : confirm by inamura on 2013-12-24
			}

			if (chShape) {
				// Nothing ??? TODO : confirm by inamura on 2013-12-24
			}
			else {
				msg += "0,";
			}
			enit++;
		}//     while(enit != enmap.end()) {

		int nbyte = 0;
//		char bsize[6];

		msg = IntToString(entSize) + msg;

		// modify 10/29
		int sendSize = msg.size() + sizeof(unsigned int);
		char *sendBuff = new char[sendSize];
		char *tmp_p = sendBuff;

		BINARY_SET_DATA_S_INCR(tmp_p, unsigned int, sendSize);
		memcpy(tmp_p, msg.c_str(), msg.size());

		//if (!sendData(sock, msg.c_str(), nbyte)) {
		if (!sendData(sock, sendBuff, sendSize)) {
			LOG_ERR(("failed to send entities data."));
			delete [] sendBuff;
			return false;
		}
		delete [] sendBuff;
	}

	return true;
}


// !Send shape file information which is updated
bool WorldSimulator::sendShapeFile(SOCKET sock, std::string name)
{
	std::string path = getenv("SIGVERSE_DATADIR");
	// Check whether the current directory has the target file
	std::string file = "./" + name;

	FILE *fp;
	char s[1024]; // TODO: Magic number should be removed

	// Message buffer
	std::string msg = "";

	if ((fp = fopen(file.c_str(), "r")) == NULL) {
		// If the current directory does not have the target file, check the directory written in the default SIGVERSE_DATADIR directory.
		file = path + "/shape/" + name;
		if ((fp = fopen(file.c_str(), "r")) == NULL) {
			LOG_ERR(("Could not read shape file. [%s]  [%s, %d]", name.c_str(), __FILE__, __LINE__ ));
			return false;
		}
	}

	// File reading and writing.
	while (fgets(s, 1024, fp) != NULL) { // TODO: Magic number should be removed
		msg += std::string(s);
	}

	// end of file
	char eof = htons(0x1a);

	msg += eof;

	// Add data size on the header
	char bsize[7];
	int nbyte = (int)msg.size() + 7; // TODO: Magic number should be removed
	sprintf(bsize, "%.7d", nbyte);
	std::string tmp = std::string(bsize);
	msg = tmp + msg;

	// Sending data
	if (!sendData(sock, msg.c_str(), nbyte)) {
		return false;
	}

	fclose(fp);
	return true;
}


static char s_buf[50000];  // TODO: magic number should be removed. position is strange by inamura on 2013-12-25


bool WorldSimulator::runStep()
{
	// If the request to simulation start has been come
	if (s_restart) {
		SSimWorld *w = m_provider.create();
		if (!w)
			return false;
		m_l->notifyInit();
		w->start();
		s_restart = false;
	}

	// Refer the instance of ODE simulation
	SSimWorld *w = m_provider.get();

	bool autostep = w->getAutoStep();

	// Progress the time count
	if (autostep) {
		w->nextStep();
	}

	// Reference of SIGVerse time
	const double currTime = w->time();
	//LOG_DEBUG2(("time = %f", currTime));

	// Lock during the access
	m_accept.lock();

	// Get clients list which is now connecting
	const std::vector<Connection*> &clients = m_accept.clients();

	// Get client message from the one which is now connecting
	const std::vector<Connection*> &messages = m_accept.messages();

	m_accept.unlock();
	m_log->flush(clients);

	// If the number of clients which is connected is 0, exit from this function
	if (m_accept.clientNum() <= 0 && messages.size() <= 0) { return true; }

	//const bool running = w->isRunning();
	ODEWorld * odeWorld = w->odeWorld();

	// Message from message service (including SIGViewer)
	////////////////SendMessage Service/////////////////
	for (int i = 0; i < (int)messages.size(); i++) {

		Source *message = messages[i]->source;

		if (strcmp(message->name(), "SIGEND") == 0) {
			w->stop();
			LOG_SYS(("****Quit Simulation****"));
			//exit(1);
			return false;
		}
		// No type or it can be ignored
		if (message->noType() || message->ignore()) {
		  continue; 
		}

		SOCKET s = message->socket();
		char *pbuf = s_buf;
		int r;

		// Skip the first five bytes in the begining of the received data, and read
		r = recv_nonblock(s, pbuf, 5);

		if (r == 5 || r == 0) {

			// Get data size (excluding the first five bytes)
			int size;
			if (r == 5)
				size= atoi(pbuf) - 5;

			// Disconnect the service
			if (size == -1 || r == 0) {
				LOG_SYS(("Service \"%s\" disconnected from SIGVerse server.", message->name()));

				// Notice the disconnection to other services 
				for (int j = 0; j < (int)messages.size(); j++) {
					// Do not send to myself
					if (i == j) continue;

					std::string msg = message->name();
					int dataSize = msg.size() + sizeof(unsigned short) * 2;

					char *sendBuff = new char[dataSize];
					char *p = sendBuff;

					BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0008); //TODO: magic number
					BINARY_SET_DATA_S_INCR(p, unsigned short, dataSize);
					memcpy(p, msg.c_str(), msg.size());

					if (!sendData(messages[j]->source->socket(), sendBuff, dataSize)) {
						LOG_ERR(("cannot inform to to another service."));
					}
					delete [] sendBuff;

				}
				m_accept.pushNoNeeded(message);
				m_accept.eraseNoNeeded();

				continue;
			}
			// Create buffer for the data size
			char *tmpBuff = new char[size];

			// Get message data
			if (!recvData(s, tmpBuff, size)) {
				LOG_ERR(("Failed to recieve Message"));
			}
			// Get message source
			std::string from = std::string(message->name());
			// Sending the data
			sendOnMsg(s, tmpBuff, from);
			delete [] tmpBuff;
		}
	}
	// Loop for every connecting clients
	for (int i = 0; i < (int)clients.size(); i++) {

		// Get client source
		Source *client = clients[i]->source;
		//LOG_MSG(("i = %d, type = %d", i, client->type()));

		// No type or it can be ignored
		if (client->noType() || client->ignore()) {
			continue;
		}
		// Socket for the source
		SOCKET s = client->socket();

		// try to read
		int waitTime = 1000;
		bool forwarding = false;
	again:
		char *pbuf = s_buf;
		unsigned short packetSize = 0;
		int r;

		// Skip the first four bytes in the begining of the received data, and read
		int firstRead = COMM_DATA_PACKET_TOKEN_DATASIZE_BYTES;
		r = recv_nonblock(s, pbuf, firstRead);

		// If the four bytes existed
		if (r == firstRead) {
			char *p = pbuf;

			// Get request type
			int n = BINARY_GET_DATA_S_INCR(p, unsigned short);

			// Sending to SIGViewer (add by Okamoto on 2012-07-02)
			if (0 < n && n < REQUEST_SIZE) {
				switch(n) {
				// Request to get all of the entity data (hwne SIGViewer has just been attached)
				case REQUEST_GET_ALL_ENTITIES_FIRST:
				{
					if (!sendAllEntities(s, pbuf)) {
						LOG_ERR(("sendAllEntities failed."));
					}
					continue;
				}
				// Simulation Start
				case REQUEST_START_SIMULATION:
				{
					startSimulation(w);
					break;
				}
				// Simulation stop
				case REQUEST_STOP_SIMULATION:
				{
					if (w && w->isRunning()) {
						// Notice the 'simulation stop' event to every controller
						std::vector<Source*> srcs = m_accept.getAllCtlClients();
						int size = srcs.size();
						for (int i = 0; i < size; i++) {
							// Buffer for the message sending
							int dataSize = sizeof(unsigned short) * 2;
#ifndef WIN32
							char sendBuff[dataSize];
#else
							char sendBuff[4];
#endif
							char *p = sendBuff;
		
							// Notice the 'simulation stop' event to the target controller
							BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0003); // TODO: Magic number should be removed
							BINARY_SET_DATA_S_INCR(p, unsigned short, dataSize);
							// Send request of stop the simulation to the controller
							if (!sendData(srcs[i]->socket(), sendBuff, dataSize)) {
								LOG_ERR(("cannot send simulation stop message to controller."));
							}
						}
						LOG_SYS(("****SIMULATION STOP****"));
						w->stop();
					}
					continue;
				}
				// Request to get moved entity data
				case REQUEST_GET_MOVE_ENTITIES:
				{
					// Update the data after the last Viewer's request
					int tmp_size = clients.size();
					bool update = false;
					if (i == tmp_size - 1) update = true;
					if (!sendMoveEntities(s,update)) {
						LOG_ERR(("sendMoveEntities failed."));
					}
					packetSize = 0;
					continue;
				}
				// Sending shape file information to the request of download shape file
				case REQUEST_DOWNLOAD_SHAPE:
				{
					// size of the file name of the shape file
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);

					char name[128];  // TODO: Magic number
					memset(name, '\0', sizeof(name));
					if (!recv_nonblock(s, name, size)) {
						LOG_ERR(("Could not recieve shape file name."));
						continue;
					}
					std::string sname = std::string(name);

					// Sending file name data
					if (!sendShapeFile(s, name)) {
						LOG_ERR(("Could not send shape file."));
						continue;
					}

					packetSize = 0;
					continue;
				}
				// Dealing with disconnect request
				case REQUEST_DISCONNECT:
				{
					Source *client = clients[i]->source;
					if (strcmp(client->name(), "SIGSTART") != 0)
						LOG_SYS(("Disconnected %s", client->name()));
					m_accept.pushNoNeeded(client);
					continue;
				}
				// Dealing with quit request
				case REQUEST_QUIT:
				{
					w->stop();
					LOG_SYS(("****Quit Simulation****"));
#ifndef WIN32
					close(s);
#else
					closesocket(s);
					//WSACleanup();
#endif
					//exit(1);
					return false;
				}
				// Dealing with the sent message from controllers
				case REQUEST_SENDMSG_FROM_CONTROLLER:
				{
					// Get the size of data (excluding the header size)
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;  // This is the size of header, TODO: magic number should be removed

					char *recvBuff = new char[size];
					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve message"));
						delete [] recvBuff;
						continue;
					}

					std::string from = client->name();

					// Forwarding the message
					if (!sendOnMsg(s, recvBuff, from))
						LOG_ERR(("Failed to send message."));

					delete [] recvBuff;
					//continue;
					goto again;
				}

				case REQUEST_CONNECT_SERVICE:
				{
					// Get the size of data (excluding the header size)
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4; // TODO

					char *recvBuff = new char[size];
					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to connect service [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}

					char *pp = recvBuff;
					// Get the size of data (excluding the header size)
					unsigned short port = BINARY_GET_DATA_S_INCR(pp, unsigned short);

					std::string service = strtok(pp, ",");
					std::string name = strtok(NULL, ",");
					std::string host = client->hostname();

					// Check whether the controller is executed in the localhost
					if (host == "127.0.0.1" || host == "0.0.0.0") {

						// Get the target service
						Source *src = m_accept.get(service.c_str(), SOURCE_TYPE_MESSAGE);
						char result[4];     // Used in sending the result to controller
						char *ppp = result;

						if (!src) {
							LOG_ERR(("Could not find service provider [%s] [%s, %d]", service.c_str(), __FILE__, __LINE__));
							// Notice the controller that the service is not found
							BINARY_SET_DATA_S_INCR(ppp, unsigned short, 0x0000);
							BINARY_SET_DATA_S_INCR(ppp, unsigned short, 4);      
							continue;
						}
						else {
							// Notice the controller that the service exist
							BINARY_SET_DATA_S_INCR(ppp, unsigned short, 0x0001);
							BINARY_SET_DATA_S_INCR(ppp, unsigned short, 4);      
							//sendData(s, result, 4);
						}

						std::string sendMsg = name + ",";

						// Create buffer for the sending
						int sendSize =  sendMsg.size() + sizeof(unsigned short) * 3;
						char *sendBuff = new char[sendSize];
						char *p = sendBuff;

						// Add header and data size
						BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0003);      
						BINARY_SET_DATA_S_INCR(p, unsigned short, sendSize);      
						BINARY_SET_DATA_S_INCR(p, unsigned short, port);      

						memcpy(p, sendMsg.c_str(), sendMsg.size());

						// Send connection request to the target service
						if (!sendData(src->socket(), sendBuff, sendSize)) {
						  LOG_ERR(("Failed to send connect request to [%s] [%s, %d]", service.c_str(),  __FILE__, __LINE__));
						}
					}

					// [ToDo]
					else {
					}

					delete [] recvBuff;
					//goto again;
					break;
				}

				// Dealing with a request to get joint position
				case REQUEST_GET_JOINT_POSITION:
				{
					if (!client->isControllerData()) {
						LOG_MSG(("no controllerData"));
						break;
					}

					// Get the size of data (excluding the header size)
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];
					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						goto again;
					}
					char *pp     = recvBuff;
					char *name   = strtok(pp, ",");
					char *jname  = strtok(NULL, ",");
					SSimObj *my  = w->getSObj(name);
					Joint *joint = my->getJoint(jname);

					bool success = false;
					if (joint == NULL) {
						LOG_ERR(("getJointPosition: cannot find joint [%s]", joint));
					}
					else success = true;
					
					delete [] recvBuff;

					// Refer the position of the joint
					Vector3d pos(0.0, 0.0, 0.0);
					if (success)
						pos = joint->getCurrentAnchorPosition();

					// Buffer for position of joints
					int sendSize = sizeof(double) * 3 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					// Add position data in the buffer
					BINARY_SET_DOUBLE_INCR(sp, pos.x());
					BINARY_SET_DOUBLE_INCR(sp, pos.y());
					BINARY_SET_DOUBLE_INCR(sp, pos.z());

					// Flag for the reference success
					BINARY_SET_BOOL_INCR(sp, success);

					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] sendBuff;
					goto again;
				}

				case REQUEST_SET_JOINT_QUATERNION:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;
					
					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					// Reference of the quaternion
					double qw = BINARY_GET_DOUBLE_INCR(pp);
					double qx = BINARY_GET_DOUBLE_INCR(pp);
					double qy = BINARY_GET_DOUBLE_INCR(pp);
					double qz = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					char *jName     = strtok(NULL, ",");
					bool offset     = (atoi(strtok(NULL, ",")) != false);

					SSimObj *obj = w->getSObj(agentName);
					if (!obj) {
						LOG_ERR(("SetJointQuaternion : no agent \"%s\"", agentName));
						delete [] recvBuff;
						goto again;
					}
					
					//const char *jName = evt.getJointName(); //Get the joint name specified by the event.
					Joint *j = obj->getJoint(jName);		  //Get the joint from the joint name.
					if (!j) {
						LOG_ERR(("SetJointQuaternion : %s does NOT have joint \"%s\"", agentName, jName));
						delete [] recvBuff;
						goto again;
					}
					// Cannot execute in dynamic on mode
					if (obj->dynamics()) {
						LOG_ERR(("Cannot Set Joint Quaternion. dynamics is true")) ;
						delete [] recvBuff;
						goto again;
					}

					//set OffsetQuaternion
					//bool offset = evt.getoffset();
					if (offset) {
						Vector3d ini;
						j->setOffsetQuaternion(qw, qx, qy, qz,ini);
					}

					//set Quaternion
					else
						j->setQuaternion(qw, qx, qy, qz);

					delete [] recvBuff;
					goto again;
				} // 	case REQUEST_SET_JOINT_QUATERNION: {
	
				// Dealing with a request to get vector of pointing gesture
				case REQUEST_GET_POINTING_VECTOR:
				{
					//if (!client->isControllerData())   break;
					if (!client->isControllerData()) {
						break;
					}

					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}

					char *p = recvBuff;

					char *agentName = strtok(p, ",");
					char *jName1    = strtok(NULL, ",");
					char *jName2    = strtok(NULL, ",");

					SSimObj *obj = w->getSObj(agentName);
					bool success = true;
					if (!obj) {
						LOG_ERR(("getPointingVector : no agent \"%s\"", agentName));
						success = false;
					}

					//Get the joint1 from the joint name.
					Joint *j1 = obj->getJoint(jName1);  
					Joint *j2 = obj->getJoint(jName2);  
					if (!j1) {
						LOG_ERR(("getPointingVector : %s does NOT have joint \"%s\"", agentName, jName1));
						success = false;
					}
					else if (!j2) {
						LOG_ERR(("getPointingVector : %s does NOT have joint \"%s\"", agentName, jName2));
						success = false;
					}

					Vector3d pos1(0.0, 0.0, 0.0);
					Vector3d pos2(0.0, 0.0, 0.0);

					// Refer the position of joints
					if (success) {
						pos1 = j1->getCurrentAnchorPosition();
						pos2 = j2->getCurrentAnchorPosition();
					}
					// Vector from the start position to the destination
					pos2 -= pos1;

					// Normalization
					double tmp = sqrt(pos2.x()*pos2.x() + pos2.y()*pos2.y() + pos2.z()*pos2.z());
					pos2.set(pos2.x()/tmp, pos2.y()/tmp, pos2.z()/tmp);

					// Create buffer
					int sendSize = sizeof(double) * 3 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					// Add the pointing vector to the buffer
					BINARY_SET_DOUBLE_INCR(sp, pos2.x());
					BINARY_SET_DOUBLE_INCR(sp, pos2.y());
					BINARY_SET_DOUBLE_INCR(sp, pos2.z());

					// Flag of the result
					BINARY_SET_BOOL_INCR(sp, success);

					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] recvBuff;
					delete [] sendBuff;
					goto again;
				} //    end of case REQUEST_GET_POINTING_VECTOR:

				case REQUEST_GET_PARTS_POSITION:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						goto again;
					}

					char *pp = recvBuff;
					char *name = strtok(pp, ",");
					char *pname = strtok(NULL, ",");

					SSimObj *my = w->getSObj(name);
					SParts *parts = my->getSParts(pname);

					bool success = false;
					if (parts == NULL) {
						LOG_ERR(("getPartsPosition: cannot find joint [%s]", pname));
					}
					else success = true;

					// Reference of the parts position
					const dReal *pos;
					if (success)
						pos = parts->getPosition();
	
					// Creation of buffer
					int sendSize = sizeof(double) * 3 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					// Add position data to the buffer
					BINARY_SET_DOUBLE_INCR(sp, pos[0]);
					BINARY_SET_DOUBLE_INCR(sp, pos[1]);
					BINARY_SET_DOUBLE_INCR(sp, pos[2]);

					// Flag of the result
					BINARY_SET_BOOL_INCR(sp, success);

					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}

					delete [] recvBuff;
					delete [] sendBuff;
					goto again;
				} // end of case REQUEST_GET_PARTS_POSITION:

				case REQUEST_GET_PARTS_QUATERNION: //added by Guezout (2015/1/28)
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];
					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						goto again;
					}

					char *pp = recvBuff;
					char *name = strtok(pp, ",");
					char *pname = strtok(NULL, ",");

					SSimObj *my = w->getSObj(name);
					SParts *parts = my->getSParts(pname);

					bool success = false;
					if (parts == NULL) {
						LOG_ERR(("getPartsQuaternion: cannot find joint [%s]", pname));
					}
					else success = true;

					// Reference of the parts position
					const dReal *qt;
					if (success)
						qt = parts->getQuaternion();
	
					// Creation of buffer
					int sendSize = sizeof(double) * 4 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					// Add position data to the buffer
					BINARY_SET_DOUBLE_INCR(sp, qt[0]);
					BINARY_SET_DOUBLE_INCR(sp, qt[1]);
					BINARY_SET_DOUBLE_INCR(sp, qt[2]);
					BINARY_SET_DOUBLE_INCR(sp, qt[3]);

					// Flag of the result
					BINARY_SET_BOOL_INCR(sp, success);

					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}

					delete [] recvBuff;
					delete [] sendBuff;
					goto again;
				} // end of case REQUEST_GET_PARTS_QUATERNION:

				case REQUEST_SET_ENTITY_POSITION: 
				{
					// Check wheter the request is sent from controller
					if (!client->isControllerData()) {
						break;
					}
					
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}

					char *pp = recvBuff;

					// Reference of the position
					double x = BINARY_GET_DOUBLE_INCR(pp);
					double y = BINARY_GET_DOUBLE_INCR(pp);
					double z = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					//SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("setPosition : no agent \"%s\"", agentName));
						delete [] recvBuff;
						goto again;
					}
					
					SParts *parts = obj->getSBody();
					ODEObj oobj   = parts->odeobj();
					dBodyID body  = oobj.body();

					// Set the position of the entity in ODE world
					dBodySetPosition(body, x, y, z);

					delete [] recvBuff;
					//return true;

					goto again;
				} // end of case REQUEST_SET_JOINT_QUATERNION:

				case REQUEST_CHECK_SERVICE:
				{
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to check service [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}

					char *pp = recvBuff;
					std::string service = strtok(pp, ",");

					// Get the target service
					Source *src = m_accept.get(service.c_str(), SOURCE_TYPE_MESSAGE);

					char result[4]; // Used when sending the result
					pp = result;

					// If the target service is not found
					if (!src) {
						BINARY_SET_DATA_S_INCR(pp, unsigned short, 0x0000);
					}
					// If the target service is found
					else {
						BINARY_SET_DATA_S_INCR(pp, unsigned short, 0x0001);
					}
					BINARY_SET_DATA_S_INCR(pp, unsigned short, 4);
					//sendData(s, result, 4);
					send(s, result, 4, 0);
					delete [] recvBuff;
					goto again;
				}// end of case REQUEST_CHECK_SERVICE:

				case REQUEST_GET_ENTITY_POSITION:
				{
					// Check whether the request is sent from controller
					if (!client->isControllerData()) {
						break;
					}

					bool success = true;
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					//SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("getPosition : no agent \"%s\"", agentName));
						delete [] recvBuff;
						success = false;
					}

					const dReal *pos;
					if (success) {
						SParts *parts = obj->getSBody();
						ODEObj oobj = parts->odeobj();
						dBodyID body = oobj.body();

						// Reference of position data to ODE world
						pos = dBodyGetPosition(body);
					}
					// Buffer for sending the data
					int sendSize = sizeof(double) * 3 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					// Flag of the result
					BINARY_SET_BOOL_INCR(sp, success);

					// Add position data to the buffer
					BINARY_SET_DOUBLE_INCR(sp, pos[0]);
					BINARY_SET_DOUBLE_INCR(sp, pos[1]);
					BINARY_SET_DOUBLE_INCR(sp, pos[2]);

					// Sending result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("getPosition: Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] recvBuff;
					delete [] sendBuff;

					//return true;
					goto again;
				}

				case REQUEST_SET_ENTITY_ROTATION:
				{
					// Check whether the request is sent from controller	  
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;
					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					unsigned short abs = BINARY_GET_DATA_S_INCR(pp, unsigned short);

					// Get orientation (quaternion) data from the buffer
					double qw = BINARY_GET_DOUBLE_INCR(pp);
					double qx = BINARY_GET_DOUBLE_INCR(pp);
					double qy = BINARY_GET_DOUBLE_INCR(pp);
					double qz = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("SetRotation : no agent \"%s\"", agentName));
						delete [] recvBuff;
						goto again;
					}

					SParts *parts = obj->getSBody();
					ODEObj oobj = parts->odeobj();
					dBodyID body = oobj.body();

					dReal qua[4];
					// Abusolute orientation
					if (abs) {
						qua[0] = qw;
						qua[1] = qx;
						qua[2] = qy;
						qua[3] = qz;
					}
					// Relative orientation
					else {
						const dReal tmp_qua[] = {qw, qx, qy, qz};
						// Multiplication of quaternion
						const dReal *now_qua = dBodyGetQuaternion(body);
						dQMultiply0(qua, now_qua, tmp_qua);
					}
					// Set the new orientation to the SimObj instance (for the confirmation)
					sobj->qw(qua[0]);
					sobj->qx(qua[1]);
					sobj->qy(qua[2]);
					sobj->qz(qua[3]);
					// Set the new orientation to the ODE world
					dBodySetQuaternion(body, qua);

					delete [] recvBuff;
					//return true;
					goto again;
				}

				case REQUEST_GET_ENTITY_ROTATION:
				{
					// Check whether the request is sent from controller	  
					if (!client->isControllerData()) {
						break;
					}

					bool success = true;
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					//SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("getRotation : no agent \"%s\"", agentName));
						delete [] recvBuff;
						success = false;
					}

					const dReal *qua;
					if (success) {
						SParts *parts = obj->getSBody();
						ODEObj oobj = parts->odeobj();
						dBodyID body = oobj.body();
						// Reference of orientation data to ODE world
						qua = dBodyGetQuaternion(body);
					}
					// Buffer for the result sending
					int sendSize = sizeof(double) * 4 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					// Flag of the result
					BINARY_SET_BOOL_INCR(sp, success);
					// Add orientation data to the buffer
					BINARY_SET_DOUBLE_INCR(sp, qua[0]);
					BINARY_SET_DOUBLE_INCR(sp, qua[1]);
					BINARY_SET_DOUBLE_INCR(sp, qua[2]);
					BINARY_SET_DOUBLE_INCR(sp, qua[3]);

					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("getRotation: Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] recvBuff;
					delete [] sendBuff;
					//return true;
					goto again;
				} // end of case REQUEST_GET_ENTITY_ROTATION:

				// Check whether the entity exist?
				case REQUEST_CHECK_ENTITY:
				{
					unsigned short success = 1;
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}

					char *pp = recvBuff;

					char *agentName = strtok(pp, ",");

					SSimObj *obj = w->getSObj(agentName);

					//SimObj *sobj = (SimObj*)obj; 

					if (!obj) {
						LOG_ERR(("getObj : no agent \"%s\"", agentName));
						delete [] recvBuff;
						success = 0;
					}

					// Buffer for sending
					int sendSize = sizeof(unsigned short);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;	  

					BINARY_SET_DATA_S_INCR(sp, unsigned short, success);

					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("getObj: Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] recvBuff;
					delete [] sendBuff;

					//return true;
					goto again;
				}

				case REQUEST_SET_CAMERA_POSITION:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					// Get Camera ID from the received message
					unsigned short camID = BINARY_GET_DATA_S_INCR(pp, unsigned short);
					// Get position data from the received message
					double x = BINARY_GET_DOUBLE_INCR(pp);
					double y = BINARY_GET_DOUBLE_INCR(pp);
					double z = BINARY_GET_DOUBLE_INCR(pp);

					// Get agent name from the received message
					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("setCamPos : no agent \"%s\"", agentName));
						delete [] recvBuff;
						goto again;
					}

					char tmpx[6];
					char tmpy[6];
					char tmpz[6];

					sprintf(tmpx,"epx%d",camID);
					sprintf(tmpy,"epy%d",camID);
					sprintf(tmpz,"epz%d",camID);

					if (!sobj->isAttr(tmpx) || !sobj->isAttr(tmpy) || !sobj->isAttr(tmpz)) {
						LOG_ERR(("setCamPos: Cannot find camera id [%d]", camID));
						return false;
					}
					else {
						sobj->getAttr(tmpx).value().setDouble(x);
						sobj->getAttr(tmpy).value().setDouble(y);
						sobj->getAttr(tmpz).value().setDouble(z);
					}

					delete [] recvBuff;
					//return true;
					goto again;
				}

				case REQUEST_GET_CAMERA_POSITION:
				{
					if (!client->isControllerData()) {
						break;
					}

					bool success = true;
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					unsigned short camID = BINARY_GET_DATA_S_INCR(pp, unsigned short);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("getCamPos : no agent \"%s\"", agentName));
						delete [] recvBuff;
						success = false;
					}
					delete [] recvBuff;

					char tmpx[6];
					char tmpy[6];
					char tmpz[6];

					sprintf(tmpx,"epx%d",camID);
					sprintf(tmpy,"epy%d",camID);
					sprintf(tmpz,"epz%d",camID);

					double x=0.0;
					double y=0.0;
					double z=0.0;

					if (!sobj->isAttr(tmpx) || !sobj->isAttr(tmpy) || !sobj->isAttr(tmpz)) {
						LOG_ERR(("getCamPos: Cannot find camera id [%d]", camID));
						success = false;
					}
					else {
						x = sobj->getAttr(tmpx).value().getDouble();
						y = sobj->getAttr(tmpy).value().getDouble();
						z = sobj->getAttr(tmpz).value().getDouble();
					}

					int sendSize = sizeof(double) * 3 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					BINARY_SET_BOOL_INCR(sp, success);

					BINARY_SET_DOUBLE_INCR(sp, x);
					BINARY_SET_DOUBLE_INCR(sp, y);
					BINARY_SET_DOUBLE_INCR(sp, z);

					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("getCamPos: Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] sendBuff;

					//return true;
					goto again;
				}

				case REQUEST_SET_CAMERA_DIRECTION:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					unsigned short camID = BINARY_GET_DATA_S_INCR(pp, unsigned short);

					double x = BINARY_GET_DOUBLE_INCR(pp);
					double y = BINARY_GET_DOUBLE_INCR(pp);
					double z = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("setCamDir : no agent \"%s\"", agentName));
						delete [] recvBuff;
						goto again;
					}

					char tmpx[6];
					char tmpy[6];
					char tmpz[6];

					sprintf(tmpx,"evx%d",camID);
					sprintf(tmpy,"evy%d",camID);
					sprintf(tmpz,"evz%d",camID);

					if (!sobj->isAttr(tmpx) || !sobj->isAttr(tmpy) || !sobj->isAttr(tmpz)) {
						LOG_ERR(("setCamDir: Cannot find camera id [%d]", camID));
						return false;
					}
					else {
						sobj->getAttr(tmpx).value().setDouble(x);
						sobj->getAttr(tmpy).value().setDouble(y);
						sobj->getAttr(tmpz).value().setDouble(z);
					}

					delete [] recvBuff;
					//return true;
					goto again;
				}

				case REQUEST_GET_CAMERA_DIRECTION:
				{
					if (!client->isControllerData()) {
						break;
					}

					bool success = true;
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					unsigned short camID = BINARY_GET_DATA_S_INCR(pp, unsigned short);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("getCamDir : no agent \"%s\"", agentName));
						delete [] recvBuff;
						success = false;
					}
					delete [] recvBuff;

					char tmpx[6];
					char tmpy[6];
					char tmpz[6];

					sprintf(tmpx,"evx%d",camID);
					sprintf(tmpy,"evy%d",camID);
					sprintf(tmpz,"evz%d",camID);

					double x=0.0;
					double y=0.0;
					double z=0.0;

					if (!sobj->isAttr(tmpx) || !sobj->isAttr(tmpy) || !sobj->isAttr(tmpz)) {
						LOG_ERR(("getCamPos: Cannot find camera id [%d]", camID));
						success = false;
					}
					else {
						x = sobj->getAttr(tmpx).value().getDouble();
						y = sobj->getAttr(tmpy).value().getDouble();
						z = sobj->getAttr(tmpz).value().getDouble();
					}

					int sendSize = sizeof(double) * 3 + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					BINARY_SET_BOOL_INCR(sp, success);

					BINARY_SET_DOUBLE_INCR(sp, x);
					BINARY_SET_DOUBLE_INCR(sp, y);
					BINARY_SET_DOUBLE_INCR(sp, z);

					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("getCamDir: Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] sendBuff;

					//return true;
					goto again;
				}

				case REQUEST_SET_CAMERA_FOV:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					unsigned short camID = BINARY_GET_DATA_S_INCR(pp, unsigned short);

					double fov = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("setCamFOV : no agent \"%s\"", agentName));
						delete [] recvBuff;
						goto again;
					}

					char tmp[6];

					sprintf(tmp,"FOV%d",camID);
					
					if (!sobj->isAttr(tmp)) {
						LOG_ERR(("setCamFOV: Cannot find camera id [%d]", camID));
						return false;
					}
					else {
						sobj->getAttr(tmp).value().setDouble(fov);
					}

					delete [] recvBuff;
					goto again;
				}

				case REQUEST_SET_CAMERA_ASPECTRATIO:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					unsigned short camID = BINARY_GET_DATA_S_INCR(pp, unsigned short);

					double ar = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SimObj *sobj = (SimObj*)obj; 
					if (!obj) {
						LOG_ERR(("setCamAS : no agent \"%s\"", agentName));
						delete [] recvBuff;
						goto again;
					}

					char tmp[14];

					sprintf(tmp,"aspectRatio%d",camID);

					if (!sobj->isAttr(tmp)) {
						LOG_ERR(("setCamAS: Cannot find camera id [%d]", camID));
						return false;
					}
					else {
						sobj->getAttr(tmp).value().setDouble(ar);
					}

					delete [] recvBuff;
					goto again;
				}

				case REQUEST_SET_WHEEL:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					double wheelRadius   = BINARY_GET_DOUBLE_INCR(pp);
					double wheelDistance = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SRobotObj *robj = dynamic_cast<SRobotObj*>(obj); 
					if (robj == NULL) {
						LOG_ERR(("setWheel: %s is not ROBOT object",agentName));
						continue;
					}
					else {
						robj->setWheel(wheelRadius, wheelDistance);
					}
					delete [] recvBuff;
					goto again;
					//LOG_MSG(("%s (%f, %f)", agentName, wheelRadius, wheelDistance));
				}

				case REQUEST_SET_WHEEL_VELOCITY:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}

					char *pp = recvBuff;

					double left  = BINARY_GET_DOUBLE_INCR(pp);
					double right = BINARY_GET_DOUBLE_INCR(pp);

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					SRobotObj *robj = dynamic_cast<SRobotObj*>(obj); 
					if (robj == NULL) {
						LOG_ERR(("setWheelVelocity: %s is not ROBOT object",agentName));
						continue;
					}

					else if (robj->getWheelRadius() == 0.0 || robj->getWheelDistance() == 0.0) {
						LOG_ERR(("setWheelVelocity: %s need to set wheel property",agentName));
						continue;
					}
					else {
						robj->setWheelVelocity(left, right);
					}
					//LOG_MSG(("setWheelVeclity!"));
					delete [] recvBuff;

					goto again;
				}

				case REQUEST_GET_JOINT_ANGLE:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					char *agentName = strtok(pp, ",");
					char *jointName = strtok(NULL, ",");

					SSimObj *obj = w->getSObj(agentName);
					Joint *j = obj->getJoint(jointName);

					bool success = true;
					double angle = 0.0;
					if (!j) {
						LOG_MSG(("getJointAngle : %s does NOT have joint \"%s\"",
						         agentName, jointName));
						success = false;
					}
					else if (j->type() == Joint::TYPE_HINGE) {

						HingeJoint* hj = (HingeJoint*)j;
						angle = hj->getAngle();
					}
					else {
						LOG_MSG(("getJointAngle : %s is NOT hinge joint", jointName));
						success = false;
					}

					int sendSize = sizeof(double) + sizeof(bool);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					BINARY_SET_BOOL_INCR(sp, success);
					BINARY_SET_DOUBLE_INCR(sp, angle);

					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] recvBuff;
					delete [] sendBuff;
					goto again;
				}

				// Current status, the max has two meaning; max torque and max angular velocity
				// Now, the meaning of max angular velocity is comment out, but it is dangerous
				// Be careful is the max angular velocity will be used. by inamura on 2013-12-30
				case REQUEST_SET_JOINT_VELOCITY:
				{
					if (!client->isControllerData()) {
						break;
					}
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];
					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;
					// Get angular velocity and max torque from the received message
					double velocity  = BINARY_GET_DOUBLE_INCR(pp);
					double max       = BINARY_GET_DOUBLE_INCR(pp);
					char *agentName  = strtok(pp, ",");   // Name of the target agent
					char *jointName  = strtok(NULL, ","); 

					SSimObj *obj     = w->getSObj(agentName);
					Joint *j         = obj->getJoint(jointName); // Name of the target joint
					double angle     = 0.0;

					HingeJoint *hj;
					if (!j) {
						LOG_MSG(("setJointVelocity : %s does NOT have joint \"%s\"",
						         agentName, jointName));
						goto again;
					}
					else if (j->type() == Joint::TYPE_HINGE) {
						hj = (HingeJoint*)j;
						angle = hj->getAngle();
						// TODO: this angle seems not to be used...
					}
					else {
						LOG_MSG(("setJointVelocity : %s is NOT hinge joint", jointName));
						delete [] recvBuff;
						goto again;
					}

					if (!obj->dynamics()) {
						// when dynamics mode is off
						// modified by inamura: changed from addJointVelocity
						obj->setAngularVelocityToJoint(jointName, velocity);
					}
					else {
						// when dynamics mode is on
						// modified by inamura: changed from setVelocity
						hj->setAngularVelocityMaxTorque(velocity,max);
					}
					delete [] recvBuff;
					goto again;
				}

				case REQUEST_GRASP_OBJECT:
				{
					if (!client->isControllerData()) {
						break;
					}

					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;
					// Get name of agent and parts from the received message
					char *agentName = strtok(pp, ",");
					char *partsName = strtok(NULL, ",");
					char *target    = strtok(NULL, ",");

					// 0: Success
					// 1: Target is not found
					// 2: Grasping now
					// 3: It is alread grasped by other
					// 4: It is not graspable object
					// 5: The target is too far, grasping fail
					unsigned short result = 0;
					SSimObj *obj  = w->getSObj(agentName); 
					SSimObj *tobj = w->getSObj(target);

					SimObjBase *tobj_b = (SimObjBase*)tobj;
					bool graspable = tobj_b->graspable();

					if (!tobj) {
						// Target is not found
						result = 1;
						//LOG_ERR(("graspObj: cannot find object %s [%s, %d]",target , __FILE__, __LINE__));
					}
					else {
						SParts *myParts     = obj    ->getSParts(partsName);// Get the target of grasping
						SParts *targetParts = tobj   ->getSBody();          // Get the target parts
						bool state          = myParts->getOnGrasp();
						bool isgrasped      = tobj   ->getIsGrasped();
						double radius       = tobj_b ->graspRadius();

						if (state) {
							// It's grasping now
							std::string gname = myParts->getGraspingObject();
							//LOG_ERR(("graspObj:%s's %s is already grasping %s",agentName, partsName, gname.c_str()));
							result = 2;
						}
						else if (isgrasped) {
							// It is alread grasped by other
							result = 3;
						}
						else if (!graspable) {
							// It is not graspable object
							result = 4;
						}
						else {
							dBodyID body1 = myParts    ->odeobj().body();
							dBodyID body2 = targetParts->odeobj().body();

							const dReal *p1 = dBodyGetPosition(body1);
							const dReal *p2 = dBodyGetPosition(body2);
							
							// Claculate distance to the target object
							double tmp1 = p1[0] - p2[0];
							double tmp2 = p1[1] - p2[1];
							double tmp3 = p1[2] - p2[2];
							double distance = sqrt(tmp1*tmp1 + tmp2*tmp2 + tmp3*tmp3);

							// If the target is further than threshold, grasping failed
							if (distance > radius) {
								result = 5;
							}
							else {
								// Execution of grasping
								dWorldID world = m_provider.get()->odeWorld()->world();	      
								// Connect with fix joint
								dJointID joint = dJointCreateFixed(world, 0);
								dJointAttach(joint, body1, body2);
		
								// Start of grasping
								myParts->graspObj(target);

								// Set the flag of grasping object to true
								tobj->setIsGrasped(true);
							}
						}
					}
					// Buffer for sending
					int sendSize = sizeof(unsigned short);
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					BINARY_SET_DATA_S_INCR(sp, unsigned short, result);
					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] recvBuff;
					delete [] sendBuff;

					goto again;
				}

				case REQUEST_RELEASE_OBJECT:
				{
					if (!client->isControllerData()) {
						break;
					}

					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					char *agentName = strtok(pp, ",");
					char *partsName = strtok(NULL, ",");

					SSimObj *obj = w->getSObj(agentName); 
					SParts  *myParts = obj->getSParts(partsName);  // Get part of the target

					// set the grasping flag to off
					std::string target = myParts->getGraspingObject();
					myParts->releaseObj();
					if (!target.empty()) {
						SSimObj *tobj = w->getSObj(target.c_str());
						tobj->setIsGrasped(false);
					}

					delete [] recvBuff;
					break;
				}

				case REQUEST_GET_ALL_JOINT_ANGLES:
				{
					if (!client->isControllerData()) {
						break;
					}

					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					char *agentName = strtok(pp, ",");
					SSimObj *obj = w->getSObj(agentName);
					std::map<std::string, Joint*> *joints = obj->getJointMap();
					std::map<std::string, Joint*>::iterator it = joints->begin();

					std::string tmpMsg = "";
					// The number of joints
					int jointNum = 0;

					while(it != joints->end()) {
						if ((*it).second->type() == 2) {
							// Get joint angles
							HingeJoint* hj = (HingeJoint*)((*it).second);
							double angle = hj->getAngle();

							// Add the value to the string
							tmpMsg += std::string((*it).second->name()) + ",";
							tmpMsg += DoubleToString(angle);
							jointNum++;
						}
						it++;
					}

					int sendSize = sizeof(unsigned short)*2 + tmpMsg.size();
					char *sendBuff = new char[sendSize];
					char *sp = sendBuff;

					// Data size
					BINARY_SET_DATA_S_INCR(sp, unsigned short, sendSize);      

					// The number of joints
					BINARY_SET_DATA_S_INCR(sp, unsigned short, jointNum);
					memcpy(sp, tmpMsg.c_str(), tmpMsg.size());

					// Sending the result to controller
					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					delete [] recvBuff;
					delete [] sendBuff;

					//LOG_MSG(("msg = %s", tmpMsg.c_str()));
					break;
				}

				case REQUEST_WORLD_STEP:
				{
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					double stepsize = BINARY_GET_DOUBLE_INCR(pp);
					delete [] recvBuff;

					if (w->getAutoStep()) {
						LOG_ERR(("worldStep: cannot step world. autostep mode ON"));
					}
					else {
						// Progress the simulation step
						w->nextStep(stepsize);
					}
					
					break;
				}

				case REQUEST_WORLD_QUICK_STEP:
				{
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;

					double stepsize = BINARY_GET_DOUBLE_INCR(pp);
					delete [] recvBuff;

					if (w->getAutoStep()) {
						LOG_ERR(("worldQuickStep: cannot step world. autostep mode ON"));
					}
					else {
						// Progress the simulation in quick mode (in ODE)
						w->nextStep(stepsize,true);
					}
					break;
				}

				case REQUEST_GET_ISGRASPED:
				{
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;
					std::string name = strtok(pp, ",");
					delete [] recvBuff;

					SSimObj *tobj = w->getSObj(name.c_str());

					bool grasped = tobj->getIsGrasped();

					int sendSize = sizeof(bool);
					//char *sendBuff = new char[sendSize];
					char sendBuff[8];
					char *p = sendBuff;

					BINARY_SET_BOOL_INCR(p, grasped);

					if (!sendData(s, sendBuff, sendSize)) {
					  LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					break;
				}

				case REQUEST_SET_COLLISIONABLE:
				{
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}
					char *pp = recvBuff;
					bool flag = BINARY_GET_BOOL_INCR(pp);

					std::string name = strtok(pp, ",");
					SSimObj *obj = w->getSObj(name.c_str());

					std::map<std::string, SParts*> allj = obj->getAllParts();
					std::map<std::string, SParts*>::iterator it = allj.begin();
					while(it != allj.end()) {
						(*it).second->setCollisionEnable(flag);
						it++;
					}
					break;
				}
				
				case REQUEST_GET_SIMULATION_TIME:
				{
					double now = w->time();

					char sendBuff[16];
					char *p = sendBuff;

					BINARY_SET_DOUBLE_INCR(p, now);
					
					int sendSize = sizeof(double);

					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					break;
				}

				case REQUEST_GET_COLLISION_STATE:
				{
					unsigned short size = BINARY_GET_DATA_S_INCR(p, unsigned short);
					size -= 4;

					char *recvBuff = new char[size];

					if (!recvData(s, recvBuff, size)) {
						LOG_ERR(("Failed to recieve data [%s, %d]", __FILE__, __LINE__));
						delete [] recvBuff;
						continue;
					}

					char *pp = recvBuff;

					std::string name = strtok(pp, ",");
					std::string parts = strtok(NULL, ",");
					SSimObj *obj = w->getSObj(name.c_str());
					SParts *sparts = obj->getSParts(parts.c_str());

					bool state = sparts->getOnCollision();

					int sendSize = sizeof(bool);
					char sendBuff[8];
					char *p = sendBuff;

					BINARY_SET_BOOL_INCR(p, state);

					if (!sendData(s, sendBuff, sendSize)) {
						LOG_ERR(("Failed to send data [%s, %d]", __FILE__, __LINE__));
					}
					
					delete [] recvBuff;
					break;
				}
				///////////////////////////////////////
				/////////// add new case here!!////////
				///////////////////////////////////////

				default :
				{
					// Irregular data (trash data?)
					char tmp[256];  // TODO: magic number
					recv_nonblock(s, tmp, sizeof(tmp));
				}
	  
				} // switch(n) {

			} // if (0 < n && n < REQUEST_SIZE) {
			// if the 2bytes data is 0xabcd (request from SIGViewer)
			else if (n == COMM_DATA_PACKET_START_TOKEN)
			{
				packetSize = BINARY_GET_DATA_S_INCR(p, unsigned short);

				if (packetSize > firstRead) {
					r = recv_nonblock(s,
					         pbuf + firstRead,
					         packetSize - firstRead);
				}
			}
		}

		if (packetSize > 0) {
			int decoded;

			CommDataResult *result = clients[i]->decoder.push(*client, s_buf, packetSize, decoded);
			Source *client = clients[i]->source;

			if (decoded < 0) {

				LOG_ERROR(("bad backet : from = %s recv = %d, size=%d, type=%d\n",
						   client->name(),
						   packetSize,
						   *(unsigned short*)s_buf,
						   *(CommDataType*)(s_buf + 2)));
			} 
			else if (result != NULL) {

				assert(result->forwarded());
				bool completed = result->forwardCompleted();
				LOG_DEBUG1(("forwarded(%s)", completed?"completed":"not completed"));

				if (!completed) {
					LOG_DEBUG1(("packet(%d/%d)", result->seq(), result->packetNum()));
				}
				delete result;
				if (!completed) {
					forwarding = true;

					wait(waitTime);

					// FIX20110614(BIG-PACKET-RESPONSE-SLOW)
					// if the packet size of the forwarding message is huge, the response time was increased.
					// For exame, sending huge wave file from SIGViewer. It seems like a freezing
					// Modified no to wait more than 10 seconds
					if (waitTime < 10000) waitTime *= 2;

					goto again;
				}
				else {
					forwarding = false;
				}
			}
			// Check whether other data comes
			goto again;
		} // if (packetSize > 0) 
		else {
			if (forwarding) {
				goto again;
			}
		}
		// Encoding multiple data, and sending
		typedef std::vector<CommDataEncoder*> EncC;
		EncC encoders;

		std::vector<std::string> with;

		if (client->isControllerCmd()) {

			// Get result of collision detection
			const char *name = client->name();

			if (odeWorld->collideWith(name, with) > 0) {
				// Call of onCollision event handler
				encoders.push_back(new CommInvokeMethodOnCollisionEncoder(currTime, with));
				// TODO: check whether the memory free is not required
			}
			// Time is always sent. Is it required? TODO
			//if (running) {
			//encoders.push_back(new CommInvokeMethodOnActionEncoder(currTime));
			//}
		}

		if (r == -1
#ifndef WIN32
				&& (!errno || errno == EAGAIN)
#endif
			) { // connection is alive

			for (EncC::iterator ei=encoders.begin(); ei!=encoders.end(); ei++)
			{
				CommDataEncoder *target = *ei;
				int r = client->send(*target);

				if (r < 0) {
					LOG_ERR(("send error"));
				} else if (r == 0) {
					LOG_SYSTEM(("connection closed"));
					m_accept.pushNoNeeded(client);
					break;
				}
			}
		}

#ifndef WIN32
		// sekikawa(FIX20100826)
		else if (r < 0) {
			// detach client only when error occurred.
			//LOG_ERR(("r=%d: socket error (errno=%d, name=%s) [%s:%d]", r, errno, client->name(), __FILE__, __LINE__));
			//LOG_MSG(("ERR"));
			perror("Socket");
			//LOG_SYS(("Errno : %d",errno));
			m_accept.pushNoNeeded(client);
		}
#else
		else {
			if (r == 0) {
				m_accept.pushNoNeeded(client);
			}
		}
#endif

		for (EncC::iterator ei=encoders.begin(); ei!=encoders.end(); ei++) {
			CommDataEncoder *p = *ei;
			FREE(p);
		}
		
	}
	return true;
}


// Modified by Inamura on 2014-01-05: fixed memory leak bug
void WorldSimulator::startSimulation(SSimWorld *w)
{
	LOG_SYS(("****SIMULATION START****"));
	if (w && !w->isRunning()) {
		w->start();

		// Notice the start of the simulation
		const ServerAcceptProc::ConC clients = m_accept.clients();

		for (ServerAcceptProc::ConC::const_iterator i=clients.begin(); i!=clients.end(); i++) {

			Connection *con = *i;
			Source *c = con->source;
			if (c->isControllerCmd()) {

				// Buffer for message sending
				int dataSize = sizeof(unsigned short) * 2 + sizeof(double);
				char *sendBuff = new char[dataSize];
				char *p = sendBuff;

				// Notice the start to each controller
				BINARY_SET_DATA_S_INCR(p, unsigned short, 0x0002);  //TODO: magic number( enum ControllerDataType.START_SIM )
				BINARY_SET_DATA_S_INCR(p, unsigned short, dataSize);     

				// Sending the current time
				double currTime = w->time();
				BINARY_SET_DOUBLE_INCR(p, currTime);      

				sendData(c->socket(), sendBuff, dataSize);
				delete [] sendBuff;
			}
		}
	}
}


void WorldSimulator::loop(double endt)
{
	// Get the pointer of instance of Simulation world class
	SSimWorld *w = m_provider.get();

	if (!m_loop) {
		// Get time step width of the simulation
		double step = w->timeStep();

		// Transfer into micro second
		m_stepTime = 1000000*step;
		m_loop = true;
	}

	static timeval tv0; 
	//double step = w->timeStep();
	//int ms = (int)(1000000*step);
	//m_loop = true;

	// Start of the main loop
	while (m_loop) {

		// If the end time comes, the simulation should be finished
		if (endt >= 0 && w->time() >= endt)
			return;

		// Get the time stamp of the simulation start
		if (stepCount == 0) {
			gettimeofday(&tv0, NULL);
		}

		// If the runStep is failed, the simulation should be finished
		if (!runStep()) {
			m_loop = false;
			return;
		}

		// Get the current time
		struct timeval tv1;
		gettimeofday(&tv1, NULL);

		// Calculate time length from the start
		double time = (tv1.tv_sec - tv0.tv_sec)*1000000 + (tv1.tv_usec - tv0.tv_usec);

		// Measurement of actual time width for 1 step in the simulation
		// Only for the step should be progress automatically
		if (w->getAutoStep()) {

			// Wait for 1 step time width in SIGVerse simulation. 
			if (stepCount == 0) {

				wait((int)m_stepTime);
			}
			else {
				// Fit the actual time of realworld and SIGVerse simulator time
				double rad = m_stepTime - (time - (stepCount * m_stepTime));
				//LOG_DEBUG1(("m_stepTime = %f, rad = %f",m_stepTime ,rad));
				if (rad < 0) rad = 0;
				wait((int)rad);
			}
		}
		stepCount++;
		if (stepCount>10000)
			stepCount = 0;
	}
}



#endif // IRWAS_SIMSERVER

