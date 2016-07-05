#pragma once

#ifndef INC_CHASINGCHARACTER_H
#define INC_CHASINGCHARACTER_H

#include "NPCharacter.h"
#include "Projectile.h"

#include <irrlicht.h>
#include <IrrAI.h>

using namespace irr;

class ChasingCharacter : public NPCharacter
{
public:
	ChasingCharacter(const CharacterDesc& desc, c8* meshLocation, c8* textureLocation, scene::IMetaTriangleSelector* selector);
	~ChasingCharacter();
	void chase(IAIEntity* characterChase);
	void attack(IAIEntity* characterAttack);
	void replan();
	void goTo(core::vector3df* position); //Find all waypoints, select 1 at random and set NPC entity direction to this.
	bool update(s32 elapsedTime); //Update 

	inline bool isChasing() { return Chasing; }
private:
	bool Chasing, EnemyVisible;
	s32 TimeSinceLastShot;
	core::array<Projectile*> myProjectiles;
	scene::IMetaTriangleSelector* metaSelector;
};

#endif