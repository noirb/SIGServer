/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Result_h
#define Result_h

#include "Encoder.h"

namespace CommData {

class Result :  public Encoder
{
private:
	enum { BUFSIZE = 128, };
private:
	CommResultType m_result;
	std::string    m_msg;
public:
	Result(CommDataType type, CommResultType r, const char *msg)
		: Encoder(type, BUFSIZE), m_result(r)
	{
		if (msg) {
			m_msg = msg;
		}
	}

	int packetNum() { return 1; }
	char *encode(int seq, int &);
};

} // namespace

typedef CommData::Result CommResultEncoder;

#endif // Result_h
 

