#ifndef INC_CCOMBATNPC_H
#define INC_CCOMBATNPC_H

#define MOVEMENT_SPEED 0.1f

#include "ICombatNPC.h"

namespace IrrAI {

class CCombatNPC : public ICombatNPC {

	public:    
		CCombatNPC(const SCombatNPCDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr, s32 id);
		~CCombatNPC();
		void setDestination(IWaypoint* dest);
		void setDestination(s32 destIdx);
		void update(s32 elapsedTime);
		bool isVisibleToNPC(IAIEntity* entity);
		void setPosition(IWaypoint* w);
		void writeOutXMLDescription(core::array<core::stringw>& names, core::array<core::stringw>& values) const;
                                
	private:        
		void checkFieldOfVision();

};

} // end of IrrAI namespace

#endif /* INC_CCOMBATNPC_H */
