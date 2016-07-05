#include "ChasingCharacter.h"
#include "main.h"
#include <iostream>

using namespace std;

#define SHOT_DELAY_TIME 160 //Delay of shot or characters of type 'chasing' - all characters for now are of type chasing.


static void stateChangedCallback(E_NPC_STATE_TYPE state, void* userData)
{
	ChasingCharacter* character = (ChasingCharacter*)userData; //Create a character using the user data.
	//If character cannot be created with the data provided simply close
	if (!character)
	{
		printf("Issue creating character with userData provided, please check 'ChasingCharacter.cpp'");
		return;
	}

	//Check state of chasing character
	switch (state)
	{
	case ENST_WAITING: //If stationary.. do nothing.
	case ENST_MOVING : //If moving... set animation of character we created above to the state. (MOVING) - State is handled in 'Character.cpp'
		character->setAnimation(state);
		break;
	}
}

static void eventCallback(E_NPC_EVENT_TYPE event, void* userData, void* eventData)
{
	//Creation of character instance with the provided user data.

	ChasingCharacter* character = (ChasingCharacter*)userData;
	if (!character)
	{
		printf("Failed to load character with userData, check 'ChasingCharacter.cpp'");
		return;
	}
	
	//Character created.. now run checks using switch statement
	switch (event)
	{
	case ENET_AT_GOAL:
		character->replan();
		break;
	case ENET_ENEMY_VISIBLE:
		character->chase((IAIEntity*)eventData);
		break;
	case ENET_ENEMY_IN_RANGE:
		character->attack((IAIEntity*)eventData);
		break;
	case ENET_ENEMY_POSITION_KNOWN:
		character->goTo((core::vector3df*) eventData);
		break;
	}

}

ChasingCharacter::ChasingCharacter(const CharacterDesc& desc, c8* meshLocation, c8* textureLocation, IMetaTriangleSelector* selector): NPCharacter(desc, ECT_CHASING, meshLocation, textureLocation)
{
	//On creation set the character initial bool variables to false.
	Chasing = false;
	EnemyVisible = false;
	TimeSinceLastShot = SHOT_DELAY_TIME + 1;
	metaSelector = selector;

	if (AIEntity)
	{
		//Continue on its path
		((INPC*)AIEntity)->setStayPut(false);

		//Callbacks
		((INPC*)AIEntity)->
			setStateChangedCallback(&stateChangedCallback);
		((INPC*)AIEntity)->setEventCallback(&eventCallback);
	}
}

ChasingCharacter::~ChasingCharacter()
{
	//On chasing character destruction, remove projectiles
	for (u32 p = 0; p < myProjectiles.size(); p++)
	{
		delete myProjectiles[p];
	}
	myProjectiles.clear();//Avoid memory leaks.
}

//Characters chase method
void ChasingCharacter::chase(IAIEntity* characterChase)
{
	if (!characterChase || Chasing)
		return;

	//When character is in chasing, set waypoint to the closest one, we must get waypoint group which holds our waypoints in order to do so. Run this check using the characters absolute position.
	((INPC*)AIEntity)->setDestination(AIManager->getNearestWaypoint(((INPC*)AIEntity)->getWaypointGroup(), 
		characterChase->getAbsolutePosition()));

	//Set bools to true..
	Chasing = true;
	EnemyVisible = true;
	enemyLabel->setVisible(true);
}

void ChasingCharacter::attack(IAIEntity* characterAttack)
{
	
	if (!characterAttack)
		return;
	//In order to attack enemy visible is true...
	EnemyVisible = true;
	//Add a label above the NPC
	enemyLabel->setVisible(true);

	//NPC stop in its tracks when attacking so it doesnt move/chase and attack at the same time.
	//This will be only done to NPC entries

	if (TimeSinceLastShot >= SHOT_DELAY_TIME && Ammo > 0)
	{
		//Shoot at the person its chasing..		
		//Create instance of our projectile at the current attacker position, minus this position by the initial projectile position and normalize it. Then provide the scenemanager to do this
		Projectile* bullet = new Projectile(getAbsolutePosition(), (characterAttack->getAbsolutePosition() - getAbsolutePosition()).normalize(),
			sceneManager);

		//Push the bullet we created into our vector
		if (bullet)
		{
			myProjectiles.push_back(bullet);
			TimeSinceLastShot = 0; //Reset
			Ammo--;//decrease ammo by 1.
			timeSinceLastRefill = 0;
		}
	}
	
}

	void ChasingCharacter::replan()
	{
		//Reroute.

		Chasing = false; //NPC no longer chasing, rerouting path
		enemyLabel->setVisible(false);//No label above head.

		//Pick a random waypoint from the grid and set waypoint destination to it.
		SWaypointGroup* waypointNodes = ((INPC*)AIEntity)->getWaypointGroup();

		//Set NPC destination to a random node from our waypoint nodes.
		((INPC*)AIEntity)->setDestination(rand()%waypointNodes->Waypoints.size());
	}

	void ChasingCharacter::goTo(core::vector3df* position)
	{
		if (Chasing || !position)
			return;

		//Get nearest waypoint to 'position' and set AIEntity of NPC to it.


		//Get nearest waypoint to NPC
		IWaypoint* waypoint = AIManager->getNearestWaypoint(((INPC*)AIEntity)->getWaypointGroup(), 
			*position);

		//Set NPC destination to the waypoint we grabbed above and set chasing to true to continue chasing/finding the enemy
		((INPC*)AIEntity)->setDestination(waypoint);
		Chasing = true;

	}

	bool ChasingCharacter::update(s32 elapsedTime)
	{

		if (NPCharacter::update(elapsedTime))
			return true;

		//Update projectiles and remove from vector

		for (u32 p = 0; p < myProjectiles.size(); ++p)
		{
			//Update projectile position
			if (myProjectiles[p]->update())
			{
				delete myProjectiles[p];
				myProjectiles.erase(p);
			}
			else
			{
				Character* theEnemy = NULL;
				//Create a line/ray for every projectile, this ray is between the previous and current position of the projectile
				core::line3df ray(myProjectiles[p]->getPreviousPosition(),
					myProjectiles[p]->getPosition());
				
				core::vector3df outVec;
				core::triangle3df outTri;
				scene::ISceneNode* outNode;

				//Grab all enemies and throw them into an array called 'enemies'
				core::array<IAIEntity*> enemies = ((INPC*)AIEntity)->getEnemyGroup()->Entities;

				//Loop through the enemies and run collision detection, when collides register the hit with the enemy instance
				//Remove projectile

				for (u32 e = 0; e < enemies.size(); ++e)
				{
					//For each enemy in the array, store there 'user data'
					theEnemy = (Character*)(enemies[e]->getUserData());


					//Now run the checks on theEnemy..

					//If the array (that holds the projectile position) intersects with 'theEnemy'.. register the hit (enemy takes damage and animations are called...) and remove the projectile/pointer
					//and decrement the p-- which holds how many projectiles are on the field.
					if (theEnemy->getAIEntity()->getNode()->getTransformedBoundingBox().intersectsWithLine(ray))
					{
						theEnemy->registerHit();
						delete myProjectiles[p];
						myProjectiles.erase(p);
						p--;
						break;
					}

				}

				//if projectile/ray hits a wall/mesh/material.. also remove it. ADVANCED COLLISION DETECTION
				if (sceneManager->getSceneCollisionManager()->getCollisionPoint(ray, metaSelector, outVec, outTri, outNode))
				{
					delete myProjectiles[p];
					myProjectiles.erase(p);
					p--;
				}
			}
		}

		TimeSinceLastShot += elapsedTime;

		if (!EnemyVisible)
		{
			((INPC*)AIEntity)->setStayPut(false);
		}

			EnemyVisible = false;

			return false;
	}



