/*
 * Created  by okamoto on 2011-03-25
 * Modified by okamoto on 2011-08-15
 */

#ifndef Comm_Encoder_DetectEntities_h
#define Comm_Encoder_DetectEntities_h

#include "Encoder.h"

#include <string>
#include <vector>

namespace CommData {

class DetectEntitiesRequest : public Encoder
{
private:
	enum { BUFSIZE = 256, }; //TODO: Magic number
private:
	std::string	m_name;
	int               m_id;
public:
 DetectEntitiesRequest(const char *agentName, int id)
   : Encoder(COMM_REQUEST_DETECT_ENTITIES, BUFSIZE), m_name(agentName), m_id(id)
	{
	}

	int 	packetNum() { return 1; }
	char *	encode(int seq, int &);
};


class DetectEntitiesResult : public Encoder
{
private:
	enum { BUFSIZE = 1024, }; //TODO: Magic number
private:
	typedef std::string S;
	typedef std::vector<S> C;
private:
	C		m_detected;
public:
#ifdef DEPRECATED
	DetectEntitiesResult(const char *controllerName)
		: Encoder(COMM_RESULT_DETECT_ENTITIES, BUFSIZE) 
	{
		// setForwardTo(controllerName, false);
	}
#endif
	DetectEntitiesResult()
		: Encoder(COMM_RESULT_DETECT_ENTITIES, BUFSIZE) {}

	void	pushDetectedEntity(const char *name)
	{
		m_detected.push_back(name);
	}
	int	packetNum() { return 1; }
	char *	encode(int seq, int &);
};
	
} // namespace 

typedef CommData::DetectEntitiesRequest CommRequestDetectEntitiesEncoder;
typedef CommData::DetectEntitiesResult CommResultDetectEntitiesEncoder;

#endif // Comm_Encoder_DetectEntities_h
 

