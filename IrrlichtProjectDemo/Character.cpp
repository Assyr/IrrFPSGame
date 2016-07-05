#include "Character.h"


using namespace core;

const s32 Character::MAX_AMMO = 120;
const s32 Character::MAX_HEALTH = 100;
const s32 Character::REFILL_PERIOD = 250; //How long it takes to refill ammo...

Character::Character(const CharacterDesc& desc, s32 characterType)
{
	//pass all data 
	sceneManager = desc.sceneManager;
	AIManager = desc.AIManager;
	regenerateHealth = desc.regenerateHealth;
	CharacterType = characterType;
	health = MAX_HEALTH;
	Ammo = MAX_AMMO;
	timeSinceLastRefill = 0;//set to 0 so it will refill properly
	AIEntity = NULL;


}


Character::~Character()
{
	if (AIEntity)//If this is still active
	{
		AIManager->removeAIEntity(AIEntity);
		AIEntity = NULL;//avoid memory leak.
	}
}


bool Character::update(s32 elapsedTime)
{
	if (health <= 0)
	{
		die();//play die animation
		return true;
	}

	if (regenerateHealth && timeSinceLastRefill > REFILL_PERIOD)
	{
		//Increment health by 1 every refill period
		if (health < MAX_HEALTH)
			health+=10;
		//Do the same for ammo --Health and ammo replenish at the same rate.
		if (Ammo < MAX_AMMO)
			++Ammo;

		//Reset timeSinceLastRefill
		timeSinceLastRefill = 0;
	}
	//Else add elapsed time to time since last refill till the above check is met..
	else{ timeSinceLastRefill += elapsedTime; }

	return false;
}
//Die animation..
void Character::die()
{
#define FRAME_TIME 50 //Control speed of death animation.


	//Same as before, create an explosion on death, represent it with a billboard.
	ISceneNodeAnimator* anim = NULL;

	//Create animation for explosion
	c8 temp[64];
	array<video::ITexture*> textures;
	for (s32 g = 1; g <= 11; ++g)
	{
		//Store texture to temp
		snprintf(temp, 64, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Explosion\\%02d.jpg", g);

		//Push back the texture
		textures.push_back(sceneManager->getVideoDriver()->getTexture(temp));
	}
	//Slow the animation speed down to 50fps.
	anim = sceneManager->createTextureAnimator(textures, FRAME_TIME, false);



	//Create explosion using a billboard, this billboard will appear at the characters dead body.
	ISceneNode* Bill = sceneManager->addBillboardSceneNode(NULL, dimension2d<f32>(70, 70),
		pos + vector3df(0, 10, 0), -4);

	Bill->setMaterialFlag(video::EMF_LIGHTING, false);
	Bill->setMaterialTexture(0, sceneManager->getVideoDriver()->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Explosion\\01.jpg"));
	Bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	Bill->addAnimator(anim);
	anim->drop();

	//Create deletion animator to remove the billboard
	anim = sceneManager->createDeleteAnimator(FRAME_TIME * 11);
	Bill->addAnimator(anim);
	anim->drop();
}




	