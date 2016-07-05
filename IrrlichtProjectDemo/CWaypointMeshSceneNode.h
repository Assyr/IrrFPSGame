#ifndef INC_CWAYPOINTMESHSCENENODE_H
#define INC_CWAYPOINTMESHSCENENODE_H

#include "IWaypoint.h"

namespace IrrAI {

// own scene node implementation
class CWaypointMeshSceneNode : public scene::ISceneNode {
      
	public:
        // id for our scene node, 'wtmh', short for 'waypoint mesh'.
        static const s32 WAYPOINTMESH_SCENE_NODE_ID = MAKE_IRR_ID('w','t','m','h');

		//! constructor
		CWaypointMeshSceneNode(scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id = -1, const video::SColor& colour = video::SColor(255,255,255,255), s32 size = 5.0f);

		//! destructor
		virtual ~CWaypointMeshSceneNode() {}
		
		void remove();
		
		virtual void OnRegisterSceneNode();

		//! renders the node.
		virtual void render();
		
		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(u32 i);

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(scene::ISceneNode* newParent=0, scene::ISceneManager* newManager=0);

		void addWaypoint(IWaypoint* waypoint);
		
		void setColour(const video::SColor& colour);
				
		void clear();
				
		//! returns amount of materials used by this scene node.
		virtual inline u32 getMaterialCount() { return 2; }
        //! returns the axis aligned bounding box of this node
		virtual inline const core::aabbox3d<f32>& getBoundingBox() const { return BoundingBox; }    
        inline scene::ESCENE_NODE_TYPE getType() const { return (scene::ESCENE_NODE_TYPE)WAYPOINTMESH_SCENE_NODE_ID; }    
        inline void recalculateBoundingBox() {              
			for (u32 i = 0 ; i < CubeBuffers.size() ; ++i) 
				BoundingBox.addInternalBox(CubeBuffers[i]->BoundingBox);
        }
        inline void setWaypointSize(s32 size) { WaypointSize = size; }
        
	private:
		core::array<scene::SMeshBuffer*> CubeBuffers;
		core::array<scene::SMeshBuffer*> LinkBuffers;
		core::array<scene::ITextSceneNode*> TextNodes;
		video::SColor Colour;
		core::aabbox3df BoundingBox;
		s32 WaypointSize;
		s32 CurrentBuffer;
		
		void addNewBuffers();
		void appendCube(const core::vector3df& pos);
		void appendLink(const core::vector3df& from, const core::vector3df& to, const core::vector3df& offset);
		void addTextNode(const core::vector3df& pos, s32 id);
		
};

} // end of IrrAI namespace

#endif /* INC_CWAYPOINTMESHSCENENODE_H */
