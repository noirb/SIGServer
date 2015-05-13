/*
 * Modified by Tetsunari Inamura on 2014-02-25
 */

#include "FilenameDB.h"
#include "fname.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

static bool fileExist(const char *fname)
{
	struct stat buf;
	int ret = stat(fname, &buf);
	if (ret != 0) { return false; }
	return !S_ISDIR(buf.st_mode)? true: false;
}


static bool dirExist(const char *dirname)
{
	struct stat buf;
	int ret = stat(dirname, &buf);
	if (ret != 0) { return false; }
	return S_ISDIR(buf.st_mode)? true: false;
}

bool FilenameDB::pushDirectory(const char *dir)
{
	char buf[1024];
	strcpy(buf, dir);
	char *dname = fname_as_dir(buf);
	if (!dirExist(dname)) { return false; }
	m_dirs.push_back(dname);
	return true;
}

const char * FilenameDB::getPath(const char *fname, char *buf)
{
	std::vector<std::string>::iterator i;

	for (i=m_dirs.begin(); i!=m_dirs.end(); i++) {

		std::string dir = *i;
		std::string fpath = dir + fname;

		if (fileExist(fpath.c_str())) {
			strcpy(buf, fpath.c_str());
			return buf;
		}
	}
	return NULL;
}
