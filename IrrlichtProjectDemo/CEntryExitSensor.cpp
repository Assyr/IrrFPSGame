#include "CEntryExitSensor.h"

namespace IrrAI {

CEntryExitSensor::CEntryExitSensor(const SAIEntityDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr, s32 id) : IAISensor(desc, aimgr, smgr, EAIET_ENTRYEXIT_SENSOR, id) {
                       
}

void CEntryExitSensor::update(s32 elapsedTime) {
  
	IAIEntity::update(elapsedTime);

	if (!CallbackPtr) return;
  
	for (u32 i = 0 ; i < Entities.size() ; ++i) {
		IAIEntity* entity = Entities[i]->Entity;
		if (entity->getNode()->getTransformedBoundingBox().intersectsWithBox(Node->getTransformedBoundingBox())) {
			if (((SEntryExitSensorData*)Entities[i])->State == EAISST_OUTSIDE) 
				CallbackPtr(this, entity, EAISET_ENTER);
			((SEntryExitSensorData*)Entities[i])->State = EAISST_INSIDE;
		} else {
			if (((SEntryExitSensorData*)Entities[i])->State == EAISST_INSIDE) 
				CallbackPtr(this, entity, EAISET_EXIT);
			((SEntryExitSensorData*)Entities[i])->State = EAISST_OUTSIDE;
		}

    }
       
}

void CEntryExitSensor::addEntity(IAIEntity* entity) {

	if (!entity) return;

	SEntryExitSensorData* data = new SEntryExitSensorData;
	data->Entity = entity;

	if (entity->getNode()->getTransformedBoundingBox().intersectsWithBox(Node->getTransformedBoundingBox()))
		data->State = EAISST_INSIDE;
	else
		data->State = EAISST_OUTSIDE;

	Entities.push_back(data);

}

} // end of IrrAI namespace
