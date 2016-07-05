#ifndef INC_CFIELDOFVIEW_H
#define INC_CFIELDOFVIEW_H

#include "IFieldOfView.h"

namespace IrrAI {

class CConeFieldOfView : public IFieldOfView {
      
	public:
		CConeFieldOfView(IAIManager* aimgr, bool occlusionCheck, const core::vector3df& dim);
		~CConeFieldOfView();
		bool isInFOV(const core::vector3df& pos);
		bool isInFOV(const core::aabbox3df& box, const core::vector3df& boxPos);
		void setDimensions(const core::vector3df& dim);
		
	private:
	    video::S3DVertex Vertices[3];
	    
	    void getLeftViewLine(core::line3df&);
	    void getRightViewLine(core::line3df&);

};

} // end of IrrAI namespace

#endif /* INC_CFIELDOFVIEW_H */
