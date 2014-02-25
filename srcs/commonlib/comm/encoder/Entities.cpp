/* $Id: Entities.cpp,v 1.1.1.1 2011-03-25 02:18:50 okamoto Exp $ */ 
#include "Entities.h"
#include "SimObjBase.h"
#include "SimWorld.h"
#include "binary.h"

BEGIN_NS_COMMDATA();

char *SendEndities::encode(int seq, int &sz)
{
	int n;
	char *p = m_buf;
	{
		char *h = getHeader(seq, n);
		memcpy(p, h, n);
		p += n;
	}

	BINARY_SET_DOUBLE_INCR(p, m_time);

#if 1
	SimObjBase *obj = m_objs[seq];
	char *pp = obj->toBinary(n);
	while (1) {
		int head = p - m_buf;
		if (head + n >= m_bufsize) { // realloc
			p = bufresize(RESIZE, p);
		} else { break; }
	}
	memcpy(p, pp, n);
	p += n;
#else
	
	const C &objs = m_objs;
	for (C::const_iterator i=objs.begin(); i!=objs.end(); i++) {
		SimObjBase *obj = *i;
		char *pp = obj->toBinary(n);

		while (1) {
			int head = p - m_buf;
			if (head + n >= m_bufsize) { // realloc
				p = bufresize(RESIZE, p);
			} else { break; }
		}
		memcpy(p, pp, n);
		p += n;
	}
#endif

	{
		char *f = getFooter(n);
		memcpy(p, f, n);
		p += n;
	}

	sz = p - m_buf;
	setPacketSize(m_buf, sz);
	return m_buf;
}

GetAllEntitiesResult::GetAllEntitiesResult(const SimWorld &w)
	: Super(COMM_RESULT_GET_ALL_ENTITIES, w.time())
{
	typedef SimWorld::M M;
	const M &objs = w.objs();
	for (M::const_iterator i=objs.begin(); i!=objs.end(); i++) {
		SimObjBase *p = i->second;
		push(p);
	}
}

END_NS_COMMDATA();
