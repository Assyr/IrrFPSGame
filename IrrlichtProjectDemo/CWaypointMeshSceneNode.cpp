#include "CWaypointMeshSceneNode.h"

namespace IrrAI {

CWaypointMeshSceneNode::CWaypointMeshSceneNode(scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id, const video::SColor& colour, s32 size) : scene::ISceneNode(parent, smgr, id) {
              
	#ifdef _DEBUG
		setDebugName("CWaypointMeshSceneNode");
	#endif
	
	WaypointSize = size;
	
	CurrentBuffer = 0;

	addNewBuffers();	

	setColour(colour);
	
}

void CWaypointMeshSceneNode::remove() {
    
	clear();
  
	scene::ISceneNode::remove(); 
   
}

void CWaypointMeshSceneNode::addNewBuffers() {

	scene::SMeshBuffer* buffer = NULL;
  
	buffer = new scene::SMeshBuffer();
	buffer->Material.Lighting = false;
  
	CubeBuffers.push_back(buffer);
  
	buffer = new scene::SMeshBuffer();
	buffer->Material.Lighting = false;
	buffer->Material.BackfaceCulling = false;
  
	LinkBuffers.push_back(buffer);
	     
}

void CWaypointMeshSceneNode::appendCube(const core::vector3df& pos) {
    
    scene::SMeshBuffer buffer;
    
    // Vertices
    buffer.Vertices.set_used(12);
	buffer.Vertices[0]  = video::S3DVertex(0,0,0, -1,-1,-1, Colour, 0, 1); 
	buffer.Vertices[1]  = video::S3DVertex(1,0,0,  1,-1,-1, Colour, 1, 1); 
	buffer.Vertices[2]  = video::S3DVertex(1,1,0,  1, 1,-1, Colour, 1, 0); 
	buffer.Vertices[3]  = video::S3DVertex(0,1,0, -1, 1,-1, Colour, 0, 0); 
	buffer.Vertices[4]  = video::S3DVertex(1,0,1,  1,-1, 1, Colour, 0, 1); 
	buffer.Vertices[5]  = video::S3DVertex(1,1,1,  1, 1, 1, Colour, 0, 0); 
	buffer.Vertices[6]  = video::S3DVertex(0,1,1, -1, 1, 1, Colour, 1, 0); 
	buffer.Vertices[7]  = video::S3DVertex(0,0,1, -1,-1, 1, Colour, 1, 1); 
	buffer.Vertices[8]  = video::S3DVertex(0,1,1, -1, 1, 1, Colour, 0, 1); 
	buffer.Vertices[9]  = video::S3DVertex(0,1,0, -1, 1,-1, Colour, 1, 1); 
	buffer.Vertices[10] = video::S3DVertex(1,0,1,  1,-1, 1, Colour, 1, 0); 
	buffer.Vertices[11] = video::S3DVertex(1,0,0,  1,-1,-1, Colour, 0, 0); 

	for (s32 i = 0 ; i < 12 ; ++i) {
		buffer.Vertices[i].Pos -= core::vector3df(0.5f, 0.0f, 0.5f); // don't alter Y position
		buffer.Vertices[i].Pos *= (f32)WaypointSize;
	 	buffer.Vertices[i].Pos += pos;
	}     
	
	// Indices  
    u16 u[36] = { 0,2,1, 0,3,2, 1,5,4, 1,2,5,   4,6,7, 4,5,6, 
                  7,3,0, 7,6,3, 9,5,2, 9,8,5, 0,11,10, 0,10,7 };
	buffer.Indices.set_used(36);
	for (s32 i = 0 ; i < 36 ; ++i) buffer.Indices[i] = u[i];
		
	CubeBuffers[CurrentBuffer]->append(&buffer.Vertices[0], buffer.Vertices.size(), &buffer.Indices[0], buffer.Indices.size());
	CubeBuffers[CurrentBuffer]->BoundingBox.addInternalPoint(buffer.Vertices[0].Pos);
	CubeBuffers[CurrentBuffer]->BoundingBox.addInternalPoint(buffer.Vertices[5].Pos);
  
}

void CWaypointMeshSceneNode::appendLink(const core::vector3df& from, const core::vector3df& to, const core::vector3df& offset) {
       
	#define NORM core::vector3df(0,1,0)
	#define TC core::vector2df(0,0)
	#define NUM_INDICES 9
  
	scene::SMeshBuffer buffer;
  
	// Vertices  
	core::vector3df vec = core::line3df(from, to).getVector().normalize()*(f32)WaypointSize;
	core::vector3df cross = vec.crossProduct(NORM).normalize(); 
	// Arrow head
	buffer.Vertices.set_used(7);
	buffer.Vertices[0] = video::S3DVertex((to-vec/2.0f) + offset,											NORM, Colour, TC);                            
	buffer.Vertices[1] = video::S3DVertex((to-vec/2.0f) + (-(cross*(f32)WaypointSize)-vec*1.5f) + offset,	NORM, Colour, TC);                            
	buffer.Vertices[2] = video::S3DVertex((to-vec/2.0f) + ((cross*(f32)WaypointSize)-vec*1.5f) + offset,	NORM, Colour, TC);              
	// Arrow body              
	buffer.Vertices[3] = video::S3DVertex((from+vec*2.0f) + (-cross*(f32)WaypointSize*0.25f) + offset,		NORM, Colour, TC);                            
	buffer.Vertices[4] = video::S3DVertex((from+vec*2.0f) + (cross*(f32)WaypointSize*0.25f) + offset,		NORM, Colour, TC); 
	buffer.Vertices[5] = video::S3DVertex((to-vec*2.0f) + (-cross*(f32)WaypointSize*0.25f) + offset,		NORM, Colour, TC);                            
	buffer.Vertices[6] = video::S3DVertex((to-vec*2.0f) + (cross*(f32)WaypointSize*0.25f) + offset,			NORM, Colour, TC);
           
	// Indices 
	u16 u[NUM_INDICES] = { 0,1,2, 3,4,6, 6,5,3 };
	buffer.Indices.set_used(NUM_INDICES);
	for (s32 i = 0 ; i < NUM_INDICES ; ++i) buffer.Indices[i] = u[i];
  
	LinkBuffers[CurrentBuffer]->append(&buffer.Vertices[0], buffer.Vertices.size(), &buffer.Indices[0], buffer.Indices.size());
  
}

void CWaypointMeshSceneNode::addTextNode(const core::vector3df& pos, s32 id) {
    
	core::stringw wstr(id);
	scene::ITextSceneNode* textNode = SceneManager->addTextSceneNode(SceneManager->getGUIEnvironment()->getSkin()->getFont(), wstr.c_str(), video::SColor(255, 255-Colour.getRed(), 255-Colour.getGreen(), 255-Colour.getBlue()), this, pos);
	TextNodes.push_back(textNode);
     
}

void CWaypointMeshSceneNode::addWaypoint(IWaypoint* waypoint) {
  
	if (!waypoint) return;
 
	if (CubeBuffers.size() == 0) {
		addNewBuffers();
		CurrentBuffer = 0;
	}

	if (CubeBuffers[CurrentBuffer]->Vertices.size() + 12 >= 65536 || LinkBuffers[CurrentBuffer]->Vertices.size() + (7 * waypoint->getNeighbours().getSize()) >= 65536) {
		addNewBuffers();
		++CurrentBuffer;
	}

	appendCube(waypoint->getPosition());
 
	core::vector3df offset(0,WaypointSize/2.0f,0);
  
	addTextNode(waypoint->getPosition() + offset, waypoint->getID());
  
	core::list<SNeighbour*>::ConstIterator nbrIter = waypoint->getNeighbours().begin();
	while (nbrIter != waypoint->getNeighbours().end()) {
		appendLink(waypoint->getPosition(), (*nbrIter)->Waypoint->getPosition(), offset);
		++nbrIter;
	}  
	
}

void CWaypointMeshSceneNode::render() {
     
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

    for (u32 i = 0 ; i < CubeBuffers.size() ; ++i) 
		if (CubeBuffers[i]->Vertices.size() && CubeBuffers[i]->Indices.size()) {
			// Draw waypoints (cubes)
			video::IVideoDriver* driver = SceneManager->getVideoDriver();
			driver->setMaterial(CubeBuffers[i]->Material);
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			driver->drawMeshBuffer(CubeBuffers[0]); 
		}
	  
	for (u32 i = 0 ; i < LinkBuffers.size() ; ++i) 
		if (LinkBuffers[i]->Vertices.size() && LinkBuffers[i]->Indices.size()) {
			// Draw links (arrows)
			video::IVideoDriver* driver = SceneManager->getVideoDriver();
			driver->setMaterial(LinkBuffers[i]->Material);
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			driver->drawMeshBuffer(LinkBuffers[0]); 
		}
	
}


void CWaypointMeshSceneNode::OnRegisterSceneNode() {
     
	if (IsVisible)
		SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);

	scene::ISceneNode::OnRegisterSceneNode();
	
}

video::SMaterial& CWaypointMeshSceneNode::getMaterial(u32 i) {
                  
	switch (i) {
		case 0:		return CubeBuffers[0]->Material;
		case 1:		return LinkBuffers[0]->Material;
		default:	return CubeBuffers[0]->Material;
    }
    
}

scene::ISceneNode* CWaypointMeshSceneNode::clone(scene::ISceneNode* newParent, scene::ISceneManager* newManager) {
            
	if (!newParent) newParent = Parent;
	if (!newManager) newManager = SceneManager;

	CWaypointMeshSceneNode* nb = new CWaypointMeshSceneNode(newParent, newManager, ID, Colour, WaypointSize);

	nb->cloneMembers(this, newManager);

	nb->drop();
	return nb;
	
}

void CWaypointMeshSceneNode::setColour(const video::SColor& colour) {
     
	Colour = colour;
  
	for (u32 b = 0 ; b < CubeBuffers.size() ; ++b) {
		for (u32 v = 0 ; v < CubeBuffers[b]->Vertices.size() ; ++v)
			CubeBuffers[b]->Vertices[v].Color = Colour;
		CubeBuffers[b]->Material.DiffuseColor = Colour;  
	}
   
	for (u32 b = 0 ; b < LinkBuffers.size() ; ++b) {
		for (u32 v = 0 ; v < LinkBuffers[b]->Vertices.size() ; ++v)
			LinkBuffers[b]->Vertices[v].Color = Colour;
		LinkBuffers[b]->Material.DiffuseColor = Colour;    
	} 
  
	for (u32 b = 0 ; b < TextNodes.size() ; ++b) {
		TextNodes[b]->setTextColor(video::SColor(255, 255-Colour.getRed(), 255-Colour.getGreen(), 255-Colour.getBlue()));       
	}     
        
}

void CWaypointMeshSceneNode::clear() {
 
	for (u32 i = 0 ; i < CubeBuffers.size() ; ++i) 
		CubeBuffers[i]->drop();
	CubeBuffers.clear();
  
	for (u32 i = 0 ; i < LinkBuffers.size() ; ++i) 
		LinkBuffers[i]->drop();
	LinkBuffers.clear();
  
	for (u32 i = 0 ; i < TextNodes.size() ; ++i) 
		TextNodes[i]->remove();   
	TextNodes.clear();
  
	CurrentBuffer = 0;
  
}

} // end of IrrAI namespace
