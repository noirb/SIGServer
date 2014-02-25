#include "Value.h"
#include "binary.h"
#include <vector>

/*******************************************

            BoolValue

********************************************/

const char *BoolValue::getString() const
{
	static char *strs[] = { "true", "false", };
		
	// bugfix(sekikawa)(2007/11/30)
	// int idx = m_value? 1:0;
	int idx = m_value ? 0 : 1;

	return strs[idx];
}

const char *BoolValue::getTypeString() const
{
	return "bool";
}

void BoolValue::setDouble(double v)
{
	m_value = ((int)v) != 0? true: false;
}
void BoolValue::setInt(int v)
{
	m_value = v != 0? true: false;
}

void BoolValue::setString(const char *str)
{
#ifdef WIN32
	if (strcmp(str, "0") == 0 || _stricmp(str, "false") == 0)
#else
		if (strcmp(str, "0") == 0 || strcasecmp(str, "false") == 0)
#endif
			{
				m_value = false;
			} else {
				m_value = true;
			}
}

Value *BoolValue::clone() {
	BoolValue *newv = new BoolValue();
	newv->m_value = this->m_value;
	return newv;
}

char *  BoolValue::binary()
{
	static char buf[DATASIZE];
	char *p = buf;
	BINARY_SET_DATA_S_INCR(p, ValueType, type());
	short v = m_value? 1: 0;
	BINARY_SET_DATA_S_INCR(p, short, v);
	return buf;
}

/*******************************************

            DoubleValue

********************************************/

const char *DoubleValue::getString() const
{
	static char buf[128];
	sprintf(buf, "%f", m_value);
	return buf;
}
	
const char *DoubleValue::getTypeString() const
{
	static const char *str = "double";
	return str;
}

Value * DoubleValue::clone() {
	DoubleValue *newv = new DoubleValue();
	newv->m_value = this->m_value;
	return newv;
}

char *  DoubleValue::binary()
{
	static char buf[DATASIZE];
	char *p = buf;
	BINARY_SET_DATA_S_INCR(p, ValueType, type());
	memcpy(p, (char*)&m_value, sizeof(m_value));
	return buf;
}
void DoubleValue::copy(const Value &o)
{
	this->m_value = o.getDouble();
}


/*******************************************

            StrinValue

********************************************/

Value *StringValue::clone() {
	return new StringValue(this->getString());
}

char * StringValue::binary()
{
	const int len = binaryLength();
	if (!m_buf || len > m_bufsize) {
		delete m_buf;
		m_buf = new char[len];
		m_bufsize = len;
	}
	char *p = m_buf;
	BINARY_SET_DATA_S_INCR(p, ValueType, type());
	BINARY_SET_STRING_INCR(p, m_str.c_str());
	return m_buf;
}

short StringValue::binaryLength() const
{
	return BINARY_STRING_LEN(m_str.length()) + sizeof(ValueType);
}

void StringValue::copy(const Value &o)
{
	m_str = o.getString();
}

/*******************************************

            decode

********************************************/


Value *Value::decode(char *data)
{
	char *p = data;
	ValueType type = BINARY_GET_DATA_S_INCR(p, ValueType);
	Value *value = 0;
	switch(type) {
	case VALUE_TYPE_BOOL:
		{
			value = new BoolValue();
			short v = BINARY_GET_DATA_S_INCR(p, short);
			value->setBool(v? true: false);
			break;
		}
	case VALUE_TYPE_DOUBLE:
		{
			value = new DoubleValue();
			value->setDouble(BINARY_GET_DOUBLE(p));
			break;
		}
	case VALUE_TYPE_STRING:
		{
			value = new StringValue();
			char *v = BINARY_GET_STRING_INCR(p);
			if (v) {
				value->setString(v);
			}

			BINARY_FREE_STRING(v);
			break;
		}
	default:
		break;
	}
	//assert(value != 0);
	return value;
}

namespace ValueNS {
	
	template <class T>
	class Tokenizer
	{
	private:
		typedef std::string S;
		typedef std::vector<S> C;
	private:
		C	m_strs;
	public:
		int tokenize(const char *str)
		{
			char buf[1024];
			strcpy(buf, str);
		
			char *target = buf;
			char *p;
			while ((p = strtok(target, "[], \t")) != 0) {
				m_strs.push_back(p);
				target = 0;
			}
			return m_strs.size();
		}
		int operator()(const char *str)
		{
			return tokenize(str);
		}

#define MIN(A, B) ( (A) < (B)? (A): (B) )
#define MAX(A, B) ( (A) > (B)? (A): (B) )

		T value()
		{
			int n = m_strs.size();
			if (n == 1) {
				return (T)atof(m_strs[0].c_str());
			} else if (n == 2) {
				T v0 = (T)atof(m_strs[0].c_str());
				T v1 = (T)atof(m_strs[1].c_str());
				T maxv = MAX(v0, v1);
				T minv = MIN(v0, v1);
				return  rand() * (maxv-minv)/RAND_MAX;
			} else {
				assert(0);
				return (T)0;
			}
		}
	};
}

void DoubleValue::setString(const char *str)
{
	typedef ValueNS::Tokenizer<double> Tok;
	
	Tok tokenize;
	int n = tokenize(str);
	if (n > 0) {
		m_value = tokenize.value();
	}
}

#ifdef Value_test
int main()
{
	Value *v = new DoubleValue();
	v->setString("[-1.0, 1.0]");
	printf("%f\n", v->getDouble());
	return 0;
}
#endif
