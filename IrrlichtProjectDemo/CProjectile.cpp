#include "CProjectile.h"

#define PROJECTILE_SPEED 4.0f

CProjectile::CProjectile(const core::vector3df& pos, const core::vector3df& dir, scene::ISceneManager* smgr) {

	Direction = dir;
	PrevPos = pos;
	SqDistTravelled = 0;
	SceneManager = smgr;
  
	// create animation for projectile
	core::array<video::ITexture*> textures;
	for (s32 g=1; g<=7; ++g) {
		c8 tmp[64];
		sprintf_s(tmp, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Projectile\\portal%d.jpg", g);
		video::ITexture* t = SceneManager->getVideoDriver()->getTexture(tmp);
		textures.push_back(t);
	}
	scene::ISceneNodeAnimator* anim = SceneManager->createTextureAnimator(textures, 25);

	// create projectile
	Bill = smgr->addBillboardSceneNode(SceneManager->getRootSceneNode(), core::dimension2d<f32>(15,15), pos, -3);
	Bill->setMaterialFlag(video::EMF_LIGHTING, false);
	Bill->setMaterialTexture(0, SceneManager->getVideoDriver()->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Projectile\\portal1.jpg"));
	Bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	Bill->addAnimator(anim);
	anim->drop();
                                           
}

CProjectile::~CProjectile() {

	remove();
                            
}

void CProjectile::remove() {
     
	core::vector3df pos = Bill->getPosition();
	Bill->remove();
	Bill = NULL;
  
	s32 frameTime = 25;
	scene::ISceneNodeAnimator* anim = NULL;
  
	// create animation for explosion
	c8 tmp[64];
	core::array<video::ITexture*> textures;
	for (s32 g=1; g<=6; ++g) {
		sprintf_s(tmp, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Plasmaball\\%02d.jpg", g);
		textures.push_back(SceneManager->getVideoDriver()->getTexture(tmp));
	}
	anim = SceneManager->createTextureAnimator(textures, frameTime, false);
       
	// create explosion
	Bill = SceneManager->addBillboardSceneNode(SceneManager->getRootSceneNode(), core::dimension2d<f32>(25,25), pos, -5);
	Bill->setMaterialFlag(video::EMF_LIGHTING, false);
	Bill->setMaterialTexture(0, SceneManager->getVideoDriver()->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Plasmaball\\01.jpg"));
	Bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	Bill->addAnimator(anim);
	anim->drop();
	// create deletion animator to automatically remove the billboard
	anim = SceneManager->createDeleteAnimator(frameTime*6);
	Bill->addAnimator(anim);
	anim->drop();
     
}

bool CProjectile::update() {
  
	if (SqDistTravelled > 1000000) return true;
  
	PrevPos = Bill->getPosition();
  
	core::vector3df distance = Direction*PROJECTILE_SPEED;
  
	if (Bill) Bill->setPosition(PrevPos + distance);
  
	SqDistTravelled += distance.getLengthSQ();
  
	return false;
     
}
