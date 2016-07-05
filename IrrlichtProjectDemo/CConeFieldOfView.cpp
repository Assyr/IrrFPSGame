#include "CConeFieldOfView.h"
#include <math.h>

namespace IrrAI {

CConeFieldOfView::CConeFieldOfView(IAIManager* aimgr, bool occlusionCheck, const core::vector3df& dim) : IFieldOfView(aimgr, occlusionCheck, dim) {
		  
	setDimensions(dim);
        
}

CConeFieldOfView::~CConeFieldOfView() {
                                       
}

bool CConeFieldOfView::isInFOV(const core::aabbox3df& box, const core::vector3df& boxPos) {
     
	// Check pos is within field of vision
	core::line3df line;
	getLeftViewLine(line);
	core::vector3df leftPoint = line.getClosestPoint(boxPos);
	getRightViewLine(line);
	core::vector3df rightPoint = line.getClosestPoint(boxPos);
	core::vector3df extent = box.getExtent();
	if (Position.getDistanceFrom(boxPos) < Dimensions.Y + ((extent.X > extent.Z) ? extent.X : extent.Z)) {
		leftPoint.Y = boxPos.Y;
		rightPoint.Y = boxPos.Y;
		if ( boxPos.isBetweenPoints(leftPoint, rightPoint)
			|| box.isPointInside(leftPoint)
			|| box.isPointInside(rightPoint) ) {
			core::line3df ray = core::line3df(Position, boxPos);
			if (OcclusionCheck && AIManager && AIManager->occlusionQuery(ray))
				return false;
			else
				return true;
		}
	}
  
	return false;
     
}

void CConeFieldOfView::getLeftViewLine(core::line3df& line) {
  
	core::vector3df endPoint = Vertices[2].Pos;
	core::matrix4 rotMat, transMat, combMat;
	rotMat.setRotationDegrees(Rotation);
	transMat.setTranslation(Position);
	combMat = transMat * rotMat;
	combMat.transformVect(endPoint);
	line.start = Position;
	line.end = endPoint;
             
}

void CConeFieldOfView::getRightViewLine(core::line3df& line) {

	core::vector3df endPoint = Vertices[1].Pos;
	core::matrix4 rotMat, transMat, combMat;
	rotMat.setRotationDegrees(Rotation);
	transMat.setTranslation(Position);
	combMat = transMat * rotMat;
	combMat.transformVect(endPoint);
	line.start = Position;
	line.end = endPoint;
  
}

void CConeFieldOfView::setDimensions(const core::vector3df& dim) {

	IFieldOfView::setDimensions(dim);

	Vertices[0] = video::S3DVertex(0,0,0, 0,1,0, video::SColor(100,255,0,0), 0, 1);
	Vertices[1] = video::S3DVertex(dim.Y,0,dim.X/2.0f, 0,1,0, video::SColor(100,255,0,0), 1, 1);
	Vertices[2] = video::S3DVertex(dim.Y,0,-dim.X/2.0f, 0,1,0, video::SColor(100,255,0,0), 0, 0);

}

} // end of IrrAI namespace
