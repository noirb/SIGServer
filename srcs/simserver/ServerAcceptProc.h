/*
 * Written by okamoto on 2012-06-14
 * Modified by Tetsunari Inamura on 2013-12-29
 *   Change Set/GetVelocity to Set/GetLinearVelocity
 *   comments translation is not checked yet
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change ObjectVelocity to AngularVelocityToParts
 *    Change JointVelocity  to AngularVelocityToJoint
 * Added SetLinearVelocity by Tetsunari Inamura on 2014-01-06
 * Added addTorque by Tetsunari Inamura on 2014-02-26
 */

#ifndef ServerAcceptProc_h
#define ServerAcceptProc_h

#ifdef IRWAS_SIMSERVER
#include "systemdef.h"
#include "Source.h"
#include "CommDataDecoder.h"
#include "Thread.h"
#include "Locker.h"
#include "Logger.h"

#include <string>
#include <vector>
#include <queue>
#include <map>

class SSimWorld;
class SimWorldProvider;
class ServiceNameServer;

class Connection
{
public:
	Source *source;
  
	CommDataDecoder decoder;
	Connection(Source *s,
			   CommDataDecoder::Listener *l,
			   CommDataDecoder::PacketSender *sender) : source(s)
	{
		if(source->type() != SOURCE_TYPE_NEW_VIEW)
			{
				decoder.setListener(l);
				decoder.setPacketSender(sender);
			}
	}
	~Connection() {
		source->close();
		delete source;
	}
};


class ServerAcceptProc : public CommDataDecoder::Listener
{
private:
	typedef std::string S;
		
private:
	typedef std::vector<Connection*> ConC;
	typedef std::vector<Source*> SourceC;
#ifdef DEPRECATED
	typedef std::queue<Command*> CommandQ;
	typedef std::map<S, CommandQ*> CommandHash;
#endif
public:
	typedef ConC C;
private:
	int	m_sock;
	
	std::vector<Connection *> m_clients;
	std::vector<Connection *> m_messages; 
	std::vector<Source*>	m_noneeded;
	Locker  m_locker;
#ifdef DEPRECATED
	CommandHash m_commandH;
#endif

	bool m_startReq;
private:
	CommDataDecoder::Listener *m_decoderL;
	CommDataDecoder::PacketSender *m_sender;
	SimWorldProvider &m_wProvider;
	ServiceNameServer &m_ns;
private:
#ifdef DEPRECATED
	CommandQ & getCommandQ(const char *name);
#endif
	void	free_();
public:
 ServerAcceptProc(int sock, SimWorldProvider &provider, ServiceNameServer &ns) : m_sock(sock), m_wProvider(provider), m_ns(ns), m_startReq(true) {;}
	~ServerAcceptProc() { free_(); }

	void setDecoderListener(CommDataDecoder::Listener *l) { m_decoderL = l; }
	void setPacketSender(CommDataDecoder::PacketSender *s) { m_sender = s; }

	void lock() { m_locker.lock(); }
	void unlock() { m_locker.unlock(); }

	const C	&clients() {
		eraseNoNeeded();
		return m_clients;
	}

	const C	&messages() {
		return m_messages;
	}
	int	clientNum() const { return m_clients.size(); }
	Source * get(const char *name, SourceType type);

	std::vector<Source*> getAllClients();

	std::vector<Source*> getAllCtlClients();

	std::vector<Source*> getAllSrvClients();

	Source * get(int sock);
	
	Source * getByType(SourceType type);

	void	pushNoNeeded(Source *source)
	{
		m_noneeded.push_back(source);
	}
	

#ifdef DEPRECATED
	void    push(Command *cmd);
	void	applyCommands(SSimWorld &w);
#endif
	void	eraseNoNeeded();
	void 	run();

	void	close();
private:
	/*
	 * CommDataDecoder::Listener implementation
	 */
	
	void recvRequestAttachController         (Source &from, RequestAttachControllerEvent &evt);
	void recvRequestAttachView               (Source &from, RequestAttachViewEvent &evt);
	
	void recvRequestProvideService           (Source &from, RequestProvideServiceEvent &evt);
	void recvRequestConnectDataPort          (Source &from, RequestConnectDataPortEvent &evt);
	
	void recvResultAttachController          (Source &from, ResultAttachControllerEvent &evt) {};
	void recvResultAttachView                (Source &from, ResultAttachViewEvent &evt) {};

	//		void recvResultProvideService(Source &from, ResultProvideServiceEvent &evt) {};

	//		void recvResultConnectDataProt(Source &from, ResultConnectDataPortEvent &evt) {};

	void recvRequestSimCtrl                  (Source &from, RequestSimCtrlEvent &evt) {};
		
	void recvRequestGetAllEntities           (Source &from, RequestGetAllEntitiesEvent &evt) {};
	void recvResultGetAllEntities            (Source &from, ResultGetAllEntitiesEvent &evt) {};
	void recvRequestUpdateEntities           (Source &from, RequestUpdateEntitiesEvent &evt) {};
	void recvRequestGetEntity                (Source &from, RequestGetEntityEvent &evt) {};
	void recvResultGetEntity                 (Source &from, ResultGetEntityEvent &evt) {};
		
	void recvRequestCaptureViewImage         (Source &from, RequestCaptureViewImageEvent &evt) {};
	void recvResultCaptureViewImage          (Source &from, ResultCaptureViewImageEvent &eve) {};

	void recvRequestDetectEntities           (Source &from, RequestDetectEntitiesEvent &evt) {};
	void recvResultDetectEntities            (Source &from, ResultDetectEntitiesEvent &eve) {};

	void recvRequestDistanceSensor           (Source &from, RequestDistanceSensorEvent &evt) {}; //added by okamoto on 2011-09-09
	void recvResultDistanceSensor            (Source &from, ResultDistanceSensorEvent &evt) {};	 //added by okamoto on 2011-09-12

	void recvLogMsg                          (Source &from, LogMsgEvent &evt) {};

#ifdef DEPRECATED
	void recvControllerCommand               (Source &from, ControllerCommandEvent &evt) {}
#endif

	void recvRequestNSQuery                  (Source &from, RequestNSQueryEvent &) {}
	void recvResultNSQuery                   (Source &from, ResultNSQueryEvent &) {}

	void recvRequestNSPinger                 (Source &from, RequestNSPingerEvent &) {}
	void recvResultNSPinger                  (Source &from, ResultNSPingerEvent &) {}

	void recvRequestSetJointAngle            (Source &from, RequestSetJointAngleEvent &) {};
	
	void recvRequestSetJointQuaternion       (Source &from, RequestSetJointQuaternionEvent &) {};   //added by Okamoto on 2011-02-18
	void recvRequestAddJointTorque           (Source &from, RequestAddJointTorqueEvent &) {};   	//added by Okamoto on 2011-03-03

	//added by Okamoto (2011/3/9)
	//modified by inamura on 2013-12-30
	void recvRequestSetAngularVelocityToJoint(Source &from, RequestSetAngularVelocityToJointEvent &) {};
	void recvRequestSetAngularVelocityToParts(Source &from, RequestSetAngularVelocityToPartsEvent &) {};

	//added by Okamoto (2011/3/9)
	void recvRequestGetJointAngle            (Source &from, RequestGetJointAngleEvent &) {};
	void recvResultGetJointAngle             (Source &from, ResultGetJointAngleEvent &) {};

	void recvDisplayText                     (Source &from, DisplayTextEvent &evt) {};	          // added by Okamoto on 2011-08-02

	//added by Okamoto (2011/12/19)
	void recvRequestAddForce                 (Source &from, RequestAddForceEvent &) {};
	void recvRequestAddForceAtPos            (Source &from, RequestAddForceAtPosEvent &) {};
	void recvRequestSetMass                  (Source &from, RequestSetMassEvent &) {};
	void recvRequestGetAngularVelocity       (Source &from, RequestGetAngularVelocityEvent &) {};
	void recvResultGetAngularVelocity        (Source &from, ResultGetAngularVelocityEvent &) {};
	void recvRequestGetLinearVelocity        (Source &from, RequestGetLinearVelocityEvent &) {};
	void recvResultGetLinearVelocity         (Source &from, ResultGetLinearVelocityEvent &) {};
	void recvRequestSetLinearVelocity        (Source &from, RequestSetLinearVelocityEvent &) {}; // added by inamura on 2014-01-06
	void recvRequestAddForceToParts          (Source &from, RequestAddForceToPartsEvent &) {};
	void recvRequestAddTorque                (Source &from, RequestAddTorqueEvent &) {};         // added by inamura on 2014-02-26
	void recvRequestSetGravityMode           (Source &from, RequestSetGravityModeEvent &) {};
	void recvRequestGetGravityMode           (Source &from, RequestGetGravityModeEvent &) {};
	void recvResultGetGravityMode            (Source &from, ResultGetGravityModeEvent &) {};
	void recvRequestSetDynamicsMode          (Source &from, RequestSetDynamicsModeEvent &) {};

	void recvRequestSoundRecog               (Source &from, RequestSoundRecogEvent &) {};
	void recvResultSoundRecog                (Source &from, ResultSoundRecogEvent &) {};
	void recvRequestGetJointForce            (Source &from, RequestGetJointForceEvent &) {};
	void recvResultGetJointForce             (Source &from, ResultGetJointForceEvent &) {};

	void recvRequestConnectJoint             (Source &from, RequestConnectJointEvent &) {};
	void recvRequestReleaseJoint             (Source &from, RequestReleaseJointEvent &) {};

	void recvRequestGetObjectNames           (Source &from, RequestGetObjectNamesEvent &) {};
	void recvResultGetObjectNames            (Source &from, ResultGetObjectNamesEvent &) {};

	//added by noma@tome (2012/02/20)
	void recvRequestGetPointedObject         (Source &from, RequestGetPointedObjectEvent &) {};
	void recvResultGetPointedObject          (Source &from, ResultGetPointedObjectEvent &) {};
};


class AcceptThread : public Thread<ServerAcceptProc *>
{
private:
	typedef Thread<ServerAcceptProc *> Super;
	static void * thread_func(void *param)
	{
		ServerAcceptProc *proc = (ServerAcceptProc*)param;
		proc->run();
		return 0;
	}
public:
	void run(ServerAcceptProc *proc)
	{
		Super::run(thread_func, proc);
	}
};

#endif // IRWAS_SIMSERVER

#endif // ServerAcceptProc_h
 

