/*
 * Written by noma on 2012-03-27
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add English comments (Translation from v2.2.0 is finished)
 *    Change get_velocity to get_linear_velocity
 * Modified by Tetsunari Inamura on 2013-12-30
 *    Change ObjectVelocityData to AngularVelocityToPartsData
 *    Change JointVelocityData  to AngularVelocityToJointData
 * Added SetLinearVelocity by Tetsunari Inamura on 2014-01-06
 * Added addTorque by Tetsunari Inamura on 2014-02-26
 */

#include "CommDataDecoder.h"
#include "CommDataDef.h"
#include "comm/Header.h"
#include "binary.h"
#include "SimObj.h"
#include "SimWorld.h"

#include "ControllerInf.h"
#include "ControllerEvent.h"

#include <vector>

#define FREE(P) if (P) { delete P; P = 0; }

void CommDataDecoder::free_()
{
	FREE(m_decoder);
}


void CommDataDecoder::clear()
{
	FREE(m_decoder);
}

#if 0
#define LOG(MSG) printf MSG
#else
#define LOG(MSG) 
#endif

#define getHeader CommData::getHeader

class DecoderException {};

typedef CommDataDecoder::Listener L;

static bool LAST_PACKET_DECODED(CommDataDecoder::DecoderBase &d, CommDataHeader &h, char *p, int bytes)
{
	return d.set(h, p, bytes) && (h.seq + 1 == h.packetNum)? true: false;
}

CommDataDecoder::Result *CommDataDecoder::DecoderBase::createResult(void *p)
{
	typedef CommDataDecoder::Result R;
	return new R(dataType(), p, dataFreeFunc());
}


int CommDataDecoder::push(Source &from, char *data, int n)
{
	int readBytes = 0;
	Result *p = push(from, data, n, readBytes);
	if (p) { delete p; }
	return readBytes;
}

		
CommDataDecoder::Result* CommDataDecoder::push(Source &from, char *data, int bytes, int &readBytes)
{
	char *curr = data;
	CommDataHeader h;

	Result *ret = NULL;
	readBytes = 0;
	/*
	 * Decoder is always executed when event handler is called
	 * A series of data should be sent after the all of the decode, and finished
	 */
	while (ret == NULL && bytes > 0) {

		int r = getHeader(curr, bytes, h);
		if (r < 0) {
			LOG(("cannot get header"));
			goto err;
		} 

		// Packet is not perfect or head information is not enough
		if (r == 0 || bytes < h.packetSize) {
			LOG(("incomplete packet"));
			return NULL;
		}
		LOG(("packet type : %d (%d/%d)\n", h.type, h.seq, h.packetNum));

		int nbytes = bytes - h.packetSize;

		if (m_sender && ( h.forwardFlags & COMM_DATA_FORWARD)) {
			const char *to = h.forwardTo.length() > 0? h.forwardTo.c_str(): NULL;
			bool immediate = h.forwardFlags & COMM_DATA_FORWARD_RETURN_IMMEDIATE? true: false;

			double radius = h.forwardReachRadius;
			m_sender->forward(from, h.type, to,  h.packetSize, curr, immediate, radius);
			ret = new Result(h.type, h.seq, h.packetNum);
		} else {
			char *p = curr;

			p += r; bytes -= r;
			try {
				if (m_decoder) {
					if (LAST_PACKET_DECODED(*m_decoder, h, p, bytes)) {
						void *data = m_decoder->invoke(from);
						if (data) {
							ret = m_decoder->createResult(data);
						}
						FREE(m_decoder);
					}
				} else {
					if (h.seq == 0) { // Ignore if the packet is not the first
						DecoderBase *decoder = createDecoder(h.type);
						if (decoder) {
							if (LAST_PACKET_DECODED(*decoder, h, p, bytes)) {
								void *data = decoder->invoke(from);
								if (data) {
									ret = decoder->createResult(data);
								}
								FREE(decoder);
							} else {
								m_decoder = decoder;
							}
						}
					}
				}
			} catch(DecoderException &) {
				goto err;
			}

		}

		// Recognize as 'reading is finished' if the header and packet is existed
		char *next = curr + h.packetSize;
		readBytes = next - data;
		bytes = nbytes;
		curr = next;
	}
		
	return ret;
 err:
	FREE(m_decoder);
	readBytes = -1;
	return NULL;
}


template <CommDataType TYPE_, class E>
class Decoder : public CommDataDecoder::DecoderBase
{
public:
	enum {
		TYPE = TYPE_,
	};
private:
	E * m_evt;
	int 	m_prev;
public:
	Decoder() : CommDataDecoder::DecoderBase(), m_evt(NULL), m_prev(-1) {}
	virtual ~Decoder() {
		FREE(m_evt);
	}
public:
	bool	set(CommDataHeader &h, char *data, int n)
	{
		if (m_prev != h.seq -1) {
			throw DecoderException();
		}

		if (!m_evt) {
			m_evt = new E();
		}
		bool b = m_evt->set(h.packetNum, h.seq, data, n);
		if (!b) {
			throw DecoderException();
		}

		m_prev = h.seq;

		return h.seq +1 == h.packetNum && b;
	}
	
	E *	getEvent() { return m_evt; }
	E *	releaseEvent() {
		E *tmp = m_evt;
		m_evt = NULL;
		return tmp;
	}
};


template <CommDataType TYPE_, class E>
class ListenerInvokeDecoder : public Decoder<TYPE_, E>
{
private:
	typedef Decoder<TYPE_, E> Super;
	typedef void (L::*Method)(Source &, E &);
private:
	L	*m_listener;
	Method  m_method;
private:
	static void free_data(void *data) {
		E *e = (E*)data;
		delete e;
	}
public:
	ListenerInvokeDecoder()
		: Super(), m_listener(NULL), m_method(NULL) {}

	CommDataType dataType() { return TYPE_; }
	CommDataDecoder::DataFreeFunc dataFreeFunc() { return free_data; }

	void	setCallback(L *l, Method m)
	{
		m_listener = l;
		m_method = m;
	}
	
	void *   invoke(Source &from) {
		if (m_listener && m_method) {
			E *evt = Super::getEvent();
			(m_listener->*m_method)(from, *evt);
			return NULL;
		} else {
			E *evt = Super::releaseEvent();
			return (void*)evt;
		}
	}
};

typedef ListenerInvokeDecoder<COMM_REQUEST_ATTACH_CONTROLLER,    RequestAttachControllerEvent>   ReqAttachControllerD;
typedef ListenerInvokeDecoder<COMM_RESULT_ATTACH_CONTROLLER,     ResultAttachControllerEvent>    ResAttachControllerD;

typedef ListenerInvokeDecoder<COMM_REQUEST_CONNECT_DATA_PORT,    RequestConnectDataPortEvent>    ReqConnectDataPortD;

typedef ListenerInvokeDecoder<COMM_REQUEST_ATTACH_VIEW,          RequestAttachViewEvent>         ReqAttachViewD;
typedef ListenerInvokeDecoder<COMM_RESULT_ATTACH_VIEW,           ResultAttachViewEvent>          ResAttachViewD;

typedef ListenerInvokeDecoder<COMM_REQUEST_PROVIDE_SERVICE,      RequestProvideServiceEvent>     ReqProvideServiceD;

typedef ListenerInvokeDecoder<COMM_REQUEST_SIM_CTRL,             RequestSimCtrlEvent>            ReqSimCtrlD;

typedef ListenerInvokeDecoder<COMM_REQUEST_GET_ALL_ENTITIES,     RequestGetAllEntitiesEvent>     ReqGetAllEntitiesD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_ALL_ENTITIES,      ResultGetAllEntitiesEvent>      ResGetAllEntitiesD;

typedef ListenerInvokeDecoder<COMM_REQUEST_UPDATE_ENTITIES,      RequestUpdateEntitiesEvent>     ReqUpdateEntitiesD;

typedef ListenerInvokeDecoder<COMM_REQUEST_GET_ENTITY,           RequestGetEntityEvent>          ReqGetEntityD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_ENTITY,            ResultGetEntityEvent>           ResGetEntityD;

typedef ListenerInvokeDecoder<COMM_REQUEST_CAPTURE_VIEW_IMAGE,   RequestCaptureViewImageEvent>   ReqCaptureViewImageD;
typedef ListenerInvokeDecoder<COMM_RESULT_CAPTURE_VIEW_IMAGE,    ResultCaptureViewImageEvent>    ResCaptureViewImageD;

typedef ListenerInvokeDecoder<COMM_REQUEST_DETECT_ENTITIES,      RequestDetectEntitiesEvent>     ReqDetectEntitiesD;
typedef ListenerInvokeDecoder<COMM_RESULT_DETECT_ENTITIES,       ResultDetectEntitiesEvent>      ResDetectEntitiesD;

typedef ListenerInvokeDecoder<COMM_REQUEST_DISTANCE_SENSOR,      RequestDistanceSensorEvent>     ReqDistanceSensorD;
typedef ListenerInvokeDecoder<COMM_RESULT_DISTANCE_SENSOR,       ResultDistanceSensorEvent>      ResDistanceSensorD;

// Attribute
typedef ListenerInvokeDecoder<COMM_REQUEST_GET_ATTRIBUTES,       RequestGetAttributesEvent>      ReqGetAttrsD;

typedef ListenerInvokeDecoder<COMM_REQUEST_SET_JOINT_ANGLE,      RequestSetJointAngleEvent>      ReqSetJointAngleD;

//added by okamoto@tome (2011/2/18)
typedef ListenerInvokeDecoder<COMM_REQUEST_SET_JOINT_QUATERNION, RequestSetJointQuaternionEvent> ReqSetJointQuaternionD;

//added by okamoto@tome (2011/3/3)
typedef ListenerInvokeDecoder<COMM_REQUEST_ADD_JOINT_TORQUE,     RequestAddJointTorqueEvent>     ReqAddJointTorqueD;

//added by okamoto@tome (2011/3/9)
//modified by inamura on 2013-12-30
typedef ListenerInvokeDecoder<COMM_REQUEST_SET_ANGULAR_VELOCITY_JOINT, RequestSetAngularVelocityToJointEvent>  ReqSetAngularVelocityToJointD;
typedef ListenerInvokeDecoder<COMM_REQUEST_SET_ANGULAR_VELOCITY_PARTS, RequestSetAngularVelocityToPartsEvent>  ReqSetAngularVelocityToPartsD;

//added by okamoto@tome (2011/3/9)
typedef ListenerInvokeDecoder<COMM_REQUEST_GET_JOINT_ANGLE,      RequestGetJointAngleEvent>      ReqGetJointAngleD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_JOINT_ANGLE,       ResultGetJointAngleEvent>       ResGetJointAngleD;

//added by okamoto@tome (2011/12/22)
typedef ListenerInvokeDecoder<COMM_REQUEST_ADD_FORCE,            RequestAddForceEvent>           ReqAddForceD;
typedef ListenerInvokeDecoder<COMM_REQUEST_ADD_FORCE_ATPOS,      RequestAddForceAtPosEvent>      ReqAddForceAtPosD;
typedef ListenerInvokeDecoder<COMM_REQUEST_SET_MASS,             RequestSetMassEvent>            ReqSetMassD;
typedef ListenerInvokeDecoder<COMM_REQUEST_GET_ANGULAR_VELOCITY, RequestGetAngularVelocityEvent> ReqGetAngularVelocityD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_ANGULAR_VELOCITY,  ResultGetAngularVelocityEvent>  ResGetAngularVelocityD;
typedef ListenerInvokeDecoder<COMM_REQUEST_ADD_FORCE_TOPARTS,    RequestAddForceToPartsEvent>    ReqAddForceToPartsD;

typedef ListenerInvokeDecoder<COMM_REQUEST_ADD_TORQUE,           RequestAddTorqueEvent>          ReqAddTorqueD;         // added by inamura on 2014-02-26

typedef ListenerInvokeDecoder<COMM_REQUEST_GET_LINEAR_VELOCITY,  RequestGetLinearVelocityEvent>  ReqGetLinearVelocityD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_LINEAR_VELOCITY,   ResultGetLinearVelocityEvent>   ResGetLinearVelocityD;
typedef ListenerInvokeDecoder<COMM_REQUEST_SET_LINEAR_VELOCITY,  RequestSetLinearVelocityEvent>  ReqSetLinearVelocityD; // added by inamura on 2014-01-06

typedef ListenerInvokeDecoder<COMM_REQUEST_SET_GRAVITY_MODE,     RequestSetGravityModeEvent>     ReqSetGravityModeD;
typedef ListenerInvokeDecoder<COMM_REQUEST_GET_GRAVITY_MODE,     RequestGetGravityModeEvent>     ReqGetGravityModeD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_GRAVITY_MODE,      ResultGetGravityModeEvent>      ResGetGravityModeD;
typedef ListenerInvokeDecoder<COMM_REQUEST_SET_DYNAMICS_MODE,    RequestSetDynamicsModeEvent>    ReqSetDynamicsModeD;


typedef ListenerInvokeDecoder<COMM_REQUEST_SOUND_RECOG,          RequestSoundRecogEvent>         ReqSoundRecogD;
typedef ListenerInvokeDecoder<COMM_RESULT_SOUND_RECOG,           ResultSoundRecogEvent>          ResSoundRecogD;

typedef ListenerInvokeDecoder<COMM_REQUEST_GET_JOINT_FORCE,      RequestGetJointForceEvent>      ReqGetJointForceD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_JOINT_FORCE,       ResultGetJointForceEvent>       ResGetJointForceD;

typedef ListenerInvokeDecoder<COMM_REQUEST_CONNECT_JOINT,        RequestConnectJointEvent>       ReqConnectJointD;
typedef ListenerInvokeDecoder<COMM_REQUEST_RELEASE_JOINT,        RequestReleaseJointEvent>       ReqReleaseJointD;


// NameService
typedef ListenerInvokeDecoder<COMM_NS_QUERY_REQUEST,             RequestNSQueryEvent>            ReqNSQueryD;
typedef ListenerInvokeDecoder<COMM_NS_QUERY_RESULT,              ResultNSQueryEvent>             ResNSQueryD;

typedef ListenerInvokeDecoder<COMM_NS_PINGER_REQUEST,            RequestNSPingerEvent>           ReqNSPingerD;
typedef ListenerInvokeDecoder<COMM_NS_PINGER_RESULT,             ResultNSPingerEvent>            ResNSPingerD;

typedef ListenerInvokeDecoder<COMM_LOG_MSG,                      LogMsgEvent>                    CommLogMsgD;

//added by okamoto@tome (2011/8/2)
typedef ListenerInvokeDecoder<COMM_DISPLAY_TEXT,                 DisplayTextEvent>               CommDisplayTextD;

#ifdef DEPRECATED
typedef ListenerInvokeDecoder<COMM_CONTROLLER_COMMAND,           ControllerCommandEvent>         CtrlCmdD;
#endif

typedef ListenerInvokeDecoder<COMM_REQUEST_GET_OBJECT_NAMES,     RequestGetObjectNamesEvent>     ReqGetObjectNamesD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_OBJECT_NAMES,      ResultGetObjectNamesEvent>      ResGetObjectNamesD;

//added by noma@tome (2012/02/20)
typedef ListenerInvokeDecoder<COMM_REQUEST_GET_POINTED_OBJECT,   RequestGetPointedObjectEvent>   ReqGetPointedObjectD;
typedef ListenerInvokeDecoder<COMM_RESULT_GET_POINTED_OBJECT,    ResultGetPointedObjectEvent>    ResGetPointedObjectD;



template <CommDataType TYPE_, class E>
class ControllerInvokeDecoder : public Decoder<TYPE_, E>
{
	typedef Decoder<TYPE_, E> Super;
	typedef ControllerInf C;
	typedef void (C::*Method)(E &);
protected:
	C	*m_ctrl;
	Method  m_method;
	CommDataDecoder *m_decoder;
public:
	ControllerInvokeDecoder() : Super(), m_ctrl(0), m_method(0), m_decoder(0) {;}
	void	setCallback(C *ctrl, Method m)
	{
		m_ctrl = ctrl;
		m_method = m;
	}
	void	setDecoder(CommDataDecoder *d) { m_decoder = d; }

	CommDataType dataType() { assert(0); return TYPE_; }
	CommDataDecoder::DataFreeFunc dataFreeFunc() { assert(0); return NULL; }

	void *   invoke(Source &from)
	{
		if (!m_ctrl->isProcessing()) {
			m_ctrl->onPreEvent();
			(m_ctrl->*m_method)(*Super::getEvent());
			m_ctrl->onPostEvent();
		}
		return NULL;
	}

};

class InvokeOnActionD : public ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_ACTION, ActionEvent>
{
private:
	typedef ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_ACTION, ActionEvent> Super;
public:
	void *   invoke(Source &from)
	{
		if (m_ctrl->isProcessing()) {
			printf("onAction is processing. ignored\n");
			return NULL;
		}

		ActionEvent *evt = Super::getEvent();
		double t = evt->time();
		if (t >= m_decoder->getNextTime()) {
			m_ctrl->onPreEvent();
			double step = m_ctrl->onAction(*evt);
			m_ctrl->onPostEvent();
			double nextTime = t + step;
			m_decoder->setNextTime(nextTime);
		}
		return NULL;
	}
};


class InvokeOnInitD : public ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_INIT, InitEvent>
{
private:
	typedef ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_INIT, InitEvent> Super;
public:
	void *   invoke(Source &from)
	{
		Super::invoke(from);
		m_decoder->setNextTime(0.0);
		return NULL;
	}
};

typedef ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_RECV_TEXT,    RecvTextEvent>    InvokeOnRecvTextD;
typedef ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_RECV_SOUND,   RecvSoundEvent>   InvokeOnRecvSoundD;
typedef ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_RECV_MESSAGE, RecvMessageEvent> InvokeOnRecvMessageD;
typedef ControllerInvokeDecoder<COMM_INVOKE_CONTROLLER_ON_COLLISION,    CollisionEvent>   InvokeOnCollisionD;

CommDataDecoder::DecoderBase *CommDataDecoder::createDecoder(CommDataType type)
{
	typedef Listener L;
#define CREATE_L_DECODER(DECODER, CB) \
	 if (type == DECODER::TYPE) {     \
	     DECODER *d = new DECODER();  \
	     if (m_l) { d->setCallback(m_l, CB); }\
	     return (CommDataDecoder::DecoderBase*)d; \
	 }

#define CREATE_M_DECODER(DECODER, CB) \
	if (type == DECODER::TYPE && m_ctrl) { \
	    DECODER *d = new DECODER();        \
	    d->setCallback(m_ctrl, CB);        \
	    d->setDecoder(this); \
	    return (CommDataDecoder::DecoderBase*)d; \
	}

	CREATE_L_DECODER(ReqGetAllEntitiesD,     &L::recvRequestGetAllEntities); // not tested
	CREATE_L_DECODER(ResGetAllEntitiesD,     &L::recvResultGetAllEntities);  // not tested
	CREATE_L_DECODER(ReqUpdateEntitiesD,     &L::recvRequestUpdateEntities); // not tested

	CREATE_L_DECODER(ReqGetEntityD,          &L::recvRequestGetEntity);
	CREATE_L_DECODER(ResGetEntityD,          &L::recvResultGetEntity);
	
	CREATE_L_DECODER(ReqSimCtrlD,            &L::recvRequestSimCtrl);

	CREATE_L_DECODER(ReqAttachControllerD,   &L::recvRequestAttachController);
	CREATE_L_DECODER(ResAttachControllerD,   &L::recvResultAttachController);
	CREATE_L_DECODER(ReqAttachViewD,         &L::recvRequestAttachView);
	CREATE_L_DECODER(ResAttachViewD,         &L::recvResultAttachView);

	CREATE_L_DECODER(ReqConnectDataPortD,    &L::recvRequestConnectDataPort); // not tested
	CREATE_L_DECODER(ReqProvideServiceD,     &L::recvRequestProvideService);  // not tested
	CREATE_L_DECODER(CommLogMsgD,            &L::recvLogMsg);

	//added by okamoto@tome (2011/8/2)
	CREATE_L_DECODER(CommDisplayTextD,       &L::recvDisplayText);

	CREATE_L_DECODER(ReqCaptureViewImageD,   &L::recvRequestCaptureViewImage);
	CREATE_L_DECODER(ResCaptureViewImageD,   &L::recvResultCaptureViewImage);
		
	CREATE_L_DECODER(ReqDetectEntitiesD,     &L::recvRequestDetectEntities);
	CREATE_L_DECODER(ResDetectEntitiesD,     &L::recvResultDetectEntities);

	CREATE_L_DECODER(ReqDistanceSensorD,     &L::recvRequestDistanceSensor);
	CREATE_L_DECODER(ResDistanceSensorD,     &L::recvResultDistanceSensor);

	CREATE_L_DECODER(ReqGetAttrsD, NULL);

	CREATE_L_DECODER(ReqNSQueryD,            &L::recvRequestNSQuery);
	CREATE_L_DECODER(ResNSQueryD,            &L::recvResultNSQuery);
	CREATE_L_DECODER(ReqNSPingerD,           &L::recvRequestNSPinger);
	CREATE_L_DECODER(ResNSPingerD,           &L::recvResultNSPinger);

	CREATE_L_DECODER(ReqSetJointAngleD,      &L::recvRequestSetJointAngle);

	//added by okamoto@tome (2011/2/18)
	CREATE_L_DECODER(ReqSetJointQuaternionD, &L::recvRequestSetJointQuaternion);

	//added by okamoto@tome (2011/3/3)
	CREATE_L_DECODER(ReqAddJointTorqueD,     &L::recvRequestAddJointTorque);

	//added by okamoto@tome (2011/3/9)
	//modified by inamura on 2013-12-30
	CREATE_L_DECODER(ReqSetAngularVelocityToJointD,  &L::recvRequestSetAngularVelocityToJoint);
	CREATE_L_DECODER(ReqSetAngularVelocityToPartsD,  &L::recvRequestSetAngularVelocityToParts);

	//added by okamoto@tome (2011/3/9)
	CREATE_L_DECODER(ReqGetJointAngleD,      &L::recvRequestGetJointAngle);
	CREATE_L_DECODER(ResGetJointAngleD,      &L::recvResultGetJointAngle);

	//added by okamoto@tome (2011/12/19)
	CREATE_L_DECODER(ReqAddForceD,           &L::recvRequestAddForce);
	CREATE_L_DECODER(ReqAddForceAtPosD,      &L::recvRequestAddForceAtPos);
	CREATE_L_DECODER(ReqSetMassD,            &L::recvRequestSetMass);
	CREATE_L_DECODER(ReqGetAngularVelocityD, &L::recvRequestGetAngularVelocity);
	CREATE_L_DECODER(ResGetAngularVelocityD, &L::recvResultGetAngularVelocity);
	CREATE_L_DECODER(ReqGetLinearVelocityD,  &L::recvRequestGetLinearVelocity);
	CREATE_L_DECODER(ResGetLinearVelocityD,  &L::recvResultGetLinearVelocity);
	CREATE_L_DECODER(ReqSetLinearVelocityD,  &L::recvRequestSetLinearVelocity); // added by inamura on 2014-01-06
	CREATE_L_DECODER(ReqAddForceToPartsD,    &L::recvRequestAddForceToParts);
	CREATE_L_DECODER(ReqAddTorqueD,          &L::recvRequestAddTorque);         // added by inamura on 2014-02-26

	CREATE_L_DECODER(ReqSetGravityModeD,     &L::recvRequestSetGravityMode);
	CREATE_L_DECODER(ReqGetGravityModeD,     &L::recvRequestGetGravityMode);
	CREATE_L_DECODER(ResGetGravityModeD,     &L::recvResultGetGravityMode);
	CREATE_L_DECODER(ReqSetDynamicsModeD,    &L::recvRequestSetDynamicsMode);

	CREATE_L_DECODER(ReqSoundRecogD,         &L::recvRequestSoundRecog);
	CREATE_L_DECODER(ResSoundRecogD,         &L::recvResultSoundRecog);
	CREATE_L_DECODER(ReqGetJointForceD,      &L::recvRequestGetJointForce);
	CREATE_L_DECODER(ResGetJointForceD,      &L::recvResultGetJointForce);
	CREATE_L_DECODER(ReqConnectJointD,       &L::recvRequestConnectJoint);
	CREATE_L_DECODER(ReqReleaseJointD,       &L::recvRequestReleaseJoint);

	CREATE_L_DECODER(ReqGetObjectNamesD,     &L::recvRequestGetObjectNames);
	CREATE_L_DECODER(ResGetObjectNamesD,     &L::recvResultGetObjectNames);

	//added by noma@tome(2012/02/20)
	CREATE_L_DECODER(ReqGetPointedObjectD,   &L::recvRequestGetPointedObject);
	CREATE_L_DECODER(ResGetPointedObjectD,   &L::recvResultGetPointedObject);

#ifdef DEPRECATED
	CREATE_L_DECODER(CtrlCmdD,               &L::recvControllerCommand);
#endif

	CREATE_M_DECODER(InvokeOnActionD,        0);
	CREATE_M_DECODER(InvokeOnInitD,          &ControllerInf::onInit);
	CREATE_M_DECODER(InvokeOnRecvTextD,      &ControllerInf::onRecvText);
	CREATE_M_DECODER(InvokeOnRecvSoundD,     &ControllerInf::onRecvSound);
	CREATE_M_DECODER(InvokeOnRecvMessageD,   &ControllerInf::onRecvMessage);
	CREATE_M_DECODER(InvokeOnCollisionD,     &ControllerInf::onCollision);
	
	return NULL;
}

#ifdef Decoder_test

#undef getHeader

#include "command/Move.h"
#include "command/JointControl.h"
#include "comm/encoder/ControllerCommand.h"
#include "comm/Header.h"
#include "Source.h"

static Command * create()
{
	//return new MoveCommand("foo", Vector3d(1, 2, 3), 5);
	JointControlCommand *cmd = new JointControlCommand("foo");
	cmd->set("JOINT1", 1.0);
	cmd->set("JOINT2", -1.0);
	return cmd;
}

class Listener : public CommDataDecoder::Listener
{
	void recvControllerCommand(Source &from, ControllerCommandEvent &evt)
	{
		printf("recvControllerCommand called\n");
	}
};

int main(int argc, char **argv)
{
	Source source(1, "localhost");
	Command *cmd = create();
	ControllerCommandEncoder enc(*cmd);
	int n;
	char *buf = enc.encode(0, n);
	printf("data bytes = %d\n", n);
	if (buf) {
		CommDataDecoder decoder;
		Listener l;
		decoder.setListener(&l);
		int read = 0;

		int nn = 20;
		while (read < n) {
			int left = n - read;
			char *p = buf + read;
			printf("to read = %d\n", nn);
			int ret = decoder.push(source, p, nn);
			printf("ret = %d\n", ret);
			if (ret == nn) {
				break;
			} else if (ret >= 0) {
				read += ret;
				nn += 20;
			}
		}
	}

	return 0;

}
#endif

