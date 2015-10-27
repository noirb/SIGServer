/*
 * Modified by Okamoto on 2011-03-25
 */

#ifndef BodyXMLReader_h
#define BodyXMLReader_h

class SSimObj;
class ODEWorld;

class BodyXMLReader
{
private:
	SSimObj &m_obj;
	ODEWorld &m_w;
public:
	BodyXMLReader(SSimObj &obj, ODEWorld &w) : m_obj(obj), m_w(w) { ; }
	bool read(const char *fname);
//	bool operator()(const char *fname)
//	{
//		return read(fname);
//	}
};


#endif // BodyXMLReader_h
 

