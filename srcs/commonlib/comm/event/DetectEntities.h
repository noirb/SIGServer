/* $Id: DetectEntities.h,v 1.2 2011-08-15 05:34:46 okamoto Exp $ */ 
#ifndef Comm_Event_DetectEntities_h
#define Comm_Event_DetectEntities_h

#include <string>
#include <vector>

class RequestDetectEntitiesEvent
{
private:
	typedef std::string S;
private:
	S	m_agentName;
	int            m_id;
public:
	bool	set(int packetNum, int seq, char *data, int n);

	const char *getAgentName() { return m_agentName.c_str(); }
	int getId() { return m_id; }
};

class ResultDetectEntitiesEvent
{
private:
	typedef std::string S;
	typedef std::vector<S> C;
private:
	C	m_detected;
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	int	getDetectedCount() { return m_detected.size(); }
	const char *getDetected(int i)
	{
		if (i < 0 || i >= getDetectedCount()) { return 0; }
		return m_detected[i].c_str();
	}
	void	copy(C &o)
	{
		for (C::iterator i=m_detected.begin(); i!=m_detected.end(); i++) {
			S s = *i;
			o.push_back(s);
		}
	}
	
};

#endif // Comm_Event_DetectEntities_h
 

