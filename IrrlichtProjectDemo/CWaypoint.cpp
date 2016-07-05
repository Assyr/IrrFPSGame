#include "CWaypoint.h"

namespace IrrAI {

CWaypoint::CWaypoint(s32 id, const core::vector3df& position) : IWaypoint(id, position) {
	
}

void CWaypoint::addNeighbour(IWaypoint* w) {

  if (!w) return;
  
  if (!contains(Neighbours, w)) {
    core::stringc currStr = NeighbourString;
    core::stringc idStr(w->getID());
    if (currStr.find(idStr.c_str()) == -1) {
      // Update waypoint's name to include the new neighbour
      if (currStr.size() > 0) currStr += ',';
      currStr += idStr;
      //printf("NN: %s\n", curr_name.c_str());
      NeighbourString = currStr.c_str();
    }
    SNeighbour* n = new SNeighbour();
    if (n) {
      n->Waypoint = w;
      n->Distance = Position.getDistanceFrom(w->getPosition());
      Neighbours.push_back(n);   
    }
  }
        
}

void CWaypoint::removeNeighbour(IWaypoint* w) {
  
  if (!w) return;

  core::list<SNeighbour*>::Iterator iter = Neighbours.begin();
  while (iter != Neighbours.end()) {
    if ((*iter)->Waypoint->getID() == w->getID()) {
      core::stringc currStr = NeighbourString;
      core::stringc idStr(w->getID());
      s32 idx = currStr.find(idStr.c_str());
      if (idx != -1) {
        if (idx > 0) {
          idx--;
          currStr.erase(idx); // erase the preceeding comma
        }
        for (u32 i = 0 ; i < idStr.size() ; ++i) currStr.erase(idx); // erase the id (could be multi-digit number, hence for loop)
        if (idx == 0) currStr.erase(idx); // erase the following comma
      }
      //printf("%i NN: %s\n", id, currName.c_str());
      NeighbourString = currStr.c_str();
      iter = Neighbours.erase(iter);
      return;
    } else ++iter;
  }
  
}

bool CWaypoint::hasNeighbour(IWaypoint* w) const {
  
	return contains(Neighbours, w);
     
}

} // end of IrrAI namespace
