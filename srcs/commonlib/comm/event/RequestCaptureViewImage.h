/* $Id: RequestCaptureViewImage.h,v 1.2 2011-08-26 05:24:08 okamoto Exp $ */ 
#ifndef CommEvent_RequestCaptureViewImage_h
#define CommEvent_RequestCaptureViewImage_h

#include <string>

class ViewImageInfo;


class RequestCaptureViewImageEvent
{
private:
	std::string	m_agentName;
	ViewImageInfo *m_info;
	int            m_id;
private:
	void	free_();
public:
	RequestCaptureViewImageEvent() : m_info(0) {;}
	~RequestCaptureViewImageEvent() { free_(); }
	
	bool set(int packetNum, int seq, char *data, int n);

	const char *getAgentName() { return m_agentName.c_str(); }
	const ViewImageInfo & getImageInfo() { return *m_info; }
	int getId() { return m_id; }	
};


#endif // CommEvent_RequestCaptureViewImage_h
 

