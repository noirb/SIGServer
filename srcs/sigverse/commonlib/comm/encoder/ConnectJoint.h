/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Comm_Encoder_ConnectJoint_h
#define Comm_Encoder_ConnectJoint_h

#include <sigverse/commonlib/comm/encoder/Encoder.h>
#include <sigverse/commonlib/comm/data/RequestConnectJointData.h>

namespace CommData {

class RequestConnectJoint : public Encoder
{
	enum { BUFSIZE = 512, }; //TODO: Magic number
private:
	RequestConnectJointData m_data;
public:
	RequestConnectJoint(
		const char *jointName, const char *agentName,const char *agentParts,
		const char *targetName, const char *targetParts) :
		Encoder(COMM_REQUEST_CONNECT_JOINT, BUFSIZE),
		m_data(jointName, agentName, agentParts, targetName, targetParts
	) {}

private:
	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace

typedef CommData::RequestConnectJoint CommRequestConnectJointEncoder;

#endif // Comm_Encoder_ConnectJoint_h
 

