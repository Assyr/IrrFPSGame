#ifndef INC_CPATHFINDINGNPC_H
#define INC_CPATHFINDINGNPC_H

#define MOVEMENT_SPEED 0.1f

#include "INPC.h"

namespace IrrAI { 

class CPathFindingNPC : public INPC {

	public:    
		CPathFindingNPC(const SNPCDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr, s32 id);
		~CPathFindingNPC();
		void setDestination(IWaypoint* dest);
		void setDestination(s32 destIdx);
		void update(s32 elapsedTime);
		void setPosition(IWaypoint* w);
		void writeOutXMLDescription(core::array<core::stringw>& names, core::array<core::stringw>& values) const;
                                            
};

} // end of IrrAI namespace

#endif /* INC_CPATHFINDINGNPC_H */
