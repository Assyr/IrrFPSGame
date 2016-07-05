#include "CDebugConeFOVSceneNode.h"

#define FOV_HEIGHT 0.5f

namespace IrrAI {

CDebugConeFOVSceneNode::CDebugConeFOVSceneNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, const core::vector3df& dim) : IDebugFOVSceneNode(parent, mgr, id) {
	
	#ifdef _DEBUG
		setDebugName("CDebugConeFOVSceneNode");
	#endif

	Material.Wireframe = false;
	Material.Lighting = false;
	Material.BackfaceCulling = false;
	Material.MaterialType = video::EMT_TRANSPARENT_VERTEX_ALPHA;
	Material.ZWriteEnable = false;
	Material.ZBuffer = 1;

	setDimensions(dim);
    
}

void CDebugConeFOVSceneNode::render() {

	u16 indices[] = {0,1,2};
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
    
    driver->setMaterial(Material);
    driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	driver->drawIndexedTriangleList(&Vertices[0], 3, &indices[0], 1);
	
}

video::SMaterial& CDebugConeFOVSceneNode::getMaterial(u32 i) {

	return Material;

}


u32 CDebugConeFOVSceneNode::getMaterialCount() {

	return 1;

}

void CDebugConeFOVSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) {

	scene::ISceneNode::serializeAttributes(out, options);

}

void CDebugConeFOVSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options) {

	scene::ISceneNode::deserializeAttributes(in, options);
}

scene::ISceneNode* CDebugConeFOVSceneNode::clone(scene::ISceneNode* newParent, scene::ISceneManager* newManager) {

	if (!newParent) newParent = Parent;
	if (!newManager) newManager = SceneManager;

	CDebugConeFOVSceneNode* nb = new CDebugConeFOVSceneNode(newParent, newManager, ID, Dimensions);

	nb->cloneMembers(this, newManager);

	nb->drop();
	return nb;

}

//! Returns type of the scene node
scene::ESCENE_NODE_TYPE CDebugConeFOVSceneNode::getType() const {

	return (scene::ESCENE_NODE_TYPE)CONEFOV_SCENE_NODE_ID;

}

void CDebugConeFOVSceneNode::OnRegisterSceneNode() {
  
	if (IsVisible) SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);
  
	scene::ISceneNode::OnRegisterSceneNode();

}

void CDebugConeFOVSceneNode::setDimensions(const core::vector3df& dim) {

	IDebugFOVSceneNode::setDimensions(dim);

	Vertices[0] = video::S3DVertex(0,0,0, 0,1,0, video::SColor(100,255,0,0), 0, 1);
	Vertices[1] = video::S3DVertex(dim.Y,0,dim.X/2.0f, 0,1,0, video::SColor(100,255,0,0), 1, 1);
	Vertices[2] = video::S3DVertex(dim.Y,0,-dim.X/2.0f, 0,1,0, video::SColor(100,255,0,0), 0, 0);

	Box.reset(Vertices[0].Pos);
	for (s32 i = 1 ; i < 3 ; ++i)
		Box.addInternalPoint(Vertices[i].Pos);

}

} // end of IrrAI namespace
