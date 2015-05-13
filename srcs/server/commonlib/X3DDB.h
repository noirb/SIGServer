/*
 * Modified by okamoto on 2011-03-31
 * Added comments by Tetsunari Inamura on 2014-02-28
 */

#ifndef X3DDB_h
#define X3DDB_h

#include <string>
#include <map>
#include <vector>


/**
 * @brief Class for file database of X3D
 *
 * Manage parts of agents and X3D files for each status
 * Key string is:
 * <pre><i>
 *    <Agent Name>:<Parts Name>:<Status Name>:<Status Value>
 * </i></pre>
 * Parts name, status name and status value can be omitted as key
 */
class X3DDB
{
public:
	//! Class for X3DDB entry iterator
	class Iterator {
	private:
		std::map<std::string, std::string> &m_m;
		std::map<std::string, std::string>::iterator m_itr;
	public:
		Iterator(std::map<std::string, std::string> &m) : m_m(m)
		{
			m_itr = m_m.end();
		}
		//! Refer the next key string
		const char *nextKey()
		{
			if (m_itr == m_m.end()) {
				m_itr = m_m.begin();
			} else {
				m_itr++;
			}
			return m_itr != m_m.end()? m_itr->first.c_str(): NULL;
		}
	};
private:
	std::map<std::string, std::string> m_m;
public:
	//! Refer the number of entries
	int size() { return m_m.size(); }
	//! Clear the stored database
	void clear() { m_m.clear(); }

	//! Refer iterator for entries
	Iterator *getIterator() { return new Iterator(m_m); }

	//! Set key and value
	void set(const char *key, const char *value)
	{
		m_m[std::string(key)] = std::string(value);
	}

	//! Set specified key value
	const char *get(const char *key)
	{
		std::map<std::string, std::string>::iterator i = m_m.find(std::string(key));
		return i != m_m.end()? i->second.c_str(): NULL;
	}

	//! Refer value for specified key
	std::string getS(std::string key)
	{
		std::map<std::string, std::string>::iterator i = m_m.find(key);
		return i != m_m.end()? i->second: NULL;
	}

	//! Refer shape file held by the specified class name
	std::vector<std::string>  getShapeFromClass(std::string cl)
	{
		std::vector<std::string> shapes;
		std::map<std::string, std::string>::iterator i = m_m.begin();

		while(i != m_m.end()) {
			std::string c = i->first; 
			std::string::size_type index = c.find(cl.c_str());

			if(index != std::string::npos) { 
				//if(strstr(c.c_str(), cl.c_str()) != NULL) {
				shapes.push_back(i->second);
			}
			i++;
		}
		return shapes; 
	}
	
	//! Refer the size of data transfer for shape file
	int getAllShapeFileDataSize()
	{
		int size = 0;
		std::map<std::string, std::string>::iterator i = m_m.begin();

		while(i != m_m.end()){
			size += (*i).second.size();
			i++;
		}
		return size;
	}
};

#endif // X3DDB_h
 

