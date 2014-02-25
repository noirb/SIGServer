/* $Id: ResultGetEntity.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef CommEvent_ResultGetEntity_h
#define CommEvent_ResultGetEntity_h

class SimObj;


class ResultGetEntityEvent
{
private:
	double m_time;
	SimObj *m_obj;
public:
	bool 	set(int packetNum, int seq, char *data, int n);
private:
	void	free_();
public:
	ResultGetEntityEvent() :  m_time(-1), m_obj(0) {;}
	~ResultGetEntityEvent() { free_(); }
	double time() { return m_time; }
	SimObj *release()
	{
		SimObj *tmp = m_obj;
		m_obj = 0;
		return tmp;
	}
};

#endif // CommEvent_ResultGetEntity_h
 

