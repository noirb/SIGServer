#include "JspWave.h"
#include "JspLog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

namespace Jsp {
	Wave::Wave() {
		m_pWaveData = NULL;
		m_dataLen = 0;
	}

	Wave::~Wave() {
		_free();
	}

	void Wave::_free() {
		if (m_pWaveData) {
			free(m_pWaveData);
			m_pWaveData = NULL;
		}

		m_dataLen = 0;
	}

	bool Wave::loadFromFile(const char *waveFile) {
		_free();

		FILE *fp = fopen(waveFile, "r");
		if (!fp) {
			Jsp::printLog(LOG_FATAL, "failed to open file (%s)\n", waveFile);
			return false;
		}

		struct stat st;
		int ret = stat(waveFile, &st);
		if (ret != 0) {
			Jsp::printLog(LOG_FATAL, "failed to get file size\n");
			return false;
		}

		int fileSize = st.st_size;
		printf("size : %d bytes\n", fileSize);

		if (fileSize > 0) {
			char *buf = (char *)malloc(sizeof(char)*fileSize);
			if (!buf) {
				Jsp::printLog(LOG_FATAL, "out of memory\n");
				return false;
			}

			int readBytes = fread(buf, sizeof(char), fileSize, fp);
			if (readBytes == fileSize) {
				Jsp::printLog(LOG_INFO, "successfully loaded (%s)\n", waveFile);

				m_pWaveData = buf;
				m_dataLen = fileSize;
			}
		}

		return true;
	}
};
