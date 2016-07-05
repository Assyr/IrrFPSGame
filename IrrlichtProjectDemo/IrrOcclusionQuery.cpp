#include "IrrOcclusionQuery.h"

#ifndef IRRPHYSX_OCCLUSION

scene::ISceneManager* sceneManager = NULL;
scene::ITriangleSelector* triangleSelector = NULL;

bool occlusionQuery(const core::line3df& ray) {
 
  // Check obj is not behind a wall
  core::vector3df collisionPoint;
  core::triangle3df collisionTri;
  scene::ISceneNode* collisionNode = NULL;
  if (triangleSelector && sceneManager)
    if (sceneManager->getSceneCollisionManager()->getCollisionPoint(ray, triangleSelector, collisionPoint, collisionTri, collisionNode))
      return true;  
      
  return false;
         
}

void setOcclusionTriangleSelector(scene::ITriangleSelector* selector) {

  triangleSelector = selector;
       
}

void setOcclusionSceneManager(scene::ISceneManager* smgr) {
     
  sceneManager = smgr;
       
}

#endif /* !IRRPHYSX_OCCLUSION */
