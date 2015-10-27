/*
 * Written by Atsushi Okamoto on 2011-03-31
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-16
 */
#ifndef CommUtil_h
#define CommUtil_h

#include <sigverse/commonlib/systemdef.h>
#include <stdio.h>

class CommUtil
{
public:
	// sekikawa(FIX20100826)
	//static SOCKET connectServer(const char *hostname, int port);
	static SOCKET connectServer(const char *hostname, int port, int retry=0);

	static void disconnectServer(SOCKET sock);

	static int sendData(SOCKET sock, const char *data, int bytes);

	// Opens a file for a time log
	void openTimeDebugLogFp(const char* fname);

	// Gets a pointer of a time log file
	FILE *getTimeDebugLogFp();

	// Closes a file for a time log
	void closeTimeDebugLogFp();

private:
	// File for a time log
	FILE *tdebug;
};

#endif // CommUtil_h


