#ifndef INC_CTURRETCHARACTER_H
#define INC_CTURRETCHARACTER_H

#include "Character.h"
#include "Projectile.h"

class CTurretCharacter : public Character {
      
	public:
		CTurretCharacter(const CharacterDesc& desc, scene::IMetaTriangleSelector* metaSelector);
		~CTurretCharacter();
		bool update(s32 elapsedTime);
		void fire(const core::vector3df& enemyPos);
       
	private:
		scene::IMeshSceneNode* TurretNode;
		scene::EMD2_ANIMATION_TYPE Animation;
		scene::IMetaTriangleSelector* MetaSelector;
		core::array<Projectile*> Projectiles;
		s32 TimeSinceLastShot;
    
};

#endif /* INC_CTURRETCHARACTER_H */
