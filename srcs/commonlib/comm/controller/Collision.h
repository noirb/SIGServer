/* $Id: Collision.h,v 1.4 2012-03-19 07:04:45 okamoto Exp $ */ 
#ifndef CommController_Collision_h
#define CommController_Collision_h

#include <string>
#include <vector>
#include <string.h>

#include "Controller.h"

class CollisionEvent : public ControllerEvent
{
public:
	typedef std::vector<std::string> WithC;
private:
	WithC m_with;
	
	//$B>WFMAj<j$N%Q!<%D$H>WFM$7$?<+J,$N%Q!<%D(B 2012/3/19
	WithC m_withParts;
	WithC m_myParts;
public:
	bool	set(int packetNum, int seq, char *data, int n);
	//! $B>WFM$7$?Aj<j$NL>A0$rF@$k(B
	const WithC & getWith() { return m_with; }

	//added by okamoto@tome 2012/3/19
	//! $B>WFM$7$?Aj<j$N%Q!<%DL>$rF@$k(B($BAj<j$,(BEntity$B$N>l9g$O(Bbody$B$r<hF@(B)
	const WithC & getWithParts() { return m_withParts; }

	//! $B>WFM$7$?<+J,$N%Q!<%DL>$rF@$k(B
	const WithC & getMyParts() { return m_myParts; }
};


#endif // CommController_Collision_h
 
