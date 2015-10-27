/* $Id: ResultDistanceSensor.h,v 1.6 2011-09-16 05:01:56 okamoto Exp $ */ 
#ifndef CommEvent_ResultDistanceSensor_h
#define CommEvent_ResultDistanceSensor_h


class ResultDistanceSensorEvent
{
private:
	unsigned char m_distance;
	void free_();

public:
	ResultDistanceSensorEvent() : m_distance(0) {;}
	~ResultDistanceSensorEvent() {free_();}
	bool set(int packetNum, int seq, char *data, int n);

	unsigned char getDistance(){
		unsigned char distance = m_distance;
		m_distance = 0;
		return distance;
	}
};


#endif // CommEvent_ResultCaptureViewImage_h
 

