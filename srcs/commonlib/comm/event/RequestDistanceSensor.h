/* $Id: RequestDistanceSensor.h,v 1.4 2011-09-22 07:02:57 okamoto Exp $ */ 
#ifndef CommEvent_RequestDistanceSensor_h
#define CommEvent_RequestDistanceSensor_h

#include <string>

//class ViewImageInfo;


class RequestDistanceSensorEvent
{
private:
	std::string	m_agentName;
	double      m_start;
	double        m_end;
	int            m_id;
	int           m_dim; 

public:
	bool set(int packetNum, int seq, char *data, int n);

	const char *getAgentName() { return m_agentName.c_str(); }
	double getStart() { return m_start; }	
	double getEnd() { return m_end; }	
	int getId() { return m_id; }	
	int getDimension() { return m_dim; }	

};


#endif // CommEvent_RequestDistanceSensor_h
 

