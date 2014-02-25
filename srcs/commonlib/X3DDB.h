/* $Id: X3DDB.h,v 1.2 2011-03-31 08:15:58 okamoto Exp $ */ 
#ifndef X3DDB_h
#define X3DDB_h

#include <string>
#include <map>
#include <vector>


class X3DDB
{
private:
	typedef std::string S;
	typedef std::map<S, S> M;
public:
	class Iterator {
	private:
		M &	m_m;
		M::iterator m_itr;
	public:
		Iterator(M &m) : m_m(m)
		{
			m_itr = m_m.end();
		}
		const char *nextKey() {
			if (m_itr == m_m.end()) {
				m_itr = m_m.begin();
			} else {
				m_itr++;
			}
			return m_itr != m_m.end()? m_itr->first.c_str(): NULL;
		}
	};
private:
	M m_m;
public:
	int	size() { return m_m.size(); }
	void	clear() { m_m.clear(); }

	Iterator *getIterator() { return new Iterator(m_m); }

	void	set(const char *key, const char *value)
	{
		m_m[S(key)] = S(value);
	}

	const char *get(const char *key)
	{
		M::iterator i = m_m.find(S(key));
		return i != m_m.end()? i->second.c_str(): NULL;
	}

	std::string getS(std::string key)
	{
		M::iterator i = m_m.find(key);
		return i != m_m.end()? i->second: NULL;
	}

	std::vector<std::string>  getShapeFromClass(std::string cl)
	{
	  std::vector<std::string> shapes;
	  M::iterator i = m_m.begin();
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
	
	int getAllShapeFileDataSize()
	{
	  int size = 0;
	  M::iterator i = m_m.begin();
	  while(i != m_m.end()){
	    size += (*i).second.size();
	    i++;
	  }
	  return size;
	}

};
	


#endif // X3DDB_h
 

