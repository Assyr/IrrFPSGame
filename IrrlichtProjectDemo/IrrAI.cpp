#include "IrrAI.h"
#include "CAIManager.h"

namespace IrrAI {

IAIManager* aiManager = NULL;


IAIManager* createAIManager(IrrlichtDevice* device) {

	if (aiManager) removeAIManager(aiManager);    
	aiManager = new CAIManager(device);
	return aiManager;
   
}

void removeAIManager(IAIManager* aimgr) {

	if (aiManager == aimgr) {
		delete aiManager;
		aiManager = NULL;           
	}     
  
}

} // end of IrrAI namespace
