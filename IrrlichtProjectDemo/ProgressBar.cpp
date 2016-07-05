#include "ProgressBar.h"

void updateProgress(IrrlichtDevice* device, f32 prog, void (*fn)()) {
  
	video::IVideoDriver* driver = device->getVideoDriver();
	core::dimension2d<u32> screenDim = driver->getScreenSize();  
	s32 barWidth = 256;
	s32 barHeight = 16;
	video::SColor colours[4];
	for (s32 i = 0 ; i < 4 ; ++i)
		colours[i] = video::SColor(255,240,115,0);
	core::rect<s32> clip((screenDim.Width/2)-(barWidth/2),(screenDim.Height-50)-barHeight, ((screenDim.Width/2)-(barWidth/2))+((s32)(barWidth*(prog/100.0f))),screenDim.Height-50);
  
	if (device->run()) {
		driver->beginScene(true, true, video::SColor(255,0,0,0));
    
		// If there's a render callback passed in then call it
		if (fn) fn();
    
		// Draw the progress box
		driver->draw2DImage(driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\progress_bar.png"),
                        core::rect<s32>((screenDim.Width/2)-(barWidth/2),(screenDim.Height-50)-barHeight, (screenDim.Width/2)+(barWidth/2),screenDim.Height-50),
                        core::rect<s32>(0,0, barWidth,barHeight),
                        &clip, colours, true);
		// Draw the progress bar
		driver->draw2DImage(driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\progress_box.png"),
                        core::rect<s32>((screenDim.Width/2)-(barWidth/2),(screenDim.Height-50)-barHeight, (screenDim.Width/2)+(barWidth/2),screenDim.Height-50),
                        core::rect<s32>(0,0, barWidth,barHeight),
                        NULL, NULL, true);
                    
		// Draw the progress percentage    
		core::stringw strw((s32)prog);
		strw += "%";
		device->getGUIEnvironment()->getSkin()->getFont()->draw(strw.c_str(), core::rect<s32>((screenDim.Width/2)-(barWidth/2),(screenDim.Height-50)-barHeight, (screenDim.Width/2)+(barWidth/2),screenDim.Height-50), video::SColor(255,255,255,255), true, true);
    
		driver->endScene();                 
	}
   
}
