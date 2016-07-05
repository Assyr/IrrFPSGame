#include "CBreadthFirstPathFinder.h"

namespace IrrAI {

bool CBreadthFirstPathFinder::findPath(IWaypoint* startNode, IWaypoint* goalNode, core::array<IWaypoint*>& path) {

	if (!startNode || !goalNode) return false;
  
	core::array<SSearchNode*> visited;
	core::array<SSearchNode*> queue;
	bool found = false;
  
	queue.push_back(new SSearchNode(NULL, startNode));
            
	while (queue.size() != 0) {
            
		// Get the waypoint at the front of the queue
		SSearchNode* sNode = queue[0];
		queue.erase(0);
		visited.push_back(sNode);
            
		// If it's the goal then generate the path to it and return it
		if (sNode->Waypoint->equals(goalNode)) {
			found = getPath(sNode, path);
			break;
		}
                  
		// Otherwise add all the waypoints children to the queue
		core::list<SNeighbour*>::ConstIterator iter = sNode->Waypoint->getNeighbours().begin();
		while (iter != sNode->Waypoint->getNeighbours().end()) {
			if (!contains(visited, (*iter)->Waypoint) && !contains(queue, (*iter)->Waypoint)) 
			queue.push_back(new SSearchNode(sNode, (*iter)->Waypoint));
			++iter;
		}
     
	}
  
	deleteSearchNodes(visited);
	deleteSearchNodes(queue);
          
	return found; 
        
}

} // end of IrrAI namespace
