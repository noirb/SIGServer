/*
 * Written by Okamoto on 2011-03-31
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-29
 *    Modify indent, add English comments
 */

#ifndef ControllerInf_h
#define ControllerInf_h

class InitEvent;
class ActionEvent;
class RecvTextEvent;
class RecvSoundEvent;
class RecvMessageEvent;
class RecvMsgEvent;
class CollisionEvent;

/**
 * @brief Controller interface
 */
class ControllerInf
{
public:
	ControllerInf(): m_startSim(false), m_excon(false) {;}

	//! Destructor
	virtual ~ControllerInf() {;}

	/**
	 * @brief Callback function to initialize a controller
	 *
	 * This function is called in onset of the simulation
	 */
	virtual void onInit(InitEvent &evt) {}
	
	/**
	 * @brief Action callback function
	 *
	 * Implements motion of an agents. Generally, this function is called back in every similation time.
	 *
	 * @return Time interval
	 */
	virtual double onAction(ActionEvent &evt) { return 0.0; }

	//! Callback function for receiving text
	virtual void onRecvText(RecvTextEvent &evt) {}
	//! Callback function for receiving sounds
	virtual void onRecvSound(RecvSoundEvent &evt) {}
	//old
	virtual void onRecvMessage(RecvMessageEvent &evt) {}
	//! Callback function for receiving messages (added on 2012-09-10)
	virtual void onRecvMsg(RecvMsgEvent &evt) {}
	//! Callback function for detecting collision
	virtual void onCollision(CollisionEvent &evt) {}

#ifndef UNIT_TEST
	//! Whether controller is in processing
	virtual bool isProcessing() = 0;
	//! Implements process before calling callback function
	virtual void onPreEvent() = 0;
	//! Implements process after calling callback function
	virtual void onPostEvent() = 0;
#else
	virtual bool isProcessing() { return false; };
	virtual void onPreEvent() {};
	virtual void onPostEvent() {};
#endif


	//! Gets the state of whether the simulation is running
	bool getSimState(){ return m_startSim; }

	//! Sets the state of whether the simulation is running
	void setSimState(bool sim){ m_startSim = sim; }

	// Gets the state of whether other thread uses functions of controller
	bool getExCon(){ return m_excon; }

	// Sets the state that a function of controller is while in use
	void setExCon(bool excon){ m_excon = excon; }

protected:
	//! Whether the simulation is running
	bool m_startSim;

	//! For exclusive access control
	bool m_excon;
};


#endif // ControllerInf_h
 
 


