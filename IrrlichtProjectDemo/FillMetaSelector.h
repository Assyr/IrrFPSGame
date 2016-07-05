#ifndef INC_FILLMETASELECTOR_H
#define INC_FILLMETASELECTOR_H

#include <irrlicht.h>

using namespace irr;

/**
\brief Fills the specified meta triangle selector with triangle selectors from the current scene

Only nodes of the following types have triangle selectors created, all others are ignored:
      ESNT_MESH,
      ESNT_ANIMATED_MESH,
      ESNT_MD3_SCENE_NODE,
      ESNT_SPHERE,
      ESNT_CUBE,
      ESNT_BILLBOARD,
      ESNT_WATER_SURFACE,
      ESNT_OCT_TREE,
      ESNT_TERRAIN,
      
All have triangle selectors created from their bounding boxes, except for Oct Tree nodes (which
have an Oct Tree Triangle Selector created) and Terrain nodes (which have a Terrain Triangle Selector
created).
\param device - The device that holds the scene
\param metaSelector - The meta selector to get the triangle selectors
\param node - The node to start from (should be the root scene node!)
*/
void fillMetaSelector(IrrlichtDevice* device, scene::IMetaTriangleSelector* metaSelector, scene::ISceneNode* node);

#endif /* INC_FILLMETASELECTOR_H */
