/*
 * Created by Okamoto on 2011-03-25
 */

#ifdef DEPRECATED
#include "JointControl.h"
#include "binary.h"

char *JointControlCommand::toBinary(int &n)
{
	static char buf[8192];

	char *p = buf;

	BINARY_SET_DATA_S_INCR(p, CommandType, m_type);
	BINARY_SET_STRING_INCR(p, target());

	unsigned num = m_values.size();
	BINARY_SET_DATA_S_INCR(p, unsigned short, num);
	for (std::map<std::string, double>::iterator i=m_values.begin(); i!=m_values.end(); i++) {
		std::string name = i->first;
		double v = i->second;
		BINARY_SET_STRING_INCR(p, name.c_str());
		BINARY_SET_DOUBLE_INCR(p, v);
	}
	
	n = p - buf;
	return buf;


}

void JointControlCommand::dump()
{
#ifdef _DEBUG
	printf("type : %d\n", m_type);
	printf("target agent name : \"%s\"\n", target());

	for (std::map<std::string, double>::iterator i=m_values.begin(); i!=m_values.end(); i++) {
		std::string name = i->first;
		double v = i->second;
		printf("\t\"%s\" = %f\n", name.c_str(), v);
	}
	printf("\n");
#endif
}

#endif // DEPRECATED
