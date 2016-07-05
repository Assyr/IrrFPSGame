#include "Projectile.h"

using namespace core;
using namespace video;
using namespace scene;

#define PROJECTILE_SPEED 8.0f;

Projectile::Projectile(const vector3df& pos, const vector3df& dir, ISceneManager* smgr)
{
	direction = dir;
	prevPos = pos;
	distanceTravelled = 0;
	sceneManager = smgr;


	//Create our animation for the projectile.
	array<ITexture*> textures; //Array of textures..
	for (s32 g = 1; g <= 7; ++g)
	{
		//Array of 64 8 bit character variables
		c8 tmp[64];

		//Store data of following image to our array.
		sprintf_s(tmp, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Projectile\\portal%d.jpg", g);

		//Compile together the texture and push it into our 'textures' array
		ITexture* projectileTexture = sceneManager->getVideoDriver()->getTexture(tmp);
		textures.push_back(projectileTexture);
	}
		//Create an animation to slow down how fast it changes between each texture
		ISceneNodeAnimator* anim = sceneManager->createTextureAnimator(textures, 25);//projectile animates at 25 fps

		//Create our projectile using a billboard, set its size, position and an id.
		Bill = smgr->addBillboardSceneNode(sceneManager->getRootSceneNode(),
			dimension2d<f32>(15, 15), pos, -3);
		Bill->setMaterialFlag(EMF_LIGHTING, false);//Enable this when we get lights.

		Bill->setMaterialTexture(0, sceneManager->getVideoDriver()->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Projectile\\portal1.jpg"));
		Bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		//Add our texture animator to our billboard.
		Bill->addAnimator(anim);
		//We are done with our animator, drop it
		anim->drop();

	}



Projectile::~Projectile()
{
	remove(); //Removes projectiles on collision
}

void Projectile::remove()
{
	vector3df pos = Bill->getPosition();
	Bill->remove();
	Bill = NULL;

	s32 frameTime = 25;
	ISceneNodeAnimator* anim = NULL; //Our animator


	//Do the same as before, load in our texture and push it into our array of textures
	c8 tmp[64];
	array<ITexture*> textures;
	for (s32 g = 1; g <= 6; ++g)
	{
		sprintf_s(tmp, "C:\\Users\\Leon\\Desktop\\New folder\\Media\\Plasmaball\\%02d.jpg",g);
		textures.push_back(sceneManager->getVideoDriver()->getTexture(tmp));
	}

	//Setup animator to go through textures at 25 fps..
	anim = sceneManager->createTextureAnimator(textures, frameTime, false);//Dont loop - final parameter


	//Create the explosion using a billboard
	Bill = sceneManager->addBillboardSceneNode(sceneManager->getRootSceneNode(), 
		dimension2d<f32>(25, 25), pos, -5);
	Bill->setMaterialFlag(EMF_LIGHTING, false);
	Bill->setMaterialTexture(0, sceneManager->getVideoDriver()->getTexture("C:\\Users\\Leon\\Desktop\\New folder\\Media\\Plasmaball\\01.jpg"));
	Bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
	Bill->addAnimator(anim); //Add the animation to the billboard
	anim->drop();

	//Deletion animator to delete the billboard when the projectile is removed (otherwise the billboard will still exist.

	anim = sceneManager->createDeleteAnimator(frameTime * 6);
	Bill->addAnimator(anim); //Add deletion animator to our billboard to remove it.

}

bool Projectile::update()
{
	if (distanceTravelled > 1000000)
		return true; //Only update if distance travelled is more than 10000000

	prevPos = Bill->getPosition();

	vector3df distance = direction * PROJECTILE_SPEED; //PROJECTILE_SPEED is 8.0f;

	if (Bill)
	{
		Bill->setPosition(prevPos + distance); //Update position of billboard
	}

	//Update distance travelled for future updates
	distanceTravelled += distance.getLengthSQ(); //getLengthSQ is faster than getLength


	return false;


}
