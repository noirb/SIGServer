#ifndef DistanceSensor_h
#define DistanceSensor_h

#include "Encoder.h"
#include "Logger.h"


#include <string>


namespace CommData {
 
	class DistanceSensorRequest : public Encoder
	{
	private:
		enum{ BUFSIZE=128,};

	private:
		std::string m_agentName;
		double      m_start;
		double      m_end;
		int         m_id;
		int         m_dim;
	public:

	DistanceSensorRequest(const char *agentName, double start, double end, int id, int dim = 0)
		: Encoder(COMM_REQUEST_DISTANCE_SENSOR, BUFSIZE),  m_agentName(agentName), m_start(start), m_end(end), m_id(id)
		{ m_dim = dim ;}

		int packetNum() { return 1; }
		char *encode(int seq, int &);
	};

	class DistanceSensorResult : public Encoder
	{
	private:
		enum { BUFSIZE = 128, };
		unsigned char m_distance;
		
		
	public:
	DistanceSensorResult(unsigned char distance)
		: Encoder(COMM_RESULT_DISTANCE_SENSOR, BUFSIZE), m_distance(distance)
		{;}
		
		int packetNum() { return 1; }
		char * encode(int seq, int &);
		
	private:
		unsigned char getDistance(){ return m_distance;}
	};
}
typedef CommData::DistanceSensorRequest CommRequestDistanceSensorEncoder;
typedef CommData::DistanceSensorResult  CommResultDistanceSensorEncoder;

#endif // DistanceSensor_h

