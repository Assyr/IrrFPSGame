#ifndef INC_CBREADTHFIRSTPATHFINDER_H
#define INC_CBREADTHFIRSTPATHFINDER_H

#include "IPathFinder.h"

namespace IrrAI {

class CBreadthFirstPathFinder : public IPathFinder {

	public:
		CBreadthFirstPathFinder() {};
		bool findPath(IWaypoint* startNode, IWaypoint* goalNode, core::array<IWaypoint*>& path);
          
};

} // end of IrrAI namespace
      
#endif /* INC_CBREADTHFIRSTPATHFINDER_H */
