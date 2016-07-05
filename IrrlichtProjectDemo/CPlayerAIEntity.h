#ifndef INC_CPLAYERAIENTITY_H
#define INC_CPLAYERAIENTITY_H

#include "IPlayerAIEntity.h"

namespace IrrAI {

class CPlayerAIEntity : public IPlayerAIEntity {

	public:    
		CPlayerAIEntity(const SAIEntityDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr, s32 id);
                          
};

} // end of IrrAI namespace

#endif /* INC_CPLAYERAIENTITY_H */
