/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Comm_Event_Attrs_h
#define Comm_Event_Attrs_h

#include <string>
#include <vector>

class RequestGetAttributesEvent
{
private:
	std::vector<std::string> m_attrNames;
public:
	bool set(int packetNum, int seq, char *data, int n);

	int getAttrNum() {
		return m_attrNames.size();
	}
	const char * getAttrName(int i) {
		return m_attrNames[i].c_str();
	}
};

#endif // Comm_Event_Attrs_h
 

