#include "CAStarPathFinder.h"

namespace IrrAI {

bool CAStarPathFinder::findPath(IWaypoint* startNode, IWaypoint* goalNode, core::array<IWaypoint*>& path) {
    
	if (!startNode || !goalNode) return false;
    
	core::array<SSearchNode*> lstOpen, lstClosed;
	SAStarSearchNode* sNode = NULL;

	// add start point to open list
	lstOpen.push_back(new SAStarSearchNode(NULL, startNode));
  
	bool pFound = false;
	while ((lstOpen.size() > 0) && !pFound) {
		// get first waypoint from open list
		sNode = (SAStarSearchNode*)lstOpen[0];
		if (sNode->Waypoint == goalNode) {
			pFound = getPath(sNode, path);
			break;
		}
		// move waypoint from open list to closed list
		lstClosed.push_front(sNode);
		lstOpen.erase(0);
		// add all now reachable waypoints to open list

		core::list<SNeighbour*>::ConstIterator iter = sNode->Waypoint->getNeighbours().begin();
		while (iter != sNode->Waypoint->getNeighbours().end()) {
			SAStarSearchNode* newSNode = new SAStarSearchNode(sNode, (*iter)->Waypoint);
			newSNode->CostH = (*iter)->Distance;//(long)sNode->waypoint->getPosition().getDistanceFrom(newSNode->waypoint->getPosition());
			newSNode->CostG = sNode->CostG + (*iter)->Distance;
			newSNode->CostF = newSNode->CostG + newSNode->CostH;
			// add to open list (position in array depends on costF)
			s32 pInsert = 0, pExist = -1;
			// is new in closed list !?!?!
			for (u32 r = 0; r < lstClosed.size(); ++r) {
				if (lstClosed[r]->Waypoint == newSNode->Waypoint) {
					pInsert = -1;
					break;
				}
			}
			if (pInsert >= 0) {
				// check for position and existance in open list
				for (u32 r = 0; r < lstOpen.size(); ++r) {
					// way point already in open list !!!
					if (lstOpen[r]->Waypoint == newSNode->Waypoint) pExist = r;
					// position for insert
					if (((SAStarSearchNode*)lstOpen[r])->CostF < newSNode->CostF) pInsert = r + 1;
				}
				if (pExist >= 0) {
					// erase existing in open list if it has more costF !!!
					if (((SAStarSearchNode*)lstOpen[pExist])->CostF > newSNode->CostF) {
						lstOpen.erase(pExist);
					} else {
						// do not add to open list
						pInsert = -1;
					}
				}
				// add as new to open list
				if (pInsert >= 0) {
					// is new waypoint = destination, then path was found !!!
					if (newSNode->Waypoint == goalNode) {
						pFound = getPath(newSNode, path);
						lstClosed.push_front(newSNode);
						break;
					} else {
						if ((u32)pInsert >= lstOpen.size()) {
							lstOpen.push_back(newSNode);
						} else {
							lstOpen.insert(newSNode, pInsert);
						}
					}
				}
			}
			++iter;
		}
	}
  
	deleteSearchNodes(lstOpen);
	deleteSearchNodes(lstClosed);
          
	return pFound; 
                   
}

} // end of IrrAI namespace
