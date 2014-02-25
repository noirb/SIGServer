#ifndef Comm_Event_GetPointedObject_h
#define Comm_Event_GetPointedObject_h


#include <string>
#include <vector>
#include <stdlib.h>


class RequestGetPointedObjectEvent
{
private:
	std::string m_speakerName;
	std::string m_SStr1;
	std::string m_SStr2;
	int m_TypicalType;
public:
	bool	set(int packetNum, int seq, char *data, int n);

	const char* getSpeakerName() { return m_speakerName.length()>0? m_speakerName.c_str(): NULL; }
	std::string getPartsName1() { return m_SStr1;}
	std::string getPartsName2() { return m_SStr2;}
	const int getTypicalType() { return m_TypicalType;}


};



class ResultGetPointedObjectEvent
{
private:
	std::vector<std::string>  m_candidateNameVec;
public:
	bool	set(int packetNum, int seq, char *data, int n);
	int copy(std::vector<const char*> &outVector) {
		outVector.clear();
		for(unsigned int n=0; n<m_candidateNameVec.size(); n++){
			outVector.push_back(m_candidateNameVec[n].c_str());
		}
		return (int)outVector.size();
	}
};



#endif // Comm_Event_GetPointedObject_h
 

