/*
 * Modified by okamoto on 2012-03-19
 */
#ifndef CommController_Collision_h
#define CommController_Collision_h

#include <string>
#include <vector>
#include <string.h>

#include "Controller.h"

class CollisionEvent : public ControllerEvent
{
private:
	std::vector<std::string> m_other_entitiesname;
	std::vector<std::string> m_other_partsname;
	std::vector<std::string> m_self_partsname;
public:
	bool set(int packetNum, int seq, char *data, int n);

	//! get other colliding entities name list.
	const std::vector<std::string> & getWith() { return m_other_entitiesname; }

	//added by okamoto@tome 2012/3/19
	//! get other colliding parts name list.(If it is Entity, get body)
	const std::vector<std::string> & getWithParts() { return m_other_partsname; }

	//! get own colliding parts name list.
	const std::vector<std::string> & getMyParts() { return m_self_partsname; }
};


#endif // CommController_Collision_h
 
