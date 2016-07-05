#ifndef INC_TURRETENTITY_H
#define INC_TURRETENTITY_H

#include <IrrAI.h>

using namespace IrrAI;


struct TurretDesc : public SAIEntityDesc
{
TurretDesc() : SAIEntityDesc()
			{
				fovDimensions = core::vector3df(200, 200, 0);
				range = 100.0f;
			}

			core::vector3df	 fovDimensions;
			f32 range;
};

enum E_EXTENDED_AIENTTIY_TYPE
{
	EEAIET_TURRET = EAIET_NUM_TYPES + 1

};

#pragma once
class TurretEntity : public IAIEntity
{
public:
	TurretEntity(const TurretDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr);
	~TurretEntity();
	void update(s32 elapsedTime);
	void setPosition(const core::vector3df& pos);

	void setEnemy(IAIEntity* enemy)
	{
		Enemy = enemy;
	}
	IAIEntity* getEnemy()
	{
		return Enemy;
	}

	void setFireEventCallBack(void(*cb)(const core::vector3df&, void*))
	{
		eventCallBackPtr = cb;
	}

private:
	IFieldOfView* fov;
	f32 fovRange;
	IDebugFOVSceneNode* fovNode;
	IAIEntity* Enemy;

	void(*eventCallBackPtr)(const core::vector3df&, void*);
};

#endif