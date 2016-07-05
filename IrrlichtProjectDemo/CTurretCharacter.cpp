#include "CTurretCharacter.h"
#include "TurretEntity.h"
#include "Projectile.h"
#include "CPlayerCharacter.h"

#define SHOT_DELAY_TIME 80

static void fireEventCallback(const core::vector3df& enemyPos, void* userData) {
  
	CTurretCharacter* turret = (CTurretCharacter*)userData;
  
	if (turret) turret->fire(enemyPos);
       
}

CTurretCharacter::CTurretCharacter(const CharacterDesc& desc, scene::IMetaTriangleSelector* metaSelector) : Character(desc, 0) {

	MetaSelector = metaSelector;
	TimeSinceLastShot = 0;
  
	// Load up a mesh to use as an avatar for the NPC
	scene::IMesh* mesh = sceneManager->getMesh("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\turret.ms3d");
	if (!mesh) {
		printf("Turret Mesh load failed\n");
		return;
	}
	TurretNode = sceneManager->addMeshSceneNode(mesh);
	if (!TurretNode) {
		printf("Turret Node creation failed\n");
		return;
	}
	TurretNode->setMaterialFlag(video::EMF_LIGHTING, false);
	TurretNode->setScale(core::vector3df(10,10,10));
	TurretNode->OnAnimate(0);
  
	// Create the entity      
	TurretDesc entityDesc;
	entityDesc.fovDimensions = core::vector3df(400,250,0); 
	entityDesc.range = 200.0f;
	entityDesc.UserData = this;
	entityDesc.Name = "Turret 0";
	core::aabbox3d<f32> box = TurretNode->getTransformedBoundingBox();
	entityDesc.Scale = box.MaxEdge - box.MinEdge;
	entityDesc.Offset = core::vector3df(0,box.MaxEdge.Y - box.getCenter().Y,0);
	AIEntity = new TurretEntity(entityDesc, AIManager, sceneManager);
	if (AIEntity) {
		AIEntity->setPosition(core::vector3df(-110,0,-30));
		((TurretEntity*)AIEntity)->setFireEventCallBack(&fireEventCallback);
	} else printf("Failed entity creation\n");

}

CTurretCharacter::~CTurretCharacter() {

	if (TurretNode) {
		TurretNode->remove();
		TurretNode = NULL;
	}

	// Remove projectiles
	for (u32 i = 0 ; i < Projectiles.size() ; i++)
		delete Projectiles[i];
	 Projectiles.clear();
                    
}

bool CTurretCharacter::update(s32 elapsedTime) {
     
	if (Character::update(elapsedTime)) return true;
     
	if (TurretNode && AIEntity) {
		TurretNode->setPosition(AIEntity->getAbsolutePosition());
		TurretNode->setRotation(AIEntity->getNode()->getRotation());
	}
  
	// Update projectiles and check collisions
	for (u32 i = 0 ; i < Projectiles.size() ; i++) {
		if (Projectiles[i]->update()) {
			delete Projectiles[i];
			Projectiles.erase(i);
			i--;
		} else {
			core::line3df ray(Projectiles[i]->getPreviousPosition(), Projectiles[i]->getPosition());
			core::vector3df outVec;
			core::triangle3df outTri;
			scene::ISceneNode* outNode = NULL;
			IAIEntity* enemy = ((TurretEntity*)AIEntity)->getEnemy();
			if (enemy->getNode()->getTransformedBoundingBox().intersectsWithLine(ray)) {
				((CPlayerCharacter*)enemy->getUserData())->registerHit();
				delete Projectiles[i];
				Projectiles.erase(i);
				i--;
				break;
			}
			if (sceneManager->getSceneCollisionManager()->getCollisionPoint(ray, MetaSelector, outVec, outTri, outNode)) {
				delete Projectiles[i];
				Projectiles.erase(i);
				i--;
			}  
		}
	}
  
	TimeSinceLastShot += elapsedTime;

	return false;
        
}

void CTurretCharacter::fire(const core::vector3df& enemyPos) {
  
	if (TimeSinceLastShot >= SHOT_DELAY_TIME) {
		// Calculate the position to fire from
		core::vector3df offset = core::vector3df(1.5f,0,0);
		core::matrix4 mat = TurretNode->getAbsoluteTransformation();
		mat.transformVect(offset); 
		// Calculate the direction to fire in
		core::vector3df dir = enemyPos-TurretNode->getAbsolutePosition();
		dir.Y = 0;
		dir.normalize();
		// Fire projectile
		Projectile* proj = new Projectile(offset, dir, sceneManager);
		if (proj) Projectiles.push_back(proj);
		TimeSinceLastShot = 0;
	}
  
}
  
