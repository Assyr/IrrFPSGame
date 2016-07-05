#ifndef INC_CWAYPOINT_H
#define INC_CWAYPOINT_H

#include "IWaypoint.h"

namespace IrrAI {

class CWaypoint : public IWaypoint {
      
	public:
		CWaypoint(s32 id, const core::vector3df& position = core::vector3df(0,0,0));
						
		void addNeighbour(IWaypoint* w);
        void removeNeighbour(IWaypoint* w);
        bool hasNeighbour(IWaypoint* w) const;

        void setID(s32 id) { ID = id; }
		void setNeighbourString(const char* str) { NeighbourString = str; }
		const char* getNeighbourString() { return NeighbourString.c_str(); }
           
};

} // end of IrrAI namespace

#endif /* INC_CWAYPOINT_H */
