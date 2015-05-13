/*
 * Created by okamoto on 2011-03-25
 */

#ifndef Encoder_h
#define Encoder_h

#include "systemdef.h"
#include "CommDataType.h"
#include "CommDataDef.h"

#include <string>
#include <string.h>
#include <stdlib.h>


#define BEGIN_NS_COMMDATA() namespace  CommData {
#define END_NS_COMMDATA() }

class CommDataEncoder
{
private:
	CommDataType    m_type;
	unsigned short  m_forwardFlags;
	std::string     m_forwardTo;
	double          m_forwardReachRadius;
protected:
	char *          m_buf;
	int             m_bufsize;
protected:
	//int  bufleft(char *p) { return m_bufsize - (p-m_buf); }
	char * bufresize(int add, char *ptr)
	{
		int idx = ptr - m_buf;
		
		int newsize = m_bufsize + add;
		char *newbuf = new char [newsize];
		memcpy(newbuf, m_buf, m_bufsize);
		delete m_buf;
		m_buf = newbuf; m_bufsize = newsize;
		return m_buf + idx;
	}
		
		
protected:
	CommDataEncoder(CommDataType type, int bufsize) : m_type(type), m_forwardFlags(false), m_forwardReachRadius(-1.0), m_buf(0), m_bufsize(bufsize)
	{
		m_buf = new char[bufsize];
	}
	void setForwardTo(const char *to, bool returnImmediate, double reachRadius);
	void setForwardTo(const char *to, bool returnImmediate)
	{
		setForwardTo(to, returnImmediate, -1);
	}
public:
	virtual ~CommDataEncoder()
	{
		if (m_buf) { delete [] m_buf; m_buf = 0; }
	}
	virtual char *encode(int seq, int &n) = 0;
	virtual int packetNum() = 0;

	virtual char *release() {
		char *tmp = m_buf;
		m_buf = 0;
		return tmp;
	}

	int send(SOCKET sock);

	typedef int (SendProc)(SOCKET, const char *, int);
	static void setSendProc(SendProc *p) { s_sendProc = p; }

private:
	static SendProc *s_sendProc;

protected:
	char * getHeader(int seq, int &n);
	char * getFooter(int &n);

	void setPacketSize(char *data, unsigned short n);

protected:
	char *  setHeader(char *buf, int seq);
	char *  setFooter(char *buf);

};

//BEGIN_NS_COMMDATA();
namespace CommData {

typedef CommDataEncoder Encoder;

class NoDataEncoder : public Encoder
{
	enum { BUFSIZE = COMM_DATA_HEADER_MAX_SIZE + COMM_DATA_FOOTER_SIZE, };

protected:
	NoDataEncoder(CommDataType t) : Encoder(t, BUFSIZE) {;}

public:
	int packetNum() { return 1; }
	char *encode(int seq, int &sz);
};

class RawDataEncoder : public Encoder
{
private:
	enum { UNIT_DATA_SIZE = 10000, };
	enum { THIS_BUFSIZSE  = UNIT_DATA_SIZE + sizeof(unsigned short), };
	enum { BUFSIZE        = COMM_DATA_HEADER_MAX_SIZE + COMM_DATA_FOOTER_SIZE + THIS_BUFSIZSE, };

private:
	int  m_packetNum;
public:
	RawDataEncoder(CommDataType t) : Encoder(t, BUFSIZE), m_packetNum(-1) {;}

	int packetNum();
	char * encode(int seq, int &);
protected:
	virtual char * getDataHeader(int &) = 0;
	virtual int    getDataLen() = 0;
	virtual char * getData() = 0;
};

//END_NS_COMMDATA();
} // namespace 

#endif // Encoder_h
 

