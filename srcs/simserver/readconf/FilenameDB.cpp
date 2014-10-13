/*
 * Modified by Tetsunari Inamura on 2014-02-25
 */

#include "FilenameDB.h"
#include "fname.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#ifdef WIN32
#define MSG_DONTWAIT 0

#endif

static bool fileExist(const char *fname)
{
	struct stat buf;
	int ret = stat(fname, &buf);
	if (ret != 0) { return false; }
#ifndef WIN32
	return !S_ISDIR(buf.st_mode)? true: false;
#else
	return !(buf.st_mode & _S_IFDIR)? true: false;
#endif
}


static bool dirExist(const char *dirname)
{
	struct stat buf;
	int ret = stat(dirname, &buf);
	if (ret != 0) { return false; }
#ifndef WIN32
	return S_ISDIR(buf.st_mode)? true: false;
#else
	return (buf.st_mode & _S_IFDIR)? true: false;
#endif
}

bool FilenameDB::pushDirectory(const char *dir)
{
	char buf[1024];
	strcpy(buf, dir);
	char *dname = fname_as_dir(buf);

#ifndef WIN32
	if (!dirExist(dname)) { return false; }
#else
	if (!dirExist(dir)) { return false; }
#endif
	m_dirs.push_back(dname);
	return true;
}

const char * FilenameDB::getPath(const char *fname, char *buf)
{
	C::iterator i;
	for (i=m_dirs.begin(); i!=m_dirs.end(); i++) {
		S dir = *i;
		S fpath = dir + fname;
		if (fileExist(fpath.c_str())) {
			strcpy(buf, fpath.c_str());
			return buf;
		}
	}
	return NULL;
}
