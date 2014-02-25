/* 
 * Written by okamoto on 2011-09-09
 *
 */
// This file was used in old version (ver.1). Just keeping for the reference in ver.2

#ifdef IRWAS_SIMSERVER

#include "PacketForward.h"
#include "ServerAcceptProc.h"
#include "Source.h"
#include "Logger.h"
#include "CommUtil.h"
#include "SimWorldProvider.h"

void PacketForward::forward(Source &from, CommDataType cmdType,
			    const char *to, int bytes, char *data,
			    bool returnImmediate, double reachRadius)
{
	LOG_DEBUG1(("forward start\n"));
	Source *target = 0;
	switch(cmdType) {
	case COMM_REQUEST_CAPTURE_VIEW_IMAGE:
	case COMM_REQUEST_DISTANCE_SENSOR:
	case COMM_REQUEST_DETECT_ENTITIES:
		LOG_ERR(("Forwarding to service provider is not supported no more"));
		return;
	default:

		if (to) {
			target = m_accept.get(to, SOURCE_TYPE_CONTROLLER_CMD);
		}
		break;
	}
	if (to && !target) {
		LOG_ERR(("Cannot forward packet : no agent \"%s\"", to));
		return;
	}
	LOG_DEBUG1(("to : %s", to? to: "all"));
	if (target) {
		target->ignore(true);
		target->send(data, bytes);

#if 0
		// returnImmediate is valid only for the destination is determined
		if (returnImmediate) {

			LOG_DEBUG1(("forwad back immediate"));
			const int bufsize =   200000;
			const int poolsize = 1000000;
			char *buf = new char[bufsize];
			char *pool = new char[poolsize];
			char *poolh = pool;
			while (true) {
				fd_set	fds;
				FD_ZERO(&fds);
				FD_SET(sock, &fds);
				struct timeval tv;
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				int r = select(sock+1, &fds, 0, 0, &tv);
				if (r == 0) {
					LOG_DEBUG1(("timeout"));
					break;
				} // timeout

				if (r > 0) {
					int n = read(sock, buf, bufsize);
					if (n > 0) {
						memcpy(poolh, buf, n);
						poolh += n;
					}
				}

				int size = poolh - pool;
				if (size > poolsize/2) {
					int rr = CommUtil::sendData(from.socket(), pool, size);
					LOG_DEBUG1(("forward back to %s : %d bytes", from.hostname(), rr));
					poolh = pool;
				}
			}

			int size = poolh - pool;
			if (size > 0) {
				int rr = CommUtil::sendData(from.socket(), pool, size);
				LOG_DEBUG1(("forward back to %s : %d bytes", from.hostname(), rr));
			}

			delete [] buf;
			delete [] pool;
		}
#endif		
		target->ignore(false);

	} else {
		typedef ServerAcceptProc::C C;
		m_accept.lock();
		C clients(m_accept.clients());
		m_accept.unlock();
		for (C::iterator i=clients.begin(); i!=clients.end(); i++) {
			Connection *conn = *i;
			Source *source = conn->source;
			if (!source->isControllerCmd()) { continue; }
			if (source->equals(from)) { continue; }
			double dist = m_world.calcDistance(from.name(), source->name());
			LOG_DEBUG2(("distance between %s and %s is %f\n", from.name(), source->name(), dist));
			LOG_DEBUG2(("reachRadius = %f\n", reachRadius));
			bool send = true;
			if (reachRadius < 0 ||
			    (reachRadius > 0 && dist > 0 && dist < reachRadius)) {
				source->send(data, bytes);
			}
		}
	}

	LOG_DEBUG1(("forward end\n"));
};

#endif
