/* $Id: SoundRecog.h,v 1.2 2011-03-31 08:16:28 okamoto Exp $ */ 
#ifndef Comm_Encoder_SoundRecog_h
#define Comm_Encoder_SoundRecog_h

#include "Encoder.h"
#include "Text.h"
#include <string.h>

class RawSound;

namespace CommData {

class RequestSoundRecog : public RawDataEncoder
{
private:
	enum { DATA_HEADER_BUFSIZE = 256, };
private:
	RawSound &m_sound;
	char	m_dataHeaderBuf[DATA_HEADER_BUFSIZE];
public:
	RequestSoundRecog(RawSound &sound) : RawDataEncoder(COMM_REQUEST_SOUND_RECOG), m_sound(sound) {}

private:
	char *	getDataHeader(int &);
	int 	getDataLen();
	char *	getData();
};

class ResultSoundRecog : public Encoder
{
private:
	enum { BUFSIZE = 1024, };
private:
	Text m_data;
public:
	ResultSoundRecog(Encode encode, const char *text)
		: Encoder(COMM_RESULT_SOUND_RECOG, BUFSIZE),
		  m_data(encode, text) {}


	int 	packetNum() { return 1; }
	char *	encode(int seq, int &);
};


} // namespace

typedef CommData::RequestSoundRecog CommRequestSoundRecogEncoder;
typedef CommData::ResultSoundRecog CommResultSoundRecogEncoder;

#endif // Comm_Encoder_SoundRecog_h
 

