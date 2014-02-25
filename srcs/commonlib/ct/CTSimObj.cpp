/* $Id: CTSimObj.cpp,v 1.19 2011-11-18 06:26:30 okamoto Exp $ */
#include "CTSimObj.h"
#include "CommDataEncoder.h"
#include "CommDataDecoder.h"
#include "CTReader.h"
#include "Logger.h"
#include "CommUtil.h"
#include "MessageSender.h"
#include "NSLookup.h"
#include "Text.h"
#include <unistd.h>

#define FREE(P) if (P) { delete P; P = 0; }

CTSimObj::~CTSimObj()
{
	FREE(m_ns);

	if (m_close && m_sock >= 0) {
	  // fix(sekikawa)(2009/3/3)
#ifdef WIN32
	  closesocket(m_sock);
#else
	  close(m_sock);
#endif
		m_sock = -1;
	}
}

typedef CommDataDecoder::Result Result;

bool CTSimObj::connect(const char *server, int port, const char *agentName)
{
	SOCKET s = CommUtil::connectServer(server, port);
	if (s < 0) {
		// add(sekikawa)(FIX20100826)
		LOG_ERR(("connectServer() failed. (hostname=%s, port=%d) [%s:%d]",
			server, port, __FILE__, __LINE__));

		return false;
	}

#if 0
	CommRequestConnectDataPortEncoder enc(agentName);
	enc.send(s);
#endif

	m_sock = s;
	m_myname = agentName;
	m_close = true;
	return true;
}

NSLookup::Provider * CTSimObj::lookupProvider(Service::Kind kind)
{
	if (!m_ns) {
		if (m_sock < 0) { return NULL; }
		m_ns = new NSLookup(m_sock);
	}
	return m_ns->lookup(kind);
}

/*
Attribute * CTSimObj::getAttr(const char *name)
{
	return NULL;
}
*/

bool CTSimObj::detectEntities(EntityNameC &v, int id)
{
	NSLookup::Provider *prov = lookupProvider(Service::DETECT_ENTITIES);
	if (!prov) {
		LOG_ERR(("detectEntities : cannot get service provider info [%s:%d]", __FILE__, __LINE__));
		return false;
	}

	SOCKET sock = prov->sock();
	if (sock < 0) {
		LOG_ERR(("detectEntities : cannot connect to service provider [%s:%d]", __FILE__, __LINE__));
		return false;
	}

	CommRequestDetectEntitiesEncoder enc(myname(), id);

#if 1
	// fix(sekikawa)(FIX20100826)
	//
	//
	//
	if (enc.send(sock) < 0) {
		prov->close();
		return false;
	}
#else
	// orig
	enc.send(sock);
#endif

	CommDataDecoder d;

	typedef CTReader Reader;
	Reader r(sock, d, 8192);

#if 1
	// retry version (added by sekikawa)(2009/03/13)
	bool ret = false;

	int retry = 5;
	int timeout = 10000;

	while (retry > 0) {

#if 1
// sekikawa(FIX20100906)
		Result *result = NULL;
		try
		{
	  		result = r.readSync();
		}
		catch(CTReader::ConnectionClosedException &e)
		{
			break;
		}
#else
// orig
	  Result *result = r.readSync();
#endif

	  if (result) {
		if (result->type() == COMM_RESULT_DETECT_ENTITIES) {
		  ResultDetectEntitiesEvent *evt = (ResultDetectEntitiesEvent*) result->data();
		  evt->copy(v);
		  ret = true;
		}
		delete result;
		break;
	  } else {
		retry--;

#ifdef WIN32
		Sleep(timeout);
#else
		usleep(timeout);
#endif
		timeout *= 2;
	  }
	}
#else
	// original version
	Result *result = r.readSync();
	bool ret = false;

	if (result) {
		if (result->type() == COMM_RESULT_DETECT_ENTITIES) {
			ResultDetectEntitiesEvent *evt = (ResultDetectEntitiesEvent*) result->data();
			evt->copy(v);
			ret = true;
		}
		delete result;
	}
#endif

	prov->close();

	return ret;
}

Text * CTSimObj::getText(RawSound &rawSound)
{
	NSLookup::Provider *prov = lookupProvider(Service::SOUND_RECOG);
	if (!prov) {
		LOG_ERR(("getText : cannot get service provider info [%s:%d]", __FILE__, __LINE__));
		return false;
	}

	SOCKET sock = prov->sock();
	if (sock < 0) {
		LOG_ERR(("getText : cannot connect to service provider [%s:%d]", __FILE__, __LINE__));
		return false;
	}

	{
		CommRequestSoundRecogEncoder *enc = new CommRequestSoundRecogEncoder(rawSound);
#if 1
		// fix(sekikawa)(FIX20100826)
		int status = enc->send(sock);
		delete enc;
		if (status < 0)
		{
			prov->close();
			return false;
		}
#else
		// orig
		enc->send(sock);
		delete enc;
#endif
	}

	Text *text = 0;
	CommDataDecoder d;
	typedef CTReader Reader;
	int retry = 10;
	Reader *r = new Reader(sock, d, 1024);
	while  (true) {

#if 1
// sekikawa(FIX20100906)
		Result *result = NULL;
		try
		{
	  		result = r->readSync();
		}
		catch(CTReader::ConnectionClosedException &e)
		{
			break;
		}
#else
// orig
	  Result *result = r->readSync();
#endif

		if (!result) {
			if (retry <= 0) {
				break;
			} else {
				LOG_DEBUG1(("retry"));
				retry--;
#ifdef WIN32
				Sleep(100000);
#else
				usleep(100000);
#endif
				continue;
			}
		}
		if (result->type() == COMM_RESULT_SOUND_RECOG) {

			ResultSoundRecogEvent *evt = (ResultSoundRecogEvent*)result->data();
			text = evt->releaseText();
		}
		delete result;
		break;
	}

	prov->close();
	delete r;

	return text;
}

//add id argument by okamoto@tome(2011/8/26)
ViewImage * CTSimObj::captureView(ColorBitType cbtype, ImageDataSize size, int id)
{
	NSLookup::Provider *prov = lookupProvider(Service::CAPTURE_VIEW);
	if (!prov) {
		LOG_ERR(("captureView : cannot get service provider info [%s:%d]", __FILE__, __LINE__));
		return false;
	}

	SOCKET sock = prov->sock();
	if (sock < 0) {
		LOG_ERR(("captureView : cannot connect to service provider [%s:%d]", __FILE__, __LINE__));
		return false;
	}

	ViewImageInfo info(IMAGE_DATA_TYPE_ANY, cbtype, size);
	{
	  CommRequestCaptureViewImageEncoder *enc = new CommRequestCaptureViewImageEncoder(myname(), info, id);
#if 1
		// fix(sekikawa)(FIX20100826)
		int status = enc->send(sock);
		delete enc;
		if (status < 0)
		{
			prov->close();
			return false;
		}
#else
		// orig
		enc->send(sock);
		delete enc;
#endif
	}

	ViewImage *img = 0;
	CommDataDecoder d;

	typedef CTReader Reader;

// modify(sekikawa)(2010/08/10)
//	int retry = 10;
	int retry = 500;

	Reader *r = new Reader(sock, d, 100000);

	while  (true) {

#if 1
// sekikawa(FIX20100906)
		Result *result = NULL;
		try
		{
	  		result = r->readSync();
		}
		catch(CTReader::ConnectionClosedException &e)
		{
			break;
		}
#else
// orig
	  Result *result = r->readSync();
#endif

		if (!result) {
			if (retry <= 0) {
				// add(sekikawa)(2010/08/10)
				LOG_ERR(("readSync() failed. max retry count exceeded. [%s:%d]", __FILE__, __LINE__));

				break;
			} else {
				LOG_DEBUG1(("retrying readSync() ... [retry=%d]", retry));
				retry--;
#ifdef WIN32
				Sleep(100000);
#else
				usleep(100000);
#endif
				continue;
			}
		}

		LOG_DEBUG1(("readSync() succeeded"));
		if (result->type() == COMM_RESULT_CAPTURE_VIEW_IMAGE) {

			ResultCaptureViewImageEvent *evt = (ResultCaptureViewImageEvent*)result->data();
			img = evt->release();
		}
		delete result;
		break;
	}

	prov->close();
	delete r;

	return img;
}

//added by okamoto@tome(2011/9/8)
unsigned char CTSimObj::distanceSensor(double start, double end, int id)
{
  NSLookup::Provider *prov = lookupProvider(Service::DISTANCE_SENSOR);
  
  if (!prov) {
    LOG_ERR(("distanceSensor : cannot get service provider info [%s:%d]", __FILE__, __LINE__));
    return false;
  }
  
  SOCKET sock = prov->sock();
  if (sock < 0) {
    LOG_ERR(("distanceSensor : cannot connect to service provider [%s:%d]", __FILE__, __LINE__));
    return false;
  }
  
  CommRequestDistanceSensorEncoder *enc = new CommRequestDistanceSensorEncoder(myname(), start, end, id);

  int status = enc->send(sock);
  delete enc;

  unsigned char distance;
  CommDataDecoder d;
  typedef CTReader Reader;
  int retry = 15;
  Reader *r = new Reader(sock, d, 1024);

  while  (true) {
    Result *result = NULL;
    try
      {
	result = r->readSync();
      }
    catch(CTReader::ConnectionClosedException &e)
      {
	break;
      }
    
    if (!result) {
      if (retry <= 0) {
	// add(sekikawa)(2010/08/10)
	LOG_ERR(("readSync() failed. max retry count exceeded. [%s:%d]", __FILE__, __LINE__));
	
	break;
      } else {
	LOG_DEBUG1(("retrying readSync() ... [retry=%d]", retry));
	retry--;
#ifdef WIN32
	Sleep(100000);
#else
	usleep(100000);
#endif
	continue;
      }
    }
    if (result->type() == COMM_RESULT_DISTANCE_SENSOR) {
      
      ResultDistanceSensorEvent *evt = (ResultDistanceSensorEvent*)result->data();
      distance = evt->getDistance();
    }
    delete result;
    break;
  }

  prov->close();
  return distance;
  
}

ViewImage *CTSimObj::distanceSensorD(double start, double end, int id, bool map)
{

  NSLookup::Provider *prov = lookupProvider(Service::DISTANCE_SENSOR);
  
  if (!prov) {
    LOG_ERR(("distanceSensor1D : cannot get service provider info [%s:%d]", __FILE__, __LINE__));
    return false;
  }
  
  SOCKET sock = prov->sock();
  if (sock < 0) {
    LOG_ERR(("distanceSensor1D : cannot connect to service provider [%s:%d]", __FILE__, __LINE__));
    return false;
  }
  CommRequestDistanceSensorEncoder *enc; 

  if(map == false)
    {
      enc = new CommRequestDistanceSensorEncoder(myname(), start, end, id, 1);
    }
  else
    {
      enc = new CommRequestDistanceSensorEncoder(myname(), start, end, id, 2);
    }
  //	CommRequestDistanceSensorEncoder enc(myname(), id);
  int status = enc->send(sock);

  delete enc;

  ViewImageInfo info(IMAGE_DATA_TYPE_ANY, DEPTHBIT_8, IMAGE_320X1);
  ViewImage *img;
  CommDataDecoder d;
  typedef CTReader Reader;
  int retry;
  if(map == true)
    {
      retry = 25;
    }
  else
    {
      retry = 20;
    }
  Reader *r = new Reader(sock, d, 100000);

  while  (true) {
    Result *result = NULL;
    try
      {
	result = r->readSync();
      }
    catch(CTReader::ConnectionClosedException &e)
      {
	break;
      }
    
    if (!result) {
      if (retry <= 0) {
	// add(sekikawa)(2010/08/10)
	LOG_ERR(("readSync() failed. max retry count exceeded. [%s:%d]", __FILE__, __LINE__));
	
	break;
      } else {
	LOG_DEBUG1(("retrying readSync() ... [retry=%d]", retry));
	retry--;
#ifdef WIN32
	Sleep(100000);
#else
	usleep(100000);
#endif
	continue;
      }
    }

    if (result->type() == COMM_RESULT_CAPTURE_VIEW_IMAGE) {
      
      ResultCaptureViewImageEvent *evt = (ResultCaptureViewImageEvent*)result->data();
      img = evt->release();
    }
    /*
    if (result->type() == COMM_RESULT_DISTANCE_SENSOR1D) {
      
      ResultDistanceSensor1DEvent *evt = (ResultDistanceSensor1DEvent*)result->data();
      distance = evt->getDistance();
    }
    */
    delete result;
    break;
  }

  prov->close();
  return img;
}


void CTSimObj::sendText(double t, const char *to, const char *text)
{
        sendText(t, to, text, -1);
}

void CTSimObj::sendText(double t, const char *to, const char *text, double reachRadius) {
	CommInvokeMethodOnRecvTextEncoder enc(t, myname(), to, text, ENCODE_ASCII, reachRadius);

	enc.send(m_sock);
}

void CTSimObj::sendMessage(const char *to, int argc, char**argv)
{
	MessageSender s(myname(), m_sock);
	s.sendto(to, argc, argv);
}

void CTSimObj::broadcastMessage(int argc, char**argv)
{
	sendMessage(NULL, argc, argv);
}

// begin(FIX20110401)
void CTSimObj::sendSound(double t, const char *to, RawSound &sound)
{
	SoundSender s(myname(), m_sock);
	s.sendto(to, sound);
}
// end(FIX20110401)

static Result *getResult(SOCKET sock, int bufsize, int retry)
{
	CommDataDecoder d;
	typedef CTReader Reader;
	Reader *r = new Reader(sock, d, bufsize);
	while  (true) {
		Result *result = r->readSync();
		if (!result) {
			if (retry <= 0) {
				break;
			} else {
				LOG_DEBUG1(("retry"));
				retry--;
#ifdef WIN32
				Sleep(100000);
#else
				usleep(100000);
#endif
				continue;
			}
		}
		return result;
	}
	return NULL;

}

bool CTSimObj::getAllObjectNames(EntityNameC &v)
{
	Result *result = getResult(m_sock, 8192, 10);
	if (!result) { return false; }
	if (result->type() != COMM_RESULT_GET_OBJECT_NAMES) { return false; }
	ResultGetObjectNamesEvent *evt = (ResultGetObjectNamesEvent *)result->data();
	evt->copy(v);
	return true;
}

bool CTSimObj::getAllEntities(EntityNameC &v)
{
	CommRequestGetEntityNamesEncoder enc;
	enc.send(m_sock);

	return getAllObjectNames(v);
}

