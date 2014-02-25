/*
 * Modified by Okamoto on 2011-03-25
 */

#ifndef FilenameDB_h
#define FilenameDB_h

#include <string>
#include <vector>

class FilenameDB
{
	typedef std::string S;
	typedef std::vector<S> C;
private:
	C	m_dirs;
public:
	bool 		pushDirectory(const char *dir);
	const char * 	getPath(const char *fname, char *buf);

	int		getSize() { return m_dirs.size(); }
	const char *	getDirectory(int i) { return m_dirs[i].c_str(); }
};

#endif // FilenameDB_h
 

