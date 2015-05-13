/*
 * Written by Okamoto on 2011-03-31
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-29
 *    Modify indent, add English comments
 */
#ifndef RawSound_h
#define RawSound_h

#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

enum {
	RAW_SOUND_LITTLE_ENDIAN = 0,
	RAW_SOUND_BIG_ENDIAN,
};

typedef unsigned short RawSoundEndian;

//! Sound header class
class RawSoundHeader
{
private:
	int             m_channels;
	unsigned        m_samplingRate;  // [Hz]
	unsigned        m_bitsPerSample; // Byte/sec
	RawSoundEndian  m_endian;
public:
	/**
	 * @brief Constructor
	 *
	 * @param channels      channel number (monaural:1, stereo:2)
	 * @param samplingRate  sampling rate
	 * @param bitsPerSample bits per sample
	 * @param endian        data endian
	 */
	RawSoundHeader(int channels, unsigned samplingRate, unsigned bitsPerSample, RawSoundEndian endian) :
		m_channels(channels), m_samplingRate(samplingRate),
		m_bitsPerSample(bitsPerSample), m_endian(endian) {;}

	//! Constructor
	RawSoundHeader() : m_channels(0), m_samplingRate(0), m_bitsPerSample(0), m_endian(0) {}

	//! Gets channel number
	int             getChannelNum() { return m_channels; }
	//! Gets sampling rate
	unsigned        getSamplingRate() { return m_samplingRate; }
	//! Gets bits per sample
	unsigned        getBitPerSample() { return m_bitsPerSample; }
	//! Gets data endian
	RawSoundEndian  getEndian() { return m_endian; }
};

//! Sound data class
class RawSound
{
private:
	RawSoundHeader m_header;
	int            m_datalen;
	char          *m_data;
public:
	/**
	 * @brief Constructor
	 *
	 * @param h       sound data header
	 * @param datalen sound data length
	 *
	 */
	RawSound(RawSoundHeader &h, int datalen)
		: m_header(h), m_datalen(datalen)
	{
#ifdef WIN32
		m_data = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (DWORD)datalen);
#else
		m_data = new char[datalen];
#endif
	}

	//! Constructor
	RawSound() : m_datalen(0), m_data(NULL) {}
	//! Destructor
	~RawSound()	{ freeWaveData(); }

	//! Gets sound data header
	RawSoundHeader &getHeader() { return m_header; }

	//! Gets sound data length
	int getDataLen() { return m_datalen; }
	//! Gets sound data
	char * getData() { return m_data; }

	/**
	 * @brief Loads WAV type file
	 * @param waveFile WAV file name
	 */
	bool loadWaveFile(const char *waveFile);
	/**
	 * @brief Loads WAV type data
	 * @param data    WAV data
	 * @param datalen WAV data length
	 */
	bool loadWaveData(char *data, int datalen);
	//! Plays sound
	bool play();
	//! Release WAV data memory
	void freeWaveData();
};

#endif // RawSound_h


