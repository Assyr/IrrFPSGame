#pragma once
#ifndef INC_CHARACTER_H
#define INC_CHARACTER_H

#include <irrlicht.h>
#include <IrrAI.h>

using namespace irr;
using namespace scene;
using namespace IrrAI;

class Character
{
public:

	//Struct to hold all our character properties
	struct CharacterDesc
	{
		CharacterDesc()
		{
			//On CharacterDesc creation set the values to the following..
			sceneManager = NULL;
			AIManager = NULL;
			startWaypointID = 1;
			WaypointGroupName = "";
			waypointGroupIdx = 0;
			NPCRange = 200; //Controls range of NPC before they react.
			Name = L"";
			regenerateHealth = true;
			
		}

		//These are the properties of our struct
		ISceneManager* sceneManager;
		IAIManager* AIManager;
		s32 startWaypointID;
		core::stringc WaypointGroupName;
		s32 waypointGroupIdx;
		f32 NPCRange;
		core::stringw Name;
		bool regenerateHealth;

	};

	static const s32 MAX_AMMO; //Holds the maximum ammo
	static const s32 MAX_HEALTH; //Maximum health (100)
	static const s32 REFILL_PERIOD; //How long it takes to reload ammo.


	//Character constructor
	Character(const CharacterDesc& desc, s32 characterType);


	//Deconstructor
	virtual ~Character();


	virtual bool update(s32 elapsedTime); //Updates character and provide an elapsed time.

	//Informs character if been hit by enemy weapon (collision detection);

	//On hit...
	inline virtual void registerHit()
	{
		health -= 20; timeSinceLastRefill = 0; //Reduce health and reset timeSinceLastRefill
	}

	inline IAIEntity* getAIEntity()
	{
		return AIEntity;
	}

	//Get character absolute position
	inline core::vector3df getAbsolutePosition()
	{
		return AIEntity->getAbsolutePosition();
	}


	//Return character type

	inline s32 getCharacterType()
	{
		return CharacterType;
	}

	//Return character health
	inline s32 getHealth()
	{
		return health;
	}


	inline s32 getAmmo()
	{
		return Ammo;
	}
	inline void addHealth(s32 amount)
	{
		health += amount;
		if (health > 100) health = 100; //Add health and check to make srue health isnt more than 100 NOR less than 0
		if (health < 0) health = 0;
	}


	virtual void die();//Holds death animation
protected:
	s32 health;
	s32 timeSinceLastRefill;
	IAIEntity* AIEntity;
	IAIManager* AIManager;
	ISceneManager* sceneManager;
	core::vector3df pos;
	s32 CharacterType;
	bool regenerateHealth;
	s32 Ammo;
	//Protected variables of this class that holds data regarding our character..
};

#endif
