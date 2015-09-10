/* Wrtten by okamoto on 2011-03-25 02
 * Modified by Tetsunari Inamura on 2013-12-29
 *   Add English comments
 */
// This file was used in old version (ver.1). Just keeping for the reference in ver.2
#ifndef PacketForward_h
#define PacketForward_h

#include <sigverse/commonlib/CommDataDecoder.h>

class ServerAcceptProc;
class SimWorldProvider;

/**
 * @brief Class for transport of packet
 *
 * Relaying the packet sent from a client to another client
 * Implimentation of CommDataDecoder::PacketSender interface
 */
class PacketForward : public CommDataDecoder::PacketSender
{
private:
	typedef CommDataDecoder::PacketSender Super;
private:
	ServerAcceptProc &m_accept;
	SimWorldProvider &m_world;
public:
	PacketForward(ServerAcceptProc &a, SimWorldProvider &w) : Super(), m_accept(a), m_world(w) {;}
private:
	void forward(Source &from, CommDataType cmdType, const char *to, int bytes, char *data, bool returnImmediate, double reachRadius);
};

#endif // PacketForward_h



