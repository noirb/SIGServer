/*
 * Modified indent by Yoshiaki Mizuchi on 2014-01-29
 */
#include "RawSound.h"

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

#pragma comment(lib,"winmm.lib")

#ifdef SIGVERSE_OGRE_CLIENT
#include "SgvLog.h"
#endif  // SIGVERSE_OGRE_CLIENT

#ifdef IRWAS_OGRE_CLIENT
#include "IrcApp.h"
#endif  // IRWAS_OGRE_CLIENT

#else   // WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#endif // WIN32

bool RawSound::loadWaveFile(const char *waveFile)
{
#ifdef WIN32
	if (!waveFile) return false;

#ifdef IRWAS_OGRE_CLIENT
	IrcApp *app = getApp();
#endif  // IRWAS_OGRE_CLIENT

	// free memory if previous wave data exists
	freeWaveData();

	HANDLE fh;
	DWORD dwReadSize;
	static LPCSTR lpSound;

	// open file
	fh=CreateFile(
		waveFile,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (fh==INVALID_HANDLE_VALUE)
	{
#ifdef SIGVERSE_OGRE_CLIENT
		Sgv::Log::println("** ERROR ** cannot open wave file : %s [%s:%d]", waveFile, __FILE__, __LINE__);
#else
#ifdef IRWAS_OGRE_CLIENT
		app->printLog("** ERROR ** cannot open wave file : %s [%s:%d]\n", waveFile, __FILE__, __LINE__);
#endif // IRWAS_OGRE_CLIENT
#endif // SIGVERSE_OGRE_CLIENT
		return false;
    }

	// alloc memory for contents of file
	m_datalen = (int)GetFileSize(fh, NULL);
	m_data = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_datalen);
	if (!m_data)
	{
#ifdef SIGVERSE_OGRE_CLIENT
		Sgv::Log::println("** ERROR ** cannot alloc %d bytes of memory for wave data [%s:%d]", m_datalen, __FILE__, __LINE__);
#else
#ifdef IRWAS_OGRE_CLIENT
		app->printLog("** ERROR ** cannot alloc %d bytes of memory for wave data [%s:%d]\n", m_datalen, __FILE__, __LINE__);
#endif // IRWAS_OGRE_CLIENT
#endif // SIGVERSE_OGRE_CLIENT
		return false;
	}

	// read file
	if (!ReadFile(fh, (LPVOID)m_data, m_datalen, &dwReadSize, NULL))
	{
#ifdef SIGVERSE_OGRE_CLIENT
		Sgv::Log::println("** ERROR ** cannot read wave file [%s:%d]", __FILE__, __LINE__);
#else
#ifdef IRWAS_OGRE_CLIENT
		app->printLog("** ERROR ** cannot read wave file [%s:%d]\n", __FILE__, __LINE__);
#endif // IRWAS_OGRE_CLIENT
#endif // SIGVERSE_OGRE_CLIENT
		return false;
	}

	if ((int)dwReadSize != m_datalen)
	{
#ifdef SIGVERSE_OGRE_CLIENT
		Sgv::Log::println("** ERROR ** file size and read size is different. (fileSize=%d, readSize=%d) [%s:%d]", m_datalen, dwReadSize, __FILE__, __LINE__);
#else
#ifdef IRWAS_OGRE_CLIENT
		app->printLog("** ERROR ** file size and read size is different. (fileSize=%d, readSize=%d) [%s:%d]\n", m_datalen, dwReadSize, __FILE__, __LINE__);
#endif // IRWAS_OGRE_CLIENT
#endif // SIGVERSE_OGRE_CLIENT
		return false;
	}

	// everything is ok. close file handle
	CloseHandle(fh);

	return true;

#else  // WIN32
	if (!waveFile) return false;

	// free memory if previous wave data exists
	freeWaveData();

	FILE *fp = fopen(waveFile, "r");
	if (!fp) {
		fprintf(stderr, "failed to open file (%s)\n", waveFile);
		return false;
	}

	struct stat st;
	int ret = stat(waveFile, &st);
	if (ret != 0) {
		fprintf(stderr, "failed to get file size\n");
		return false;
	}

	int fileSize = st.st_size;
	printf("size : %d bytes\n", fileSize);

	if (fileSize > 0) {
		char *buf = new char[fileSize];
		if (!buf) {
			fprintf(stderr, "out of memory\n");
			return false;
		}

		int readBytes = fread(buf, sizeof(char), fileSize, fp);
		if (readBytes == fileSize) {
			fprintf(stderr, "successfully loaded (%s)\n", waveFile);

			// store
			m_data = buf;
			m_datalen = fileSize;
		}
	}

	return true;
#endif
}

bool RawSound::loadWaveData(char *data, int datalen)
{
#ifdef WIN32
#ifdef IRWAS_OGRE_CLIENT
	IrcApp *app = getApp();
#endif // IRWAS_OGRE_CLIENT

	// free memory if previous wave data exists
	freeWaveData();

	// alloc memory for new wave data
	m_data = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (DWORD)datalen);
	if (!m_data)
	{
#ifdef SIGVERSE_OGRE_CLIENT
		Sgv::Log::println("** ERROR ** cannot alloc memory for wave data [%s:%d]", __FILE__, __LINE__);
#else
#ifdef IRWAS_OGRE_CLIENT
		app->printLog("** ERROR ** cannot alloc memory for wave data [%s:%d]\n", __FILE__, __LINE__);
#endif	// IRWAS_OGRE_CLIENT
#endif	// SIGVERSE_OGRE_CLIENT
		return false;
	}

	// copy
	memcpy(m_data, data, datalen);
	m_datalen = datalen;

	return true;
#else  // WIN32
	fprintf(stderr, "*** sorry. not yet implemeted in Linux environment *** (%s:%d)\n", __FILE__, __LINE__);
	return false;
#endif  // WIN32
}

bool RawSound::play()
{
#ifdef WIN32
	if (m_datalen == 0) return false;

	// Win32 sound play API
	return ( PlaySound(m_data, NULL, SND_MEMORY) == TRUE);
#else
	fprintf(stderr, "*** sorry. not yet implemeted in Linux environment *** (%s:%d)\n", __FILE__, __LINE__);
	return false;
#endif
}

void RawSound::freeWaveData()
{
	if (m_data)
	{
		// free data
#ifdef WIN32
		HeapFree(GetProcessHeap(), 0, (LPVOID)m_data);
#else
		delete [] m_data;
#endif
		// clear non-valid pointer
		m_data = NULL;

		// clear data size too
		m_datalen = 0;
	}
}

