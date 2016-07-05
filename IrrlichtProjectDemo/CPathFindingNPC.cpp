#include "CPathFindingNPC.h"

//#define NPC_DEBUG
#define ASTAR_SEARCH

namespace IrrAI {

CPathFindingNPC::CPathFindingNPC(const SNPCDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr, s32 id) : INPC(desc, aimgr, smgr, EAIET_PATHFINDINGNPC, id) {

	StateChangedCallbackPtr = NULL;
	EventCallbackPtr = NULL;
	PathDestinationWaypoint = NULL;
	CurrentWaypoint = NULL;
	DestinationWaypoint = NULL;

	// Try to get a pointer to the desired starting waypoint
	if (WaypointGroup && WaypointGroup->Waypoints.size() > 0) {
		if (desc.StartWaypointID == -1) CurrentWaypoint = aimgr->getWaypointFromId(WaypointGroup, rand()%(WaypointGroup->Waypoints.size()));
		else CurrentWaypoint = AIManager->getWaypointFromId(WaypointGroup, desc.StartWaypointID);
		if (CurrentWaypoint) {
			setPosition(CurrentWaypoint);
			PathDestinationWaypoint = CurrentWaypoint;
		} else printf("Couldn't place NPC (%i) at desired waypoint (%i)\n", id, desc.StartWaypointID);
	} else printf("Couldn't place NPC (%i) in desired waypoint group (%s)\n", id, desc.WaypointGroupName.c_str()); 
    
	#ifdef ASTAR_SEARCH
		PathFinder = AIManager->createAStarPathFinder();
	#else
		PathFinder = AIManager->createBreadthFirstPathFinder();
	#endif
	if (!PathFinder) printf("Failed PathFinder creation\n");

	// wait for a command before moving
	setStayPut(true);

}

CPathFindingNPC::~CPathFindingNPC() {
  
	if (PathFinder) {
		AIManager->removePathFinder(PathFinder);
		PathFinder = NULL;                
	}
              
}

void CPathFindingNPC::setDestination(IWaypoint* dest) {
     
	if (!dest /*|| dest == currentWaypoint*/) return;
    
	StayPut = false;
  
	#ifdef NPC_DEBUG
		if (CurrentWaypoint)
			printf("NPC (%i) Path from %i to %i\n", Node->getID(),CurrentWaypoint->getID(), dest->getID());
	#endif  
  
	// get the path to the destination
	if (PathFinder->findPath(CurrentWaypoint, dest, PathToDestination)) {
  
		PathDestinationWaypoint = dest;
    
		#ifdef NPC_DEBUG
			printf("REVERSE PATH (size %i)\n", PathToDestination.size());
			IWaypoint::printWaypointIDs(PathToDestination);
		#endif
  
		changeState(ENST_FOLLOWING_PATH); // ready to go now!
	} else printf("NPC (%i) FAILED TO FIND PATH\n", Node->getID());
  
}

void CPathFindingNPC::setDestination(s32 destIdx) {
  
	setDestination(AIManager->getWaypointFromId(WaypointGroup, destIdx));
   
}

void CPathFindingNPC::update(s32 elapsedTime) {
  
	IAIEntity::update(elapsedTime);
  
	#ifdef NPC_DEBUG
		printf("NPC (%i) Update nodes\n", node->getID());
	#endif

	switch (State) {
		case ENST_WAITING: {
			#ifdef NPC_DEBUG
				printf("NPC (%i) waiting\n", node->getID());
			#endif
			if (StayPut) {// If we want to stay put then keep waiting, whilst scanning the area
				return;
			} else {
				// If we've completed our path we return
				if (PathToDestination.size() == 0 && CurrentWaypoint == PathDestinationWaypoint) {
					sendEvent(ENET_AT_GOAL, NULL);
					changeState(ENST_WAITING);
				} else 
					changeState(ENST_FOLLOWING_PATH); // Keep on going!
			}
			break;    
		}
		case ENST_MOVING: {
			#ifdef NPC_DEBUG
				printf("NPC (%i) moving\n", node->getID());
			#endif
			if (DestinationWaypoint && !(Node->getAbsolutePosition().getDistanceFrom(DestinationWaypoint->getPosition() + NodeOffset) <= AtDestinationThreshold) ) {
				// Not at our destination yet so move a bit closer
				core::vector3df dir = ((DestinationWaypoint->getPosition() + NodeOffset) - Node->getAbsolutePosition()).normalize();
				f32 factor = elapsedTime * MoveSpeed;
				Node->setPosition(Node->getAbsolutePosition() + dir * factor);
				break; 
			} else { // reached destination, wait for what to do next
				//currentWaypoint = destinationWaypoint;
				changeState(ENST_WAITING);
			}
			break;    
		}
		case ENST_FOLLOWING_PATH: {
			#ifdef NPC_DEBUG
				printf("NPC (%i) following path\n", node->getID());
			#endif
			// If we're not at the current waypoint then go there (this can happen when stopping mid-path)
			if (DestinationWaypoint && !(Node->getPosition().getDistanceFrom(DestinationWaypoint->getPosition() + NodeOffset) <= AtDestinationThreshold) ) {
				DestinationWaypoint = CurrentWaypoint;
			} else {
				if (PathToDestination.size() == 0) {
					changeState(ENST_WAITING);
					break;
				}
				// Take the next waypoint in the path (the path is in reverse so we take from the back)
				DestinationWaypoint = PathToDestination[PathToDestination.size()-1];
				// remove the waypoint we've just taken
				PathToDestination.erase(PathToDestination.size()-1); 
			}
       
			if (DestinationWaypoint) {
				// Rotate to face new destination and set running animation
				rotateToFace(DestinationWaypoint->getPosition() + NodeOffset);     
				// Update current waypoint, that being the one we're aiming for.
				CurrentWaypoint = DestinationWaypoint;
				changeState(ENST_MOVING);
			} else changeState(ENST_WAITING);        
      
			break;    
		}     
	}
  
	#ifdef NPC_DEBUG
		printf("NPC (%i) updated\n", Node->getID());
	#endif
   
}

void CPathFindingNPC::setPosition(IWaypoint* w) {

	CurrentWaypoint = w;
	IAIEntity::setPosition(w->getPosition());
  
}

void CPathFindingNPC::writeOutXMLDescription(core::array<core::stringw>& names, core::array<core::stringw>& values) const {
    
	IAIEntity::writeOutXMLDescription(names, values);
  
	core::stringw strw;
	names.push_back(core::stringw(L"waypointGroupName"));
	values.push_back(core::stringw(WaypointGroup->getName()).c_str());
	names.push_back(core::stringw(L"startWaypointID"));
	strw = (CurrentWaypoint ? core::stringw(CurrentWaypoint->getID()) : core::stringw(0));
	values.push_back(strw);
	names.push_back(core::stringw(L"moveSpeed"));
	strw = core::stringw(MoveSpeed);
	values.push_back(strw);
	names.push_back(core::stringw(L"atDestinationThreshold"));
	strw = core::stringw(AtDestinationThreshold);
	values.push_back(strw);
         
}

} // end of IrrAI namespace
