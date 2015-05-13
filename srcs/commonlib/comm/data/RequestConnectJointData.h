/* $Id: RequestConnectJointData.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef RequestConnectJointData_h
#define RequestConnectJointData_h

#include <string>

class RequestConnectJointData
{
public:
	std::string jointName;
	std::string agentName;
	std::string agentParts;
	std::string targetName;
	std::string targetParts;

public:
	RequestConnectJointData() {}
	RequestConnectJointData(const char *jointName_, const char *agentName_,const char *agentParts_, const char *targetName_, const char *targetParts_)
	{
		if (jointName_) {
			jointName = jointName_;
		}
		if (agentName_) {
			agentName = agentName_;
		}
		if (agentParts_) {
			agentParts = agentParts_;
		}
		if (targetName_) {
			targetName = targetName_;
		}
		if (targetParts_) {
			targetParts = targetParts_;
		}
	}
};


#endif // RequestConnectJointData_h
 
