#pragma once

#ifndef INC_NPCHARACTER_H
#define INC_NPCHARACTER_H


#include <irrlicht.h>
#include <IrrAI.h>

#include "Character.h"

using namespace irr;
using namespace scene;


//This class controls the NP Characters (Non playable characters)
//This class inherits from 'Character' class
class NPCharacter : public Character
{
public:
	enum NPCHARACTER_TYPE
	{
		CHASING_CHARACTER,
		FLEEING_CHARACTER
	};
	//On creation of a NPC..
	//Constructor --This will take a 'CharacterDesc', a character type, the path to the mesh and the path to the texture
	NPCharacter(const CharacterDesc& desc, s32 type, c8* meshLocation, c8* textureLocation);
	
	//Destructor --MUST BE OVERRIDDEN
	virtual ~NPCharacter();

	//Update for NPC, similar to player/character class.
	virtual bool update(s32 elapsedTime);

	virtual void setAnimation(E_NPC_STATE_TYPE state);


	//This will return the node that the NPC belongs to.
	inline IAnimatedMeshSceneNode* getNode()
	{
		return characterNode;
	}
	inline void registerHit() //When an npc registers a hit..
	{
		Character::registerHit(); //Call the register hit method from the Character class which will reduce NPC health.. 
		//but also..
		((INPC*)AIEntity)->sendEventToNPC(ENET_UNDER_ATTACK, NULL); //Send a 'UNDER_ATTACK' event to the NPC so it can react as needed.
	}



protected:
	//This will store which node the NPC is part of.
	IAnimatedMeshSceneNode* characterNode;
	EMD2_ANIMATION_TYPE currAnimation;
	ITextSceneNode* enemyLabel;
};

#endif
