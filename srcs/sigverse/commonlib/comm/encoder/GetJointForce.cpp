/* $Id: GetJointForce.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */
#include <sigverse/commonlib/comm/encoder/GetJointForce.h>
#include <sigverse/commonlib/binary.h>

BEGIN_NS_COMMDATA();

char * RequestGetJointForce::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	BINARY_SET_STRING_INCR(p, m_agentName.c_str());
	BINARY_SET_STRING_INCR(p, m_jointName.c_str());

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

char * ResultGetJointForce::encode(int seq, int &sz)
{
	char *p = m_buf;

	p = setHeader(p, seq);

	unsigned short b = m_data.success()? 1: 0;
	BINARY_SET_DATA_S_INCR(p, unsigned short, b);

	if (b) {
		JointForce *jfs = m_data.getJointForces();

		for (int i=0; i<ResultGetJointForceData::JOINT_FORCE_NUM; i++) {
			JointForce *jf = &jfs[i];

			const char *partsName = jf->getPartsName();
			BINARY_SET_STRING_INCR(p, partsName);
			
			const Vector3d &f = jf->getForce();
			BINARY_SET_DOUBLE_INCR(p, f.x());
			BINARY_SET_DOUBLE_INCR(p, f.y());
			BINARY_SET_DOUBLE_INCR(p, f.z());

			const Vector3d &tq = jf->getTorque();
			BINARY_SET_DOUBLE_INCR(p, tq.x());
			BINARY_SET_DOUBLE_INCR(p, tq.y());
			BINARY_SET_DOUBLE_INCR(p, tq.z());
		}
	}

	p = setFooter(p);

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

END_NS_COMMDATA();
