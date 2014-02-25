/*
 * Modified by noma on 2012-02-03
 * Modified by Tetsunari Inamura on 2014-02-25
 *    Translate Japanese comments in English
 */

#ifndef SimpleShapeDB_h
#define SimpleShapeDB_h

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

// ------------------------------------------------
// @brief simple shape for 1 part
// ------------------------------------------------
class SimpleShapeElem
{
public:
	enum SS_TYPE
	{
		SS_TYPE_UNKNOWN,
		SS_TYPE_BOX,
		SS_TYPE_CYLINDER,
		SS_TYPE_SPHERE
	};

	SimpleShapeElem(SS_TYPE t);

	void setPosition(double x, double y, double z);
	void setSize(double sx, double sy, double sz);
	void setRadius(double r);
	void setHeight(double h);

	SS_TYPE getType() { return m_type; }
	double x()  { return m_x;  }
	double y()  { return m_y;  }
	double z()  { return m_z;  }
	double sx() { return m_sx; }
	double sy() { return m_sy; }
	double sz() { return m_sz; }
	double r()  { return m_r;  }
	double h()  { return m_h;  }

private:
	SS_TYPE m_type;
	double m_x, m_y, m_z;
	double m_sx, m_sy, m_sz;
	double m_r;
	double m_h;
};

// ------------------------------------------------
// @brief set of simple shape in an entity class
//
// An entity consists of multiple parts (n>=1)
// ------------------------------------------------
class SimpleShape
{
public:
	SimpleShape(const char *entClassName);
	~SimpleShape();

	SimpleShapeElem *get(const char *partsName);
	void set(const char *partsName, SimpleShapeElem* psse);

	std::vector<std::string> getAllPartsNames();

	void dump();

private:
	std::string m_entClassName;

	// key is name of the part
	// (ex) "body", "WAIST_LINK0"
	std::map<std::string, SimpleShapeElem *> m_mapSSE;
};

// ------------------------------------------------
// @brief information of all of the simple shape
// ------------------------------------------------
class SimpleShapeDB
{
public:
	SimpleShapeDB();
	~SimpleShapeDB();

	SimpleShape *get(const char *entClassName);
	void set(const char *entClassName, const char *partsName, SimpleShapeElem* pss);

	void dump();

private:
	// key is name of entity class
	// (= value of name attribution in define-class tag in the entity definition XML file)
	//
	// (ex) "Apple", "Orange", "Robot", and so on.
	std::map<std::string, SimpleShape *> m_mapSS;
};

#endif

