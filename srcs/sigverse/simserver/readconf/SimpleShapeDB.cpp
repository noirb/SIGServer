/*
 * Modified by MSI on 2011-05-12
 */

#include "SimpleShapeDB.h"

///////////////////////////////////////////////////////////////
//
//	SimpleShapeElem

SimpleShapeElem::SimpleShapeElem(SS_TYPE t) :
	m_type(t),
	m_x(0), m_y(0), m_z(0),
	m_sx(0), m_sy(0), m_sz(0),
	m_r(0),
	m_h(0)
{
}

void SimpleShapeElem::setPosition(double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

void SimpleShapeElem::setSize(double sx, double sy, double sz)
{
	m_sx = sx;
	m_sy = sy;
	m_sz = sz;
}

void SimpleShapeElem::setRadius(double r)
{
	m_r = r;
}

void SimpleShapeElem::setHeight(double h)
{
	m_h = h;
}

///////////////////////////////////////////////////////////////
//
//	SimpleShape

SimpleShape::SimpleShape(const char *entClassName)
{
	m_entClassName = entClassName ? entClassName : "";

	m_mapSSE.clear();
}

SimpleShape::~SimpleShape()
{
	std::map<std::string, SimpleShapeElem *>::iterator i;

	for (i=m_mapSSE.begin(); i!=m_mapSSE.end(); i++)
	{
		SimpleShapeElem *p = i->second;
		if (p) delete p;
	}

	m_mapSSE.clear();
}

SimpleShapeElem *SimpleShape::get(const char *partsName)
{
	std::string key = partsName ? partsName : "";

	std::map<std::string, SimpleShapeElem *>::iterator i=m_mapSSE.find(key);
	if (i != m_mapSSE.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}

void SimpleShape::set(const char *partsName, SimpleShapeElem* psse)
{
	std::string key = partsName ? partsName : "";

	std::map<std::string, SimpleShapeElem *>::iterator i=m_mapSSE.find(key);
	if (i != m_mapSSE.end())
	{
		SimpleShapeElem *e = i->second;
		if (e) delete e;
	}

	m_mapSSE[key] = psse;
}

std::vector<std::string> SimpleShape::getAllPartsNames()
{
	std::vector<std::string> partsNames;

	std::map<std::string, SimpleShapeElem *>::iterator i;

	for (i=m_mapSSE.begin(); i!=m_mapSSE.end(); i++)
	{
		partsNames.push_back(i->first);
	}

	return partsNames;
}

void SimpleShape::dump()
{
	std::map<std::string, SimpleShapeElem *>::iterator i;

	printf("+++ entClassName=(%s) +++\n", m_entClassName.c_str());

	for (i=m_mapSSE.begin(); i!=m_mapSSE.end(); i++)
	{
		std::string key = i->first;
		SimpleShapeElem *s = i->second;
		if (s)
		{
			printf("\t*** partsName=(%s) ***\n", key.c_str());

			switch (s->getType())
			{
				case SimpleShapeElem::SS_TYPE_BOX:
				{
					printf("\t\ttype=box\n");
					printf("\t\tposition=(%f, %f, %f)\n", s->x(), s->y(), s->z());
					printf("\t\tsize=(%f, %f, %f)\n", s->sx(), s->sy(), s->sz());
				}
				break;

				case SimpleShapeElem::SS_TYPE_CYLINDER:
				{
					printf("\t\ttype=cylinder\n");
					printf("\t\tposition=(%f, %f, %f)\n", s->x(), s->y(), s->z());
					printf("\t\tradius=(%f)\n", s->r());
					printf("\t\theight=(%f)\n", s->h());
				}
				break;

				case SimpleShapeElem::SS_TYPE_SPHERE:
				{
					printf("\t\ttype=sphere\n");
					printf("\t\tposition=(%f, %f, %f)\n", s->x(), s->y(), s->z());
					printf("\t\tradius=(%f)\n", s->r());
				}
				break;

				default:
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////
//
//	SimpleShapeDB

SimpleShapeDB::SimpleShapeDB()
{
	m_mapSS.clear();
}

SimpleShapeDB::~SimpleShapeDB()
{
	std::map<std::string, SimpleShape *>::iterator i;

	for (i=m_mapSS.begin(); i!=m_mapSS.end(); i++)
	{
		SimpleShape *ss = i->second;
		if (ss) delete ss;
	}

	m_mapSS.clear();
}

SimpleShape *SimpleShapeDB::get(const char *entClassName)
{
	std::string key = entClassName ? entClassName : "";

	std::map<std::string, SimpleShape *>::iterator i=m_mapSS.find(key);
	if (i!=m_mapSS.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}

void SimpleShapeDB::set(const char *entClassName, const char *partsName, SimpleShapeElem* pss)
{
	if (!pss) return;

	SimpleShape *ss;
	std::string key = entClassName ? entClassName : "";

	std::map<std::string, SimpleShape *>::iterator i=m_mapSS.find(key);
	if (i!=m_mapSS.end())
	{
		ss = i->second;
	}
	else
	{
		ss = new SimpleShape(entClassName);
		m_mapSS[key] = ss;
	}

	if (ss) ss->set(partsName, pss);
}

void SimpleShapeDB::dump()
{
	printf("\n");
	printf("**** dump of SimpleShapeDB ****\n");

	std::map<std::string, SimpleShape *>::iterator i;

	for (i=m_mapSS.begin(); i!=m_mapSS.end(); i++)
	{
		SimpleShape *ss = i->second;
		if (ss) ss->dump();
	}
}

