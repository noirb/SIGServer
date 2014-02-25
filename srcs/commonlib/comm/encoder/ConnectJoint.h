/* $Id: ConnectJoint.h,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#ifndef Comm_Encoder_ConnectJoint_h
#define Comm_Encoder_ConnectJoint_h

#include "Encoder.h"
#include "comm/data/RequestConnectJointData.h"

namespace CommData {

class RequestConnectJoint : public Encoder
{
	enum { BUFSIZE = 512, };
private:
	RequestConnectJointData m_data;
public:
	RequestConnectJoint(
			    const char *jointName, const char *agentName,const char *agentParts, 
			    const char *targetName, const char *targetParts) :
		Encoder(COMM_REQUEST_CONNECT_JOINT, BUFSIZE),
		m_data(jointName, agentName, agentParts, targetName, targetParts) {}

private:
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace

typedef CommData::RequestConnectJoint CommRequestConnectJointEncoder;

#endif // Comm_Encoder_ConnectJoint_h
 

