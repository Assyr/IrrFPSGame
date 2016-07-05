#ifndef INC_TURRETCHARACTER_H
#define INC_TURRETCHARACTER_H

#include "Character.h"
#include "Projectile.h"


class Turret : public Character
{
public:
	Turret(const CharacterDesc& desc, scene::IMetaTriangleSelector* metaSelector);
	~Turret();
	bool update(s32 elapsedTime);
	void fire(const core::vector3df& enemyPos);


private:
	scene::IMeshSceneNode* turretNode;
	scene::EMD2_ANIMATION_TYPE animation;
	scene::IMetaTriangleSelector* MetaSelector;
	core::array<Projectile*>Projectiles;
	s32 timeSincenLastShot;
};

#endif