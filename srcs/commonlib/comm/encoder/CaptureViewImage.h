/*
 * Modified by okamoto on 2011-08-26
 */

#ifndef CaptureViewImage_h
#define CaptureViewImage_h

#include "systemdef.h"
#include "Encoder.h"
#include "ViewImageInfo.h"
#include "ViewImage.h"


#include <string>

class ViewImage;

namespace CommData {

class CaptureViewImageRequest : public Encoder
{
	enum {
		THIS_DATASIZE = sizeof(unsigned short)*4 + 128,  //TODO: Magic number
	};
	enum { BUFSIZE  =
	       COMM_DATA_HEADER_MAX_SIZE + COMM_DATA_FOOTER_SIZE + THIS_DATASIZE, };
private:
	std::string   m_agentName;
	ViewImageInfo m_info;
	int           m_id;
public:
 CaptureViewImageRequest(const char *agentName, const ViewImageInfo &info, int id)
		: Encoder(COMM_REQUEST_CAPTURE_VIEW_IMAGE, BUFSIZE),
		  m_agentName(agentName), m_info(info), m_id(id)
	{
	}
	
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

class CaptureViewImageResult : public RawDataEncoder
{
	enum { DATA_HEADER_SIZE = 12, }; //TODO: Magic number
private:
	ViewImage &m_img;
	int   m_packetNum;
	char  m_dataHeaderBuf[DATA_HEADER_SIZE];
public:
	CaptureViewImageResult(ViewImage &img);
#ifdef DEPRECATED
	// deprecated 090303 yoshi
	CaptureViewImageResult(const char *agentName, ViewImage &img);
#endif

private:
	char * getDataHeader(int &);
	int    getDataLen();
	char * getData();
};

} // namespace 

typedef CommData::CaptureViewImageRequest CommRequestCaptureViewImageEncoder;
typedef CommData::CaptureViewImageResult CommResultCaptureViewImageEncoder;

#endif // CaptureViewImage_h
 

