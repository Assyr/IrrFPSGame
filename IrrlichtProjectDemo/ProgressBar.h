#ifndef INC_PROGRESSBAR_H
#define INC_PROGRESSBAR_H

#include <irrlicht.h>

using namespace irr;

/**
\brief Renders a progress bar with the specified amount of progress.

An optional render callback can be passed which will be called before the progress bar is rendered
\param device - the Irrlicht Device to use for rendering
\param prog - the amount of progress to show
\param fn - optional render callback function
*/
void updateProgress(IrrlichtDevice* device, f32 prog, void (*fn)() = NULL);

#endif /* INC_PROGRESSBAR_H */
