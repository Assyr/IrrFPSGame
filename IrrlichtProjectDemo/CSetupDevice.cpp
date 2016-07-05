#include "CSetupDevice.h"

class SetupDeviceEventReceiver : public IEventReceiver {
      
	public:
		SetupDeviceEventReceiver(bool* quit, bool* start) {
			quitSetup = quit;
			startGame = start;
		}
    
		void setStartButton(gui::IGUIButton* button) {
			startButton = button;     
		}
    
		virtual bool OnEvent(const SEvent& event) {

			if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
				switch(event.KeyInput.Key) {
					case KEY_ESCAPE: {
						// Quit the setup
						if (quitSetup) *quitSetup = true;
						return true;     
					}
				}
			} else if (event.EventType == EET_GUI_EVENT) {
				if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED) {
					if (event.GUIEvent.Caller == startButton) {
						if (startGame) *startGame = true;
						return true; 
					} 
				} 
			}  
          
		return false;
      
	}
	
	private:
		gui::IGUIButton* startButton;
		bool* quitSetup;
		bool* startGame;
	
};

CSetupDevice::CSetupDevice(const core::dimension2d<u32>& screenDim) {
  
	startGame = false;
	quitSetup = false;
  
	// Create a software based device to allow the user to choose some options
	receiver = new SetupDeviceEventReceiver(&quitSetup, &startGame);
	if (!receiver) {
		printf("Failed eventReceiver creation\n");
		setupDevice = NULL;
		return;               
	}
	setupDevice = createDevice(video::EDT_OPENGL, screenDim, 24, false, false, true, receiver);
	if (!setupDevice) {
		printf("Failed setupDevice creation\n");
		return;
	}
	setupDevice->run(); // recreating the device makes run() return false first time so call it here so the render loop below won't quit early
	setupDevice->setWindowCaption(L"Setup");  

	driver = setupDevice->getVideoDriver();
	gui = setupDevice->getGUIEnvironment();
	gui->getSkin()->setFont(gui->getFont("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\GillSans12.png")); 
	// disable gui alpha
	for (s32 i=0; i<gui::EGDC_COUNT ; ++i) {
		video::SColor col = gui->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(255);
		gui->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}
  
	// add the lovely irrlicht-irrai logo
	gui->addImage(driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\irrlichtirrailogo.png"), core::position2di(screenDim.Width-137,screenDim.Height-110), true, NULL);
  
	// create button to start game
	s32 buttonWidth = 256;
	s32 buttonHeight = 64;
	startButton = gui->addButton(core::rect<s32>((screenDim.Width/2)-(buttonWidth/2),(screenDim.Height-10)-buttonHeight, (screenDim.Width/2)+(buttonWidth/2),screenDim.Height-10));
	video::ITexture* buttonImage = driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\start_button.png");
	core::dimension2d<u32> buttonDim = buttonImage->getSize();
	startButton->setImage(buttonImage, core::rect<s32>(0, 0, buttonDim.Width, buttonDim.Height));
	video::ITexture* buttonPressedImage = driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\start_pressed.png");
	core::dimension2d<u32> buttonPressedDim = buttonPressedImage->getSize();
	startButton->setPressedImage(buttonPressedImage, core::rect<s32>(0, 0, buttonPressedDim.Width, buttonPressedDim.Height));
	startButton->setUseAlphaChannel(true);
	startButton->setDrawBorder(false);
	receiver->setStartButton(startButton);
  
	gradTexture = driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\gradient.jpg");
  
	// create a combobox for selecting the driver type
	gui->addStaticText(L"Graphics Driver:", core::rect<s32>(screenDim.Width-285,20, screenDim.Width-170,50), false, true);
	gui::IGUIComboBox* box = gui->addComboBox(core::rect<s32>(screenDim.Width-160,20, screenDim.Width-10,40), NULL, 1);
	box->addItem(L"Software");
	box->addItem(L"Burnings Video");
	box->addItem(L"Direct3D8");
	box->addItem(L"Direct3D9");  
	box->addItem(L"OpenGL");
	box->setSelected(3);
                         
}

CSetupDevice::~CSetupDevice() {
  
	shutdown();
                              
}

void CSetupDevice::shutdown() {

	if (setupDevice) {
		setupDevice->closeDevice();
		setupDevice->drop();     
		setupDevice = NULL;       
	}     
  
	if (receiver) {
		delete receiver;
		receiver = NULL;              
	}
  
}

bool CSetupDevice::execute() {
  
	while (setupDevice->run() && !startGame && !quitSetup) {
		driver->beginScene(true, true, video::SColor(255,0,0,0));
		// draw a nice gradient background
		driver->draw2DImage(gradTexture, core::position2d<s32>(0,0));
		// draw the GUI
		gui->drawAll();
		driver->endScene();                 
	}
  
	return !startGame;
       
}
