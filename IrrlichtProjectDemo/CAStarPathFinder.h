#ifndef INC_CASTARPATHFINDER_H
#define INC_CASTARPATHFINDER_H

#include "IPathFinder.h"

namespace IrrAI {

struct SAStarSearchNode : public SSearchNode {
    SAStarSearchNode (SSearchNode* p, IWaypoint* w) : SSearchNode(p, w) {
		CostG = 0;
        CostH = 0;
        CostF = 0;
    } 
    f32 CostG; // The actual shortest distance traveled from the start node to this node
    f32 CostH; // The estimated/heuristic distance from this node to the goal node
    f32 CostF; // The sum of costG and costH
};

class CAStarPathFinder : public IPathFinder {

	public:
		CAStarPathFinder() {};
		bool findPath(IWaypoint* startNode, IWaypoint* goalNode, core::array<IWaypoint*>& path);
      
};

} // end of IrrAI namespace

#endif /* INC_CASTARPATHFINDER_H */
