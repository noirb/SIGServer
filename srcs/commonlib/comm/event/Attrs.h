/* $Id: Attrs.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Event_Attrs_h
#define Comm_Event_Attrs_h

#include <string>
#include <vector>

class RequestGetAttributesEvent
{
private:
	typedef std::string S;
	typedef std::vector<S> C;
private:
	C m_attrNames;
public:
	bool 	set(int packetNum, int seq, char *data, int n);

	int	getAttrNum() {
		return m_attrNames.size();
	}
	const char *	getAttrName(int i) {
		return m_attrNames[i].c_str();
	}
};

#endif // Comm_Event_Attrs_h
 

