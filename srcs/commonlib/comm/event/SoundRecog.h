/* $Id: SoundRecog.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef SoundRecog_h
#define SoundRecog_h

#include <stdio.h>

class RawSound;


class RequestSoundRecogEvent
{
private:
	RawSound *m_sound;
	RawSound *m_soundTmp;
	int		m_prevSeq;
	char *		m_curr;
public:
	RequestSoundRecogEvent()
		: m_sound(NULL), m_soundTmp(NULL),  m_prevSeq(-1), m_curr(NULL) {}
	~RequestSoundRecogEvent();
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	RawSound *getRawSound() { return m_sound; }
	RawSound *releaseRawSound() {
		RawSound *tmp = m_sound;
		m_sound = NULL;
		return tmp;
	}
};

#include "Text.h"

class ResultSoundRecogEvent
{
private:
	Text *m_text;
public:
	ResultSoundRecogEvent() : m_text(NULL) {}
	
	~ResultSoundRecogEvent() {
		if (m_text) { delete m_text; m_text = NULL; }
	}
public:
	bool	set(int packetNum, int seq, char *data, int n);
public:
	Text * getText() { return m_text; }
	
	Text * releaseText() {
		Text *tmp = m_text;
		m_text = NULL;
		return tmp;
	}
};


#endif // SoundRecog_h
 

