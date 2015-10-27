/*
 * Written on 2011-05-12
 *
 * Modified by Yoshiaki Mizuchi on 2014-01-16
 *    Modify indent, add English comments
 */
#ifndef PartsCmpnt_h
#define PartsCmpnt_h

#include <sigverse/commonlib/Parts.h>

/**
 * @brief Parts data class of boxy components
 */
class BoxPartsCmpnt : public PartsCmpnt
{
public:
	enum { DATA_LEN = sizeof(double)*3, };
private:
	Size m_sz;
	char m_buf[DATA_LEN];
public:
	/**
	 * @brief Constructor
	 *
	 * @param sz Box size (length * width * height)
	 */
	BoxPartsCmpnt(const Size &sz) : PartsCmpnt(), m_sz(sz) {}
	/**
	 * @brief Copy constructor
	 */
	BoxPartsCmpnt(const BoxPartsCmpnt &o) : PartsCmpnt(), m_sz(o.m_sz) {}
public:
	//! Gets box size
	Size & size() { return m_sz; }

#if 1
// FIX20110421(ExpSS)
	//! Sets box size
	void size(dReal sx, dReal sy, dReal sz)
	{
		m_sz.set(sx, sy, sz);
	}

	//! Shows box size
	void dump()
	{
		printf("\t\tsize(%f, %f, %f)\n", m_sz.x(), m_sz.y(), m_sz.z());
	}
#endif

private:
	int    datalen() { return DATA_LEN; }
	char * dataBinary();
};

/**
 * @brief Parts data class of cylindrical components
 */
class CylinderPartsCmpnt : public PartsCmpnt
{
public:
	enum { DATA_LEN = sizeof(double)*2, };
private:
	dReal m_rad;
	dReal m_len;
	char m_buf[DATA_LEN];
public:
	/**
	 * @brief Constructor
	 *
	 * @param r radius of a cylinder
	 * @param l kength of a cylinder
	 */
	CylinderPartsCmpnt(dReal r, dReal l) : PartsCmpnt(), m_rad(r), m_len(l) {}
	/**
	 * @brief Copy constructor
	 */
	CylinderPartsCmpnt(const CylinderPartsCmpnt &o)
		: PartsCmpnt(), m_rad(o.m_rad), m_len(o.m_len) {}

	//! Gets the radius of a cylinder
	dReal radius() { return m_rad; }

	//! Gets the length of a cylinder
	dReal length() { return m_len; }

#if 1
// FIX20110421(ExpSS)
	//! Sets the radius of a cylinder
	void radius(dReal r) { m_rad = r; }

	//! Sets the length of a cylinder
	void length(dReal l) { m_len = l; }

	//! Shows cylinder size
	void dump()
	{
		printf("\t\tr(%f) l(%f)\n", m_rad, m_len);
	}
#endif

private:
	int    datalen() { return DATA_LEN; }
	char * dataBinary();
};

/**
 * @brief Parts data class of spherical components
 */
class SpherePartsCmpnt : public PartsCmpnt
{
public:
	enum { DATA_LEN = sizeof(double), };
private:
	double m_radius;
public:
	/**
	 * @brief Constructor
	 *
	 * @param rad radius of a sphere
	 */
	SpherePartsCmpnt(double rad) : PartsCmpnt(),  m_radius(rad) {}
	/**
	 * @brief Copy constructor
	 */
	SpherePartsCmpnt(const SpherePartsCmpnt &o)
		: PartsCmpnt(), m_radius(o.m_radius) {}

	//! Gets the radius of a sphere
	double radius() { return m_radius; }

#if 1
// FIX20110421(ExpSS)
	//! Sets the radius of a sphere
	void radius(double r) { m_radius = r; }

	//! Shows sphere size
	void dump()
	{
		printf("\t\tr(%f)\n", m_radius);
	}
#endif

private:
	int    datalen() { return DATA_LEN; }
	char * dataBinary();
};

#endif // PartsCmpnt_h



