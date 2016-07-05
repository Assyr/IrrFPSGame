#include "Turret.h"
#include "TurretEntity.h"
#include "Projectile.h"
#include "CPlayerCharacter.h"

#define SHOT_DELAY_TIME	80 //Turret shot delay.


static void fireEventCallBack(const core::vector3df& enemyPos, void* userData)
{
	Turret* turrentInstance = (Turret*) userData;

	if (turrentInstance)
	{
		turrentInstance->fire(enemyPos);
	}
}

Turret::Turret(const CharacterDesc& desc, scene::IMetaTriangleSelector* metaSelector) : Character(desc, 0)
{
	
	MetaSelector = metaSelector;
	timeSincenLastShot = 0;

	//Load in turret mesh.

	scene::IMesh* mesh = sceneManager->getMesh("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\turret.ms3d");
	if (!mesh)
	{
		printf("Turrent mesh could not be loaded, check 'Turret.cpp'");
		return;
	}
	turretNode = sceneManager->addMeshSceneNode(mesh);
	turretNode->setMaterialFlag(video::EMF_LIGHTING, false);
	//printf("here!!!!!");
	turretNode->setScale(core::vector3df(20, 20, 20));
	turretNode->OnAnimate(0);

	//Create turret entity.

	TurretDesc entityDesc;
	entityDesc.fovDimensions = core::vector3df(400, 250, 0);
	entityDesc.range = 200.0f;
	entityDesc.UserData = this;
	entityDesc.Name = "Turret 0";
	core::aabbox3d<f32> box = turretNode->getTransformedBoundingBox();
	entityDesc.Scale = box.MaxEdge - box.MinEdge;
	entityDesc.Offset = core::vector3df(0, box.MaxEdge.Y - box.getCenter().Y, 0);
	AIEntity = new TurretEntity(entityDesc, AIManager, sceneManager);


	if (AIEntity)
	{
		AIEntity->setPosition(core::vector3df(-110, 0, -30));
		((TurretEntity*)AIEntity)->setFireEventCallBack(&fireEventCallBack);
	}
	else
	{
		printf("Failed to create turret entity, check 'Turret.cpp'");
	}
}

Turret::~Turret()
{
	//Remove turret node and set pointer to NULL.
	if (turretNode)
	{
		turretNode->remove();
		turretNode = NULL;
	}

	for (u32 i = 0; i < Projectiles.size(); i++)
	{
		delete Projectiles[i];
		Projectiles.clear();
	}
}


bool Turret::update(s32 elapsedTime)
{
	if (Character::update(elapsedTime))
	{
		return true;
	}

	if (turretNode && AIEntity)
	{
		turretNode->setPosition(AIEntity->getAbsolutePosition());
		turretNode->setRotation(AIEntity->getNode()->getRotation());
	}

	for (u32 i = 0; i < Projectiles.size(); i++) {
		if (Projectiles[i]->update()) {
			delete Projectiles[i];
			Projectiles.erase(i);
			i--;
		}
		else {
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

	timeSincenLastShot += elapsedTime;

	return false;

}

void Turret::fire(const core::vector3df& enemyPos) {

	if (timeSincenLastShot >= SHOT_DELAY_TIME) {
		// Calculate the position to fire from
		core::vector3df offset = core::vector3df(1.5f, 0, 0);
		core::matrix4 mat = turretNode->getAbsoluteTransformation();
		mat.transformVect(offset);
		// Calculate the direction to fire in
		core::vector3df dir = enemyPos - turretNode->getAbsolutePosition();
		dir.Y = 0;
		dir.normalize();
		// Fire projectile
		Projectile* proj = new Projectile(offset, dir, sceneManager);
		if (proj) Projectiles.push_back(proj);
		timeSincenLastShot = 0;
	}

}