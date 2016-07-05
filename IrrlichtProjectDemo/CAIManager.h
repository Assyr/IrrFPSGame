#ifndef INC_CAIMANAGER_H
#define INC_CAIMANAGER_H

#include "IAIManager.h"

namespace IrrAI {

/*! Class to manage AI entities */
class CAIManager : public IAIManager {

	public:
		CAIManager(IrrlichtDevice* device); 
		~CAIManager(); 
		void reinit();
		void update(s32 elapsedTime); 
		void clear();
		bool loadAI(const c8* fileName); 
		void linkWaypoints(SWaypointGroup* group);
		bool saveAI(const c8* fileName); 
		IWaypoint* getWaypointFromId(SWaypointGroup* group, s32 id); 
		ICombatNPC* createCombatNPC(const SCombatNPCDesc& desc); 
		INPC* createPathFindingNPC(const SNPCDesc& desc);
		IPlayerAIEntity* createPlayerAIEntity(const SAIEntityDesc& desc);
		SEntityGroup* createEntityGroup(); 
		IWaypoint* createWaypoint(SWaypointGroup* group, const core::vector3df& pos = core::vector3df(0,0,0)); 
		IWaypoint* createWaypoint(SWaypointGroup* group, s32 id, const core::vector3df& pos = core::vector3df(0,0,0)); 
		void removeWaypoint(SWaypointGroup* group, IWaypoint* waypoint); 
		void removeWaypointGroup(SWaypointGroup* group); 
		SWaypointGroup* createWaypointGroup(); 
		SWaypointGroup* getWaypointGroupFromName(const c8* name); 
		void removeAIEntity(IAIEntity* entity); 
		void setDebugVisible(bool val); 
		void createDebugWaypointMesh(SWaypointGroup* group);
		void createDebugWaypointMeshes(); 
		void removeDebugWaypointMeshes();
		IWaypoint* getNearestWaypoint(SWaypointGroup* group, core::vector3df pos);
		IAIEntity* getEntityFromName(const wchar_t* name);
		IFieldOfView* createConeFieldOfView(const core::vector3df& dim, bool occlusionCheck);
		void removeFieldOfView(IFieldOfView* fov);
		IDebugFOVSceneNode* createDebugConeFOVSceneNode(const core::vector3df& dim);
		void removeDebugFOVScenNode(IDebugFOVSceneNode* fovNode);
		IPathFinder* createAStarPathFinder();
		IPathFinder* createBreadthFirstPathFinder();
		void removePathFinder(IPathFinder* pathFinder);
		IAISensor* createEntryExitSensor(const SAIEntityDesc& desc);
		void removeSensor(IAISensor* sensor);

		inline s32 getNumWaypoints() const { return NumWaypoints; }
		inline s32 getNumEntities() const { return Entities.Entities.size(); }
		inline SWaypointGroup* getWaypointGroupFromIndex(u32 idx) { return WaypointGroups[idx]; } 
		inline const core::array<SWaypointGroup*>& getWaypointGroups() { return WaypointGroups; } 
		inline const SEntityGroup* getEntities() const { return &Entities; }
		inline bool isDebugVisible() const { return DebugVisible; } 
		inline void setOcclusionQueryCallback(bool (*cb)(const core::line3df& ray)) { OcclusionQueryCallbackPtr = cb; } 
		inline bool occlusionQuery(const core::line3df& ray) {
			if (OcclusionQueryCallbackPtr) return OcclusionQueryCallbackPtr(ray);
			else                           return false;
		} 
		inline void registerAIEntity(IAIEntity* entity){
			Entities.addEntity(entity);   
		}
		inline scene::ISceneNode* getDebugRootNode() { return DebugRootNode; }
            
	private:
		core::array<SWaypointGroup*> WaypointGroups;
		core::array<SEntityGroup*> EntityGroups;
		SEntityGroup Entities;
		s32 NextEntityID;
		s32 NumWaypoints;
		s32 NumEntities;
		scene::ISceneNode* DebugRootNode;

		bool (*OcclusionQueryCallbackPtr)(const core::line3df& ray);
        
};

} // end of IrrAI namespace

#endif /* INC_CAIMANAGER_H */
