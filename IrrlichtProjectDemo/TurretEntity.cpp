#include "TurretEntity.h"


TurretEntity::TurretEntity(const TurretDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr) 
: IAIEntity(desc, aimgr, smgr, (E_AIENTITY_TYPE)EEAIET_TURRET, 0)
{
	Enemy = NULL;

	fov = aimgr->createConeFieldOfView(desc.fovDimensions, true);
	fov->setPosition(Node->getAbsolutePosition());
	fovRange = desc.range;
	fovNode = aimgr->createDebugConeFOVSceneNode(desc.fovDimensions);
	fovNode->setParent(aimgr->getDebugRootNode());
	fovNode->setPosition(Node->getAbsolutePosition());
	aimgr->registerAIEntity(this);//Register our turret as an ai enttiy in our game.

}


TurretEntity::~TurretEntity()
{

	//Destroy our field of view node and remove pointer.
	if (fovNode)
	{
		fovNode->drop();
		fovNode->remove();
		fovNode = NULL;
	}

	if (fov)
	{
		delete fov;
		fov = NULL;
	}
}

void TurretEntity::update(s32 elapsedTime)
{
	Node->updateAbsolutePosition();//Update turret position

	if (Enemy)
	{
		const core::vector3df enemyPos = Enemy->getAbsolutePosition();

		//Check if enemy is in fov.
		if (fov->isInFOV(Enemy->getNode()->getTransformedBoundingBox(), enemyPos))
		{
			rotateToFace(enemyPos);//Rotate to enemypos if in fov
		}

		if (Node->getAbsolutePosition().getDistanceFrom(enemyPos) <= fovRange)
		{
			if (eventCallBackPtr)
			{
				eventCallBackPtr(enemyPos, UserData);
			}
		}
	}

	if (fov)
	{
		fov->setRotation(Node->getRotation());
	}
	if (fovNode)
	{
		fovNode->setVisible(AIManager->isDebugVisible());
		fovNode->setRotation(Node->getRotation());
	}
}

void TurretEntity::setPosition(const core::vector3df& pos)
{
	IAIEntity::setPosition(pos);

	if (fov)
	{
		fov->setPosition(getAbsolutePosition());
	}

	if (fovNode)
	{
		fovNode->setPosition(getAbsolutePosition());
	}
}
