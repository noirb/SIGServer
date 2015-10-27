/*
 * Created by okamoto on 2011-03-25
 */

#include <sigverse/commonlib/comm/event/GetJointForce.h>
#include <sigverse/commonlib/binary.h>

bool RequestGetJointForceEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	char *agentName = BINARY_GET_STRING_INCR(p);
	if (agentName) {
		m_agentName = agentName;
	}
	BINARY_FREE_STRING(agentName);

	char *jointName = BINARY_GET_STRING_INCR(p);
	if (jointName) {
		m_jointName = jointName;
	}
	BINARY_FREE_STRING(jointName);
	return true;
}

bool ResultGetJointForceEvent::set(int packetNum, int seq, char *data, int n)
{
	char *p = data;

	unsigned short nn = BINARY_GET_DATA_S_INCR(p, unsigned short);
	bool b = (nn != 0)? true: false;
	m_data.success(b);

	if (b) {
		JointForce *jfs = m_data.getJointForces();

		for (int i=0; i<ResultGetJointForceData::JOINT_FORCE_NUM; i++) {
			char *partsName = BINARY_GET_STRING_INCR(p);
			Vector3d f;
			{
				double x = BINARY_GET_DOUBLE_INCR(p);
				double y = BINARY_GET_DOUBLE_INCR(p);
				double z = BINARY_GET_DOUBLE_INCR(p);
				f.set(x, y, z);
			}
			Vector3d tq;
			{
				double x = BINARY_GET_DOUBLE_INCR(p);
				double y = BINARY_GET_DOUBLE_INCR(p);
				double z = BINARY_GET_DOUBLE_INCR(p);
				tq.set(x, y, z);
			}
			JointForce *jf = &jfs[i];
			jf->set(partsName, f, tq);
			BINARY_FREE_STRING(partsName);
		}
	}
	return true;
}
