/*
 * Created by okamoto on 2011-03-25
 */

#ifndef SimViewPoint_h
#define SimViewPoint_h

#ifdef USE_OPENGL

#include "systemdef.h"
#include "Position.h"

class SimViewPoint
{
private:
	Position m_pos;
	int m_roty;
	int m_rotz;
public:
	SimViewPoint(const Position &pos) : m_pos(pos), m_roty(0), m_rotz(0) {;}
#ifdef IMPLEMENT_DRAWER
	void draw(DrawContext &c);
#endif
	bool setKey(unsigned char key);
};

#endif

#endif // SimViewPoint_h
 
