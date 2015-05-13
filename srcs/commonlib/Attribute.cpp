/*
 * written by okamoto on 2011-03-31
 */

#include "Attribute.h"
#include "Value.h"
#include "binary.h"


Attribute::Attribute(const Attribute &o) :
	m_name(o.m_name), m_value(NULL), m_buf(0), m_bufsize(0), m_voffset(0)
{
	m_value = o.m_value->clone();
}

void Attribute::free_()
{
	delete m_value; m_value = 0;
	delete [] m_buf; m_buf = 0;
}

int Attribute::binarySize() const
{
	int n = 0;
	n += sizeof(DataLengthType); // attrlen
	n += BINARY_STRING_LEN(m_name.length()); // name
	n += BINARY_STRING_LEN(m_group.length()); // group
	n += sizeof(DataLengthType); // value data len
	n += m_value->binaryLength(); // value data
	return n;
}

const char *Attribute::toString()
{
	static std::string buf;
	buf = name();
	buf += "(";
	buf += value().getTypeString();
	buf += ",";
	buf += group();
	buf += ") -> ";
	buf += value().getString();
	return buf.c_str();
}


char *Attribute::toBinary(int &n)
{
	n = binarySize();
	if (!m_buf) {
		m_buf = new char[n];
		memset(m_buf, 0x00, n);
		m_bufsize = n;
		char *p = m_buf;
		BINARY_SET_DATA_S_INCR(p, DataLengthType, n);
		BINARY_SET_STRING_INCR(p, m_name.c_str());
		BINARY_SET_STRING_INCR(p, m_group.c_str());
		BINARY_SET_DATA_S_INCR(p, unsigned short, m_value->binaryLength());
		m_voffset = p - m_buf;
	}
	char *pp = m_buf + m_voffset;
	memcpy(pp, m_value->binary(), m_value->binaryLength());
	return m_buf;
}

int Attribute::setBinary(char *data, int n)
{
	char *p = data;
	DataLengthType datalen = BINARY_GET_DATA_S_INCR(p, DataLengthType);
	if (n < datalen) { return -1; }

	char *name = BINARY_GET_STRING_INCR(p);
	if (name) {
		m_name = name;
	}
	BINARY_FREE_STRING(name);

	char *group = BINARY_GET_STRING_INCR(p);
	if (group) {
		m_group = group;
	}
	BINARY_FREE_STRING(group);

	DataLengthType vlen = BINARY_GET_DATA_S_INCR(p, DataLengthType);
	
	m_value = Value::decode(p);

	return datalen;
}


#ifdef Attribute_test
int main()
{
	{
		Value *v = new StringValue();
		v->setString("foo");
		Attribute attr("namestring", v, "general");
		int n;
		char *p = attr.toBinary(n);

		//		fwrite(p, 1, n, stdout);
		Attribute decoded;
		if (decoded.setBinary(p, n) < 0) {
			fprintf(stderr, "cannot decode");
			return 1;
		}
		printf("%s\n", decoded.toString());
	}
	return 0;

}
#endif
