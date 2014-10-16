/*
 * Written by Atsushi Okamoto on 2011-03-31
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-16
 *    Add English comments
 */
#ifndef CommDataResult_h
#define CommDataResult_h

#include "CommDataType.h"

/** 
 * @brief Class for getting the reconstructed data as a return value from CommDataDecoder
 *
 */

class CommDataResult
{
public:
	// Data free function type
	typedef void (*DataFreeFunc)(void*);
private:
	CommDataType m_type;
	int	m_seq;
	int	m_num;
	void * m_data;
	DataFreeFunc m_free_func;
public:
	//! Returns thetType of reconstructed data
	CommDataType type() { return m_type; }
	/**
	 * @brief Gets reconstructed data
	 *
	 * Here, recieved data corresponds to the type of the second argument of a method in CommDataListener depending on the data type
	 *
	 * <pre>
	 * (Example)
	 *   CommDataDcoder d;
	 *   CommDataResult *r = d.push(...);
	 *   if (r->type() == COMM_REQUEST_ATTACH_CONTROLLER) {
	 *      typedef RequestAttachControllerEvent Event;
	 *      Event *evt = (Event *)r->data();
	 *      ...;
	 *   }
	 * </pre>
	 *
	 * @see CommDataListener
	 * 
	 * @retval != NULL Reconstructed data depending on the data type
	 * @retval =NULL   e.g. when packets are forwarded without reconstructing data (Forward method returns true)
	 *
	 */
	void * data() { return m_data; }

	bool	forwarded() { return m_num > 0? true: false; }

	bool	forwardCompleted()
	{
		return m_seq == m_num -1? true: false;
	}
	int	seq() { return m_seq; }
	int	packetNum() { return m_num; }
public:
	CommDataResult(CommDataType t, void *d, DataFreeFunc func)
		: m_type(t), m_seq(-1), m_num(-1), m_data(d), m_free_func(func) {;}
	CommDataResult(CommDataType t, int seq, int num) :
		m_type(t),
		m_seq(seq), m_num(num), 
		m_data(NULL), m_free_func(NULL) {;}

	~CommDataResult() {
		if (m_data && m_free_func) {
			(*m_free_func)(m_data);
			m_data = NULL;
		}
	}
};

#endif // CommDataResult_h
 


