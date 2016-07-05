#include "CAIManager.h"
#include "CIrrAIFileParser.h"
#include "CIrrAIFileWriter.h"
#include "CConeFieldOfView.h"
#include "CDebugConeFOVSceneNode.h"
#include "CAStarPathFinder.h"
#include "CBreadthFirstPathFinder.h"
#include "CEntryExitSensor.h"
#include "IrrAI.h"
#include "CWaypoint.h"
#include "CCombatNPC.h"
#include "CPathFindingNPC.h"
#include "CPlayerAIEntity.h"
#include "CWaypointMeshSceneNode.h"
#include <time.h>
  
//#define MANAGER_DEBUG

namespace IrrAI {
  
bool SWaypointGroup::contains(IWaypoint* waypoint) {

	return (IWaypoint::contains(Waypoints, waypoint));  
     
}

void SWaypointGroup::setColour(const video::SColor& colour) {
     
	Colour = colour;
	if (WaypointMeshNode) ((CWaypointMeshSceneNode*)WaypointMeshNode)->setColour(Colour);  
      
}

void SWaypointGroup::setSize(s32 size) {
     
	WaypointSize = size;
	if (WaypointMeshNode) ((CWaypointMeshSceneNode*)WaypointMeshNode)->setWaypointSize(WaypointSize);  
      
}


CAIManager::CAIManager(IrrlichtDevice* device) : IAIManager(device){
                     
	printf("IrrAI library version %.2f\n", IRRAI_VERSION); 
                       
	NextEntityID = 0;
	NumWaypoints = 0;
	OcclusionQueryCallbackPtr = NULL;

	srand((u32)time(NULL));

	reinit();

}

CAIManager::~CAIManager() {

	clear();
  
}

void CAIManager::reinit() {
	
	DebugRootNode = SceneManager->addEmptySceneNode();
	DebugRootNode->setVisible(DebugVisible);
	DebugRootNode->grab();

}

void CAIManager::update(s32 elapsedTime) {
  
	// Reset entities
	for (u32 i = 0 ; i < Entities.Entities.size() ; ++i) 
		Entities.Entities[i]->reset();
    
	// Update entities
	for (u32 i = 0 ; i < Entities.Entities.size() ; ++i) 
		Entities.Entities[i]->update(elapsedTime);
           
}

void CAIManager::clear() {
 
	for (u32 i = 0 ; i < Entities.Entities.size() ; ++i) 
		delete Entities.Entities[i];
	Entities.Entities.clear();
  
	for (u32 i = 0 ; i < EntityGroups.size() ; ++i) {
		delete EntityGroups[i];    
	}
	EntityGroups.clear();
  
	for (u32 wg = 0 ; wg < WaypointGroups.size() ; ++wg) { 
		if (WaypointGroups[wg]->WaypointMeshNode) {
			WaypointGroups[wg]->WaypointMeshNode->remove();
			WaypointGroups[wg]->WaypointMeshNode->drop();
			WaypointGroups[wg]->WaypointMeshNode = NULL;                      
		}
		// Delete all waypoints in the group
		for (u32 w = 0 ; w < WaypointGroups[wg]->Waypoints.size() ; ++w) {
			removeWaypoint(WaypointGroups[wg], WaypointGroups[wg]->Waypoints[w]);
			w--;
		}
		// Delete the group
		delete WaypointGroups[wg];   
	}
	WaypointGroups.clear();
	NumWaypoints = 0;

	if (DebugRootNode) {
		DebugRootNode->remove();
		DebugRootNode->drop();
		DebugRootNode = NULL;
	}
      
}

bool CAIManager::loadAI(const c8* fileName) {
     
	CIrrAIFileParser fileParser;
	bool ret = fileParser.parseXML(this, fileName);
  
	printf("Loaded: %i Waypoint Groups, %i Waypoints, %i Entities\n", WaypointGroups.size(), NumWaypoints, getNumEntities());

	return ret;
    	    
}

void CAIManager::linkWaypoints(SWaypointGroup* group) {
   
	if (!group) return;
   
	//group->waypoints.sort();
     
	// Connect up the waypoints with each other
	// Iterate through each waypoint and read its neighbour list from its name
	for (u32 i = 0 ; i < group->Waypoints.size() ; ++i) {
		CWaypoint* mainWaypoint = (CWaypoint*)group->Waypoints[i];
		const c8* name = mainWaypoint->getNeighbourString();
		const c8* ptr = &name[0]; // Start of neighbour list
		IWaypoint* neighbourWaypoint = NULL;
		while (ptr[0] != '\0') {
			s32 id = atoi(ptr);
			if (id != 0 || (id == 0 && ptr[0] == '0')) {
				neighbourWaypoint = getWaypointFromId(group, id);
				//printf("WYPT %i linked to WYPT %i\n", main_waypoint->getID(), neighbour_waypoint->getID());
				if (neighbourWaypoint) mainWaypoint->addNeighbour(neighbourWaypoint);   
			}
			while (ptr[0] != ',') 
				if (ptr[0]=='\0') // list completed
					break;
				else // number
					++ptr;
			// Skip comma or list completed if ;
			if (ptr[0]=='\0')
				 break;
			else
				++ptr; // skip comma
		}   
	}
   
}

bool CAIManager::saveAI(const c8* fileName) {
  
	if (!Device) return false;

	CIrrAIFileWriter fileWriter(Device->getFileSystem());
	return fileWriter.writeToXML(this, fileName); 
	//return fileWriter.writeToFile(this, fileName); 
     
}

IWaypoint* CAIManager::getWaypointFromId(SWaypointGroup* group, s32 id) {

	return group->Waypoints[id];

}

ICombatNPC* CAIManager::createCombatNPC(const SCombatNPCDesc& desc) {

	CCombatNPC* npc = new CCombatNPC(desc, this, SceneManager, NextEntityID++);
	Entities.addEntity(npc);
	return npc;
  
}

INPC* CAIManager::createPathFindingNPC(const SNPCDesc& desc) {

	CPathFindingNPC* npc = new CPathFindingNPC(desc, this, SceneManager, NextEntityID++);
	Entities.addEntity(npc);
	return npc;

}

IPlayerAIEntity* CAIManager::createPlayerAIEntity(const SAIEntityDesc& desc) {
  
	CPlayerAIEntity* paie = new CPlayerAIEntity(desc, this, SceneManager, NextEntityID++);
	Entities.addEntity(paie);
	return paie;
           
}

SEntityGroup* CAIManager::createEntityGroup() {

	SEntityGroup* group = new SEntityGroup();
	EntityGroups.push_back(group);
	return group;
            
}

IWaypoint* CAIManager::createWaypoint(SWaypointGroup* group, const core::vector3df& pos) {
  
	return createWaypoint(group, group->Waypoints.size(), pos);
  
}

IWaypoint* CAIManager::createWaypoint(SWaypointGroup* group, s32 id, const core::vector3df& pos) {

	if (!group) return NULL;
  
	CWaypoint* wypt = new CWaypoint(id, pos);
	if (wypt) {
		group->Waypoints.push_back(wypt);
		++NumWaypoints;
	}
	if (group->WaypointMeshNode) ((CWaypointMeshSceneNode*)group->WaypointMeshNode)->addWaypoint(wypt);
  
	return wypt;
  
}

void CAIManager::removeWaypoint(SWaypointGroup* group, IWaypoint* waypoint) {

	if (!group || !waypoint) return;

	for (u32 i = 0 ; i < group->Waypoints.size() ; ++i) 
		if (group->Waypoints[i] == waypoint) {
			group->Waypoints.erase(i);
			waypoint->remove();
			NumWaypoints--;
			i--;         
		} else group->Waypoints[i]->removeNeighbour(waypoint); // This takes care of cleaning up any waypoints that link to the specified waypoint but which the specified waypoint doesn't link to    

	delete waypoint;
  
	if (group->WaypointMeshNode) createDebugWaypointMesh(group);
       
}

void CAIManager::removeWaypointGroup(SWaypointGroup* group) {
 
	if (!group) return;
    
	for (u32 wg = 0 ; wg < WaypointGroups.size() ; ++wg) 
		if (WaypointGroups[wg] == group) {
			#ifdef MANAGER_DEBUG
				printf("remove waypoint mesh\n");
			#endif
			if (group->WaypointMeshNode) {
				group->WaypointMeshNode->remove();
				group->WaypointMeshNode->drop();
				group->WaypointMeshNode = NULL;                      
			}
			// Remove all waypoints in the group
			#ifdef MANAGER_DEBUG
				printf("remove waypoints\n");
			#endif
			for (u32 i = 0 ; i < group->Waypoints.size() ; ++i) {
				removeWaypoint(group, group->Waypoints[i]);
				NumWaypoints--;
			}
			// Remove the group from the list
			#ifdef MANAGER_DEBUG
				printf("erase group\n");
			#endif
			WaypointGroups.erase(wg);
			// Delete the group
			#ifdef MANAGER_DEBUG
				printf("delete group\n");
			#endif
			delete group;
			#ifdef MANAGER_DEBUG
				printf("waypoint group removed\n");
			#endif
			break;          
		} 
   
}

SWaypointGroup* CAIManager::createWaypointGroup() {
  
	SWaypointGroup* newGroup = new SWaypointGroup();
	WaypointGroups.push_back(newGroup);
	return newGroup;     
                             
}

SWaypointGroup* CAIManager::getWaypointGroupFromName(const c8* name) {

	for (u32 i = 0 ; i < WaypointGroups.size() ; ++i) 
		if (strcmp(WaypointGroups[i]->getName(), name) == 0)
			return WaypointGroups[i];              
   
  return NULL;
                  
}

void CAIManager::removeAIEntity(IAIEntity* entity) {
  
	for (u32 i = 0 ; i < Entities.Entities.size() ; ++i) 
		if (Entities.Entities[i] == entity) {
			delete entity;
			Entities.Entities.erase(i);
			break;
		}
     
}

void CAIManager::setDebugVisible(bool val) {

	DebugVisible = val;
	DebugRootNode->setVisible( DebugVisible );
    
}

void CAIManager::createDebugWaypointMesh(SWaypointGroup* group) {

	if (!group) return;

	if (!group->WaypointMeshNode) {
		group->WaypointMeshNode = new CWaypointMeshSceneNode(DebugRootNode, SceneManager, -1, group->Colour, group->WaypointSize);
	}
	CWaypointMeshSceneNode* meshNode = (CWaypointMeshSceneNode*)group->WaypointMeshNode;
	//meshNode->setVisible(DebugVisible);
	meshNode->clear();

	for (u32 i = 0 ; i < group->Waypoints.size() ; ++i) 
		meshNode->addWaypoint(group->Waypoints[i]); 
	meshNode->recalculateBoundingBox();    

}

void CAIManager::createDebugWaypointMeshes() {
   
	for (u32 i = 0 ; i < WaypointGroups.size() ; ++i) 
		createDebugWaypointMesh(WaypointGroups[i]);
        
}

void CAIManager::removeDebugWaypointMeshes() {
   
	for (u32 i = 0 ; i < WaypointGroups.size() ; ++i) 
		if (WaypointGroups[i]->WaypointMeshNode) {
			WaypointGroups[i]->WaypointMeshNode->remove();
			WaypointGroups[i]->WaypointMeshNode->drop();
			WaypointGroups[i]->WaypointMeshNode = NULL;
		}      
}

IWaypoint* CAIManager::getNearestWaypoint(SWaypointGroup* group, core::vector3df pos) {
        
	if (!group) return NULL;
                
	if (group->Waypoints.size() == 0) return NULL; // empty group
  
	IWaypoint* nearestWaypoint = group->Waypoints[0];
	f32 nearestDistance = nearestWaypoint->getPosition().getDistanceFrom(pos);
  
	for (u32 i = 1 ; i < group->Waypoints.size() ; ++i) {
		f32 dist = group->Waypoints[i]->getPosition().getDistanceFrom(pos);
		if (dist < nearestDistance) {
			nearestWaypoint = group->Waypoints[i];
			nearestDistance = dist;
		}
	}
  
	return nearestWaypoint; 
                    
}

IAIEntity* CAIManager::getEntityFromName(const wchar_t* name) {
  
	for (u32 i = 0 ; i < Entities.Entities.size() ; ++i) 
		if (wcscmp(Entities.Entities[i]->getName(), name) == 0)
			return Entities.Entities[i];   
	  
	return NULL;
           
}

IFieldOfView* CAIManager::createConeFieldOfView(const core::vector3df& dim, bool occlusionCheck) {
  
	return new CConeFieldOfView(this, occlusionCheck, dim);
              
}

void CAIManager::removeFieldOfView(IFieldOfView* fov) {
  
	delete fov;

}

IDebugFOVSceneNode* CAIManager::createDebugConeFOVSceneNode(const core::vector3df& dim) {
  
	return new CDebugConeFOVSceneNode(DebugRootNode, SceneManager, -1, dim);
                   
}

void CAIManager::removeDebugFOVScenNode(IDebugFOVSceneNode* fovNode) {

	fovNode->remove();
	fovNode->drop();

}

IPathFinder* CAIManager::createAStarPathFinder() {
  
	return new CAStarPathFinder();
             
}
    
IPathFinder* CAIManager::createBreadthFirstPathFinder() {
             
	return new CBreadthFirstPathFinder();
             
}

void CAIManager::removePathFinder(IPathFinder* pathFinder) {

	delete pathFinder;

}

IAISensor* CAIManager::createEntryExitSensor(const SAIEntityDesc& desc) {
  
	IAISensor* sensor = new CEntryExitSensor(desc, this, SceneManager, NextEntityID++);
	if (sensor) Entities.Entities.push_back(sensor);
  
	return sensor;
           
}

void CAIManager::removeSensor(IAISensor* sensor) {

	delete sensor;

}

} // end of IrrAI namespace
