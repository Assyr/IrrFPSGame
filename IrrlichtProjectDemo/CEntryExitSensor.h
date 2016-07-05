#ifndef INC_CENTRYEXITSENSOR_H
#define INC_CENTRYEXITSENSOR_H

#include "IAISensor.h"

namespace IrrAI {

class CEntryExitSensor : public IAISensor {
  
	public:
		CEntryExitSensor(const SAIEntityDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr, s32 id);
		void update(s32 elapsedTime);  
		void addEntity(IAIEntity* entity);

	private:
		enum E_AISENSOR_STATE_TYPE {
			EAISST_INSIDE,
			EAISST_OUTSIDE,
			E_NUM_AISENSOR_STATE_TYPES
		};

		struct SEntryExitSensorData : public SSensorData {
			E_AISENSOR_STATE_TYPE State;
		};
      
};

} // end of IrrAI namespace

#endif /* INC_CENTRYEXITSENSOR_H */
