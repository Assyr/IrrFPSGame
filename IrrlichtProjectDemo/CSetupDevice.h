#ifndef INC_CSETUPDEVICE_H
#define INC_CSETUPDEVICE_H

#include <irrlicht.h>

using namespace irr;

class SetupDeviceEventReceiver;

/*! Class to provide a simple and customisable configuration window on start-up */
class CSetupDevice {
  
	public:
		/**
		\brief Constructor
		\param screenDim - The dimensions of the window to pop-up
		*/
		CSetupDevice(const core::dimension2d<u32>& screenDim);
		/**
		\brief Destructor
		*/      
		~CSetupDevice();
		/**
		\brief Begin execution of the device
		*/
		bool execute();
		/**
		\brief End execution of the device
		*/
		void shutdown();
    
		/**
		\brief Returns the Irrlicht Device
		\return an IrrlichtDevice*
		*/
		inline IrrlichtDevice* getDevice() { return setupDevice; }
		/**
		\brief Returns the Video Driver
		\return a IVideoDriver*
		*/
		inline video::IVideoDriver* getVideoDriver() { return driver; }
		/**
		\brief Returns the GUI Environment
		\return a IGUIEnvironment*
		*/
		inline gui::IGUIEnvironment* getGUIEnvironment() { return gui; }
		/**
		\brief Returns the desktop resolution of the PC
		\return a dimension2d<u32>
		*/
		inline core::dimension2d<u32> getDesktopResolution() {
			if (setupDevice) return setupDevice->getVideoModeList()->getDesktopResolution();
			else             return core::dimension2d<u32>(0,0);
		}
        
	private:
		IrrlichtDevice* setupDevice;
		video::IVideoDriver* driver;
		video::ITexture* gradTexture;
		gui::IGUIEnvironment* gui;
		gui::IGUIButton* startButton;
		SetupDeviceEventReceiver* receiver;
		bool quitSetup;
		bool startGame;
    
};

#endif /* INC_CSETUPDEVICE_H */
