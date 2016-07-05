#ifndef INC_CPLAYERCHARACTER_H
#define INC_CPLAYERCHARACTER_H

#include <irrlicht.h>
#include <irrAI.h>

#include "main.h"

#include "Character.h"
#include "Projectile.h"

using namespace irr;

class CPlayerCharacter : public Character {
  
	public:
		CPlayerCharacter(const CharacterDesc& desc, scene::ICameraSceneNode* camera, scene::IMetaTriangleSelector* selector, s32 type = ECT_SPECTATING );
		~CPlayerCharacter();
		bool update(s32 elapsedTime);
		void fire();
    
		inline scene::IAnimatedMeshSceneNode* getNode() { return GunNode; }
    
	private:
		scene::ICameraSceneNode* Camera;
		scene::IAnimatedMeshSceneNode* GunNode;
		s32 TimeSinceLastShot;
		core::list<Projectile*> Projectiles;
		scene::IMetaTriangleSelector* MetaSelector;
                    
};

#endif /* INC_CPLAYERCHARACTER_H */
