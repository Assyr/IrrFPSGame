#include "CCombatNPC.h"
#include "CConeFieldOfView.h"

//#define NPC_DEBUG
#define ASTAR_SEARCH // undefine me to use breadth first search

#define FOV_ROTATION core::vector3df(0,0,0) // -90

namespace IrrAI {

CCombatNPC::CCombatNPC(const SCombatNPCDesc& desc, IAIManager* aimgr, scene::ISceneManager* smgr, s32 id) : ICombatNPC(desc, aimgr, smgr, id) {

	PathDestinationWaypoint = NULL;
	FieldOfView = NULL;
	DebugFOV = NULL;
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
  
	// Add a FOV
	FieldOfView = AIManager->createConeFieldOfView(desc.FovDimensions, desc.FovOcclusionCheck);
	if (FieldOfView && Node) {      
		FieldOfView->setPosition(Node->getAbsolutePosition());
		FieldOfView->setRotation(Node->getRotation() + FOV_ROTATION);
	} else printf("Failed FOV creation\n");
  
	// Add a FOV scene node to represent it
	DebugFOV = AIManager->createDebugConeFOVSceneNode(desc.FovDimensions);
	if (DebugFOV && Node) {
		DebugFOV->setPosition(Node->getAbsolutePosition());
		DebugFOV->setRotation(Node->getRotation() + FOV_ROTATION);
	} else printf("Failed D-FOV-SN creation\n");
  
	#ifdef ASTAR_SEARCH
		PathFinder = AIManager->createAStarPathFinder();
	#else
		PathFinder = AIManager->createBreadthFirstPathFinder();
	#endif
	if (!PathFinder) printf("Failed PathFinder creation\n");

	// wait for a command before moving
	setStayPut(true);

}

CCombatNPC::~CCombatNPC() {
      
	if (FieldOfView) {
		AIManager->removeFieldOfView(FieldOfView);
		FieldOfView = NULL;
	}
  
	if (DebugFOV) {
		AIManager->removeDebugFOVScenNode(DebugFOV);
		DebugFOV = NULL;               
	}
  
	if (PathFinder) {
		AIManager->removePathFinder(PathFinder);
		PathFinder = NULL;                
	}
              
}

void CCombatNPC::setDestination(IWaypoint* dest) {
     
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

void CCombatNPC::setDestination(s32 destIdx) {
  
	setDestination(AIManager->getWaypointFromId(WaypointGroup, destIdx));
   
}

void CCombatNPC::update(s32 elapsedTime) {
  
	IAIEntity::update(elapsedTime);
  
	#ifdef NPC_DEBUG
		printf("NPC (%i) Update nodes\n", node->getID());
	#endif

	if (FieldOfView) {      
		FieldOfView->setPosition(Node->getAbsolutePosition());
		FieldOfView->setRotation(Node->getRotation() + FOV_ROTATION);
	}
  
	if (DebugFOV) {
		DebugFOV->updateAbsolutePosition();
		DebugFOV->setPosition(Node->getAbsolutePosition());
		DebugFOV->setRotation(Node->getRotation() + FOV_ROTATION);
	}  
  
	#ifdef NPC_DEBUG
		printf("NPC (%i) check FOV\n", Node->getID());
	#endif
	checkFieldOfVision();

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

/* Check the NPC's FOV for enemies
 *
 */
void CCombatNPC::checkFieldOfVision() {
  
	if (EnemyGroup && CheckFovForEnemies) { // Check for enemies in FOV
  
		for (u32 e = 0 ; e < EnemyGroup->Entities.size() ; ++e) {
			IAIEntity* enemy = EnemyGroup->Entities[e];
			if (enemy != this && isVisibleToNPC(enemy)) {
				enemy->setVisibleToOtherEntity(true);
				core::vector3df enemyPos = enemy->getAbsolutePosition();
				if (AllyGroup) {
					// Alert allies to enemy's position
					for (u32 a = 0 ; a < AllyGroup->Entities.size() ; ++a) {
						if (AllyGroup->Entities[a] != this && AllyGroup->Entities[a]->getType() == EAIET_COMBATNPC ||
							AllyGroup->Entities[a] != this && AllyGroup->Entities[a]->getType() == EAIET_PATHFINDINGNPC)
							((INPC*)AllyGroup->Entities[a])->sendEventToNPC(ENET_ENEMY_POSITION_KNOWN, &enemyPos);
					}
				}
				// Check whether the enemy is in range or just visible
				if (Node->getAbsolutePosition().getDistanceFrom(enemyPos) <= Range)
					sendEvent(ENET_ENEMY_IN_RANGE, enemy);
				else
					sendEvent(ENET_ENEMY_VISIBLE, enemy);
			}
		}
  
	}
  
	if (AllyGroup && CheckFovForAllies) { // Check for allies in FOV
  
		for (u32 a = 0 ; a < AllyGroup->Entities.size() ; ++a) {
			IAIEntity* ally = AllyGroup->Entities[a];
			if (ally != this && isVisibleToNPC(ally)) {
				ally->setVisibleToOtherEntity(true);
				// Check whether the ally is in range or just visible
				if (Node->getAbsolutePosition().getDistanceFrom(ally->getAbsolutePosition()) <= Range)
					sendEvent(ENET_ALLY_IN_RANGE, ally);
				else
					sendEvent(ENET_ALLY_VISIBLE, ally);
			}
		}
  
	}
            
}

bool CCombatNPC::isVisibleToNPC(IAIEntity* entity) {
  
	if (!FieldOfView || !entity) return false;
  
	core::vector3df entityPos = entity->getAbsolutePosition();  
	if (!FieldOfView->isInFOV(entity->getNode()->getTransformedBoundingBox(), entityPos)) return false;
        
	return true;  
        
}

void CCombatNPC::setPosition(IWaypoint* w) {

	CurrentWaypoint = w;
	IAIEntity::setPosition(w->getPosition());
    
	if (FieldOfView) {      
		FieldOfView->setPosition(Node->getAbsolutePosition());
		FieldOfView->setRotation(Node->getRotation() + core::vector3df(0,-90,0));
	}
  
	if (DebugFOV) {
		DebugFOV->updateAbsolutePosition();
		DebugFOV->setPosition(Node->getAbsolutePosition());
		DebugFOV->setRotation(Node->getRotation() + core::vector3df(0,-90,0));
	}  
  
}

void CCombatNPC::writeOutXMLDescription(core::array<core::stringw>& names, core::array<core::stringw>& values) const {
    
	IAIEntity::writeOutXMLDescription(names, values);
  
	core::stringw strw;
	names.push_back(core::stringw(L"waypointGroupName"));
	values.push_back(core::stringw(WaypointGroup->getName()).c_str());
	names.push_back(core::stringw(L"startWaypointID"));
	strw = (CurrentWaypoint ? core::stringw(CurrentWaypoint->getID()) : core::stringw(0));
	values.push_back(strw);
	names.push_back(core::stringw(L"fovDimensions"));
	core::vector3df dim = (FieldOfView ? FieldOfView->getDimensions() : core::vector3df(0,0,0));
	strw = core::stringw(dim.X);
	strw += ",";
	strw += dim.Y;
	strw += ",";
	strw += dim.Z;
	values.push_back(strw);
	names.push_back(core::stringw(L"range"));
	strw = core::stringw(Range);    
	values.push_back(strw);
	names.push_back(core::stringw(L"moveSpeed"));
	strw = core::stringw(MoveSpeed);
	values.push_back(strw);
	names.push_back(core::stringw(L"atDestinationThreshold"));
	strw = core::stringw(AtDestinationThreshold);
	values.push_back(strw);
	names.push_back(core::stringw(L"fovOcclusionCheck"));
	strw = core::stringw(FovOcclusionCheck);
	values.push_back(strw);
	names.push_back(core::stringw(L"checkFovForEnemies"));
	strw = core::stringw(CheckFovForEnemies);
	values.push_back(strw);
	names.push_back(core::stringw(L"checkFovForAllies"));
	strw = core::stringw(CheckFovForAllies);
	values.push_back(strw);
         
}

} // end of IrrAI namespace
