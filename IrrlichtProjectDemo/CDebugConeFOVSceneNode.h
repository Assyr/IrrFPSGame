#ifndef INC_CDEBUGCONEFOVSCENENODE_H
#define INC_CDEBUGCONEFOVSCENENODE_H

#include "IDebugFOVSceneNode.h"

namespace IrrAI {

/*! Class implementation of IDebugFOVSceneNode to visualise a CConeFieldOfView */
class CDebugConeFOVSceneNode : public IDebugFOVSceneNode {

	public:
        // id for our scene node, 'cfov', short for 'cone field of view'.
        static const s32 CONEFOV_SCENE_NODE_ID = MAKE_IRR_ID('c','f','o','v');

		/**
        \brief Constructor
        \param parent - parent node
        \param mgr - associated scene manage
        \param id - desired id
        \param dim - desired dimensions of field of view
        \param position - desired position
        \param rotation - desired rotation
        \param scale - desired scale
        */
		CDebugConeFOVSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, const core::vector3df& dim);
		/**
        \brief Destructor
        */
		virtual ~CDebugConeFOVSceneNode() {}

		/**
		\brief Renders the node
		*/
		virtual void render();
		/**
        \brief Returns the material based on the zero based index i.
        \param i - index of the required material
        \return a video::SMaterial&
        */
		virtual video::SMaterial& getMaterial(u32 i);
		/**
        \brief Returns the number of materials used by this node.
        \return a u32
        */
		virtual u32 getMaterialCount();
		/**
        \brief Writes attributes of the scene node.
        \param out - the attributes to write to
        \param options - additonal options that maya ffect the serialisation
        */
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0);
		/**
        \brief Reads attributes of the scene node.
        \param in - the attributes to read from
        \param options - additonal options that maya ffect the serialisation
        */
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);
		/**
        \brief Creates a clone of this scene node and its children.
        \param newParent - the cloned node's new parent
        \param newManager - the cloned node's new scene manager
        \return a ISceneNode*
        */
		virtual scene::ISceneNode* clone(scene::ISceneNode* newParent=0, scene::ISceneManager* newManager=0);
        /**
        \brief Determines whether the node should be rendered this frame or not
        */
		virtual void OnRegisterSceneNode();
        /**
        \brief Returns the type of the node
        \return a ESCENE_NODE_TYPE
        */
		scene::ESCENE_NODE_TYPE getType() const;
		virtual void setDimensions(const core::vector3df& dim);
		
	private:
		video::S3DVertex Vertices[3];
	    video::SMaterial Material;

};

} // end of IrrAI namespace

#endif /* INC_CDEBUGCONEFOVSCENENODE_H */
