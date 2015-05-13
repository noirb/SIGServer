/* $Id: ResultCaptureViewImage.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef CommEvent_ResultCaptureViewImage_h
#define CommEvent_ResultCaptureViewImage_h

class ViewImage;
class ViewImageInfo;

class ResultCaptureViewImageEvent
{
private:
	ViewImage *m_img;
	ViewImage *m_imgtmp;
	char      *m_curr;
	int       m_prevSeq;
private:
	void free_();
public:
	ResultCaptureViewImageEvent() : m_img(0), m_imgtmp(0), m_curr(0), m_prevSeq(-1) {;}
	~ResultCaptureViewImageEvent() { free_(); }
	bool set(int packetNum, int seq, char *data, int n);

	ViewImage *release()
	{
		ViewImage *tmp = m_img;
		m_img = 0;
		return tmp;
	}
};


#endif // CommEvent_ResultCaptureViewImage_h
 

