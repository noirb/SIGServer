/*
 * Written by Atsushi Okamoto on 2011-03-31
 * Modified by Tetsunari Inamura on 2013-12-29
 *    Add english comments  (Translation from v2.2.0 is finished)
 */ 
#ifndef CommDataDecoder_h
#define CommDataDecoder_h

#include <string>
#include <map>

#include "CommDataDef.h"
#include "CommDataListener.h"
#include "CommDataResult.h"

class Source;
class ControllerInf;
class CommDataHeader;

/**
 * @brief Class for protcol decoder in communication between server and client
 *
 * Decoding the data which is encoded by SIGVerse communication protcol for server/client communication.
 * Decoded data will be used in two ways; 1) synchronous system and 2) asynchronous system.

   <dl>
   <dt> in synchronous system: return value of push method will be used</dt>
   <dd><pre>

        CommDataDecoder decoder;
        Source &src = ..
	char * buf = ...;
	int size = ...;
	int decoded = 0
	typedef CommDataDecoder::Result Result;
	Result *res = decoder.push(src, buf, size, decoded);
	if (decoded > 0 && res != NULL) {
		if (res->type() ==  COMM_RESULT_GET_ALL_ENTITIES) {
		    ResultGetAllEntitiesEvent * evt =  (ResultGetAllEntitiesEvent * evt) res->data();
		}
	}
    </pre></dd>
   <dt> in asynchronous system: data will be used through CommDataListener</dt>
   <dd><pre>
        class CommDataListnerImpl : public CommDataListener
	{
	private:
	     recvResultGetAllEntities(Source &from, ResultGetAllEntitiesEvent &evt) {
	         ...; // procedures for receiving
	     }
	};
	CommDataListenerImpl impl;
        CommDataDecoder decoder;
	decoder.setDecoderListener(&impl);
	
        Source &src = ..
	char * buf = ...;
	int size = ...;
	int decoded decoder.push(src, buf, size);

    </pre></dd>
    </dl>
 */
class CommDataDecoder
{
public:
	typedef CommDataListener Listener;
	typedef CommDataListener L;
	typedef CommDataResult 	 Result;
	typedef CommDataResult::DataFreeFunc DataFreeFunc;

	/**
	 * @brief Interface for data transport
	 */
	class PacketSender {
	public:
		//! Destructor
		virtual ~PacketSender() {;}

		/**
		 * @brief Forwarding the data to other client
		 * This function will be called when the forward flag is attached in the data
		 *
		 * @param from  data source
		 * @param type  data type
		 * @param to    client name of the forward destination
		 * @param bytes data size
		 * @param data  contents of data(?)
		 * @param returnImmediate flag whether receive the result of forwarding
		 * @param reachRadius     radius of reachable circle
		 */
		virtual void forward(Source &from, CommDataType type, const char *to, int bytes, char *data, bool returnImmediate, double reachRadius) = 0;
	};
public:

	/**
	 * @brief Each data decoder interface (Used in CommDataDecoder)
	 */
	class DecoderBase
	{
	public:
		//! Destructor
		virtual ~DecoderBase() {;}

		/**
		 * @brief  Analysis of forwarded data
		 * @param h    header of data
		 * @param data data itself
		 * @param n    size of data area
		 */
		virtual bool 	set (CommDataHeader &h, char *data, int n) = 0;

		//! Call of corresponding CommDataListener method or get of decoded data
		virtual void *   invoke(Source &from) = 0;

		//! Refer the data type
		virtual CommDataType dataType() = 0;

		//! Refer the destructor of data object
		virtual DataFreeFunc dataFreeFunc() = 0;

		//! Creation of data decoder class
		Result *createResult(void *);
	};
private:
	L	*m_l;
	ControllerInf *m_ctrl;
	PacketSender *m_sender;
	double	m_nextTime;
	DecoderBase *m_decoder;
private:
	void	free_();
	DecoderBase *createDecoder(CommDataType type);
public:
	//! Constructor
	CommDataDecoder() : m_l(0), m_ctrl(0), m_sender(0), m_nextTime(0.0), m_decoder(0) {;}
	//! Destructor
	~CommDataDecoder() { free_(); }

	/**
	 * @brief Configuration of Listener
	 *
	 * Implimentation of CommDataListener which has implementation of process of transport result
	 */
	void	setListener(Listener *l) { m_l = l; }

	//! Configuration of controller
	void	setController(ControllerInf *c) { m_ctrl = c; }

	//! Refer controller
	ControllerInf  *getController() { return m_ctrl; }

	//! Configuration of PacketSender
	void setPacketSender(PacketSender *s) { m_sender = s; }

	/**
	 * @brief Decoding the received data
	 * decoding will be executed asynchronous
	 *
	 * @param from source of data
	 * @param data received data
	 * @param n    data size
	 *
	 * @retval >= 0 : success (number of byte of decoded data)
	 * @retval < 0 :  fail
	 */	
	int push(Source &from, char *data, int n);

	/**
	 * @brief Decoding the received data
	 * decoding result will be referred as return value
	 *
	 * @param from source of data
	 * @param data received data
	 * @param n    data size
	 * @param decoderbase decoded data size
	 *
	 * @retval >= 0 : success (number of byte of decoded data)
	 * @retval < 0 :  fail
	 */	
	Result * push(Source &from, char *data, int n, int &decodedBytes);
	
	//! Finilization of decode
	void	clear();

	//! Set of next time (used in inner side)
	void	setNextTime(double t)
	{
		m_nextTime = t;
	}

	//! Set of next time (used in inner side)
	double	getNextTime() { return m_nextTime; }
};

#endif // CommDataDecoder_h
 

