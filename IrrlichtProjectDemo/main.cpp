#include <irrlicht.h>
#include <IrrAI.h>
#include <iostream>

#include "main.h"

#include "CSetupDevice.h"
#include "ProgressBar.h"
#include "FillMetaSelector.h"

#include "Character.h"
#include "ChasingCharacter.h"
#include "NPCharacter.h"
#include "CPlayerCharacter.h"
#include "Turret.h"
#include "TurretEntity.h"
#include "CTurretCharacter.h"
#include <irrKlang.h>


//Use the following Irrlicht namespaces
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;
using namespace std;
using namespace irrklang;


#if defined(_MSC_VER)
#pragma comment(lib, "Irrlicht.lib")
#endif

#if defined(_MSC_VER)
#pragma comment(lib, "IrrAI.lib")
#endif

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll


//Initial setup
IrrlichtDevice* device = NULL;
IVideoDriver* driver = NULL;
ISceneManager* smgr = NULL;
IGUIEnvironment* guienv = NULL;

//Array of characters to store all the NP characters (Non playable characters)
core::array<Character*> characters;
core::array<NPCharacter*> NPCharacters;
CPlayerCharacter* player = NULL;
IGUIStaticText* debugInformation = NULL;//GUI text to hold debug information when toggled
IGUIStaticText* pausedText = NULL; //Paused text to display when game paused



IGUIStaticText* occlusionText = NULL;
IGUIStaticText* aiDebugText = NULL;
bool occlusion = true;
IAIManager* AIMgr = NULL;
bool quitGame = false;
bool pauseGame = false;
bool mouseIsDown = false;
bool startMenuClosed = false;
bool AIDebug = false;

SWaypointGroup* waypointGrp = NULL; //Waypoint group will be used to see what waypoint group a enemy is part of
core::array<stringc> mapList; //This array will be used later to store paths to all the maps I will use in my game (Multiple maps)

//Our meta triangle selector, this will store triangles from multiple nodes
//This allows us to run collision detection in 1 run instead of per object..

//Simply throw all the triangles under 1 entity 'metaSelector' and run collision detection against it
IMetaTriangleSelector* metaSelector = NULL;


//Create a method of removing everything in the game in one go... this saves us from rewriting code to close the application down later..



void shutDown()
{
	// Remove characters
	for (u32 i = 0; i < characters.size(); ++i)
		delete characters[i];
	characters.clear();


	//Still to be implemented-- DONE.
	// Remove characters
	for (u32 i = 0; i < NPCharacters.size(); ++i)
		delete NPCharacters[i];
	NPCharacters.clear();

	if (player) {
		delete player;
		player = NULL;
	}

	// Clear out AI Manager
	if (AIMgr) {
		AIMgr->clear();
		removeAIManager(AIMgr);
		AIMgr = NULL;
	}

	// Shutdown device
	if (device) {
		device->closeDevice();
		device->drop();
		device = NULL;
	}
	return;
}


//Our event receiver class called CAppReceiver which inherits from the IEventReceiver class
class CAppReceiver : public IEventReceiver
{
public:
	//On Event method which takes an event as a paramter
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
			//Run switch statement regarding the key inputted.

			switch (event.KeyInput.Key)
			{
			case KEY_ESCAPE:
				{
					shutDown();
					quitGame = true;
					return true;
						   }
			case KEY_F9:
				{
					AIDebug = !AIDebug; //Set aidebug to the opposite of what it initially was, this saves us from running further checks
					if (AIMgr)
						AIMgr->setDebugVisible(AIDebug);

					if (debugInformation)
						debugInformation->setText(AIDebug ? L"ON" : L"OFF");
					return true;
					   }
			case KEY_KEY_P:
				{
					pauseGame = !pauseGame;

					if (pausedText)
						pausedText->setVisible(pauseGame);

					if (pauseGame) //Pause device timer
					{
						if (device)
							device->getTimer()->stop();

						//During game paused stop listenig for receivers
						if (smgr && player)
							smgr->getActiveCamera()->setInputReceiverEnabled(false);
					}
					else
					{
						//Resume game
						if (device)
							device->getTimer()->start();

						if (smgr)
							smgr->getActiveCamera()->setInputReceiverEnabled(true);
					}
					return true;

						  }
			}
		}

		//Run checks for mouse events
		else if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			switch (event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
				mouseIsDown = true;
				break;
			case EMIE_LMOUSE_LEFT_UP:
				mouseIsDown = false;
				break;
			}
		}
		//Run GUI event checks..
		else if (event.EventType == EET_GUI_EVENT)
		{
			if (event.GUIEvent.EventType == EGET_MESSAGEBOX_OK)
			{
				startMenuClosed = true;
			}
		}
		return false;
	}
};


void drawProgressBar()
{
	guienv->drawAll();
}


void startWindow(CSetupDevice* setupDevice)
{
	if (!setupDevice)
	{
		printf("Setup device not found to create start window");
		return;
	}

	IGUIEnvironment* startGUI = setupDevice->getGUIEnvironment();
	IGUIComboBox* comboBox = NULL; //Create the combobox to choose from multiple different renderers.

	startGUI->addStaticText(L"Difficulty:", rect<s32>(10, 50, 100, 80), true, true);


	//Run check on combobox and increase number of enemies depending on this.
	comboBox = startGUI->addComboBox(rect<s32>(100, 50, 180, 70), NULL, 2);
	comboBox->addItem(L"EASY");
	comboBox->addItem(L"MEDIUM");
	comboBox->addItem(L"HARD");
	comboBox->setSelected(1);//Set selected to index 0, default = easy.
}


int main()
{
	CSetupDevice* setupDevice = new CSetupDevice(dimension2d<u32>(480, 200));
	
	//Check if device is created properly
	if (!setupDevice)
	{
		printf("Failed to create 'setupDevice");
		//Close application
		shutDown();
		return 1;
	}

	IGUIEnvironment* startGUI = setupDevice->getGUIEnvironment();
	startWindow(setupDevice);

	if (setupDevice->execute())
	{
		printf("Setup Quit\n");
		delete setupDevice;
		setupDevice = NULL; //Avoid memory leak
		startGUI = NULL;
		shutDown();
		return 0;
	}
	//Grab the user specified settings from the initial start GUI and store them

	//Grab which number in the list they selected for drivertype whether it be directx, opengl or w/e
	s32 driverType = ((IGUIComboBox*)startGUI->getRootGUIElement()->getElementFromId(1))->getSelected() + 1;
	//Do the same for difficulty.
	s32 difficulty = ((IGUIComboBox*)startGUI->getRootGUIElement()->getElementFromId(2))->getSelected() + 1;
	cout << difficulty << endl;
	s32 playerType = 1;//Chasing
	s32 numChasing;


	switch (difficulty)
	{
	case 1:
		numChasing = 2;
		break;
	case 2:
		numChasing = 4;
		break;
	case 3:
		numChasing = 6;
		break;
	default:
		break;
	}
	
	mouseIsDown = false;

	CAppReceiver myEventListener;
	device = createDevice(EDT_OPENGL, dimension2d<u32>(800, 600), 32, false, true, true, &myEventListener);

	ISoundEngine* engine = createIrrKlangDevice();

	if (!engine)
	{
		printf("IrrKlang device could not be created");
		return 0;
	}

	if (!device)
	{
		printf("Device could not be created");
		shutDown();
		return 0;
	}

	device->getCursorControl()->setVisible(false);
	device->run();

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	updateProgress(device, 0, &drawProgressBar);

	guienv->getSkin()->setFont(guienv->getFont("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\GillSans12.png"));
	core::stringc sceneLocation = "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\myGame.irr";
	smgr->loadScene(sceneLocation.c_str()); 

	//Start AI Manager

	AIMgr = createAIManager(device);

	if (!AIMgr)
	{
		printf("AIMgr could not be started");
		shutDown();
		return 0;
	}

	updateProgress(device, 20, &drawProgressBar);

	metaSelector = smgr->createMetaTriangleSelector();
	fillMetaSelector(device, metaSelector, smgr->getRootSceneNode());

	ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(0, 100.0f, .2f, 0, 0, true, 10.f);

	scene::ISceneNodeAnimator* animator = smgr->createCollisionResponseAnimator(
		metaSelector, camera, core::vector3df(10, 10, 10),
		vector3df(0, -10, 0), vector3df(0, 30, 0), 0.01f);
	camera->addAnimator(animator);
	animator->drop();

	updateProgress(device, 40, &drawProgressBar);

	//Load the AI data version of our map
	stringc aiSceneLocation = "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\MapAIData.irrai";
	AIMgr->loadAI(aiSceneLocation.c_str());
	AIMgr->createDebugWaypointMeshes();
	waypointGrp = AIMgr->getWaypointGroupFromIndex(0);


	updateProgress(device, 50, &drawProgressBar);

	NPCharacter* NPC;
	Character::CharacterDesc description;

	for (s32 i = 0; i < numChasing; ++i)
	{
		//Create 5 descriptions for 5 NPCharacter's and create a new instance of a 'ChasingCharacter' at the end with the description, location of model of NPC and texture
		description.sceneManager = smgr;
		description.AIManager = AIMgr;
		description.startWaypointID = -1; //Start from waypoint -1 out of 20.
		description.WaypointGroupName = waypointGrp->getName();
		NPC = new ChasingCharacter(description, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\sydney.md2", "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\sydney.jpg", metaSelector);
		
		//IF NPC cant be created...
		if (!NPC)
		{
			printf("Failed to create NPC");
		}

		NPCharacters.push_back(NPC);

	}

	
	//Create our enemy team.
	SEntityGroup* NPCTeam = AIMgr->createEntityGroup();
	SEntityGroup* playerTeam = AIMgr->createEntityGroup();


	//Loop through all characters and add them to 'NPCTeam'
	for (u32 i = 0; i < NPCharacters.size(); ++i)
	{
		if (NPCharacters[i]->getCharacterType() == ECT_CHASING) {
			NPCTeam->addEntity(NPCharacters[i]->getAIEntity());
			NPCharacters[i]->getAIEntity()->setEnemyGroup(playerTeam);
			NPCharacters[i]->getAIEntity()->setAllyGroup(NPCTeam);
		}


	updateProgress(device, 60, &drawProgressBar);

	IWaypoint* waypoint = AIMgr->getWaypointFromId(waypointGrp, 1);


	//If waypoint can be found..
	if (waypoint)
	{
		camera->setPosition(waypoint->getPosition() + core::vector3df(-120.5679, 60.2333, 180.764));
	}
	else
	{
		camera->setPosition(core::vector3df(0, 0, 0));
		camera->setTarget(core::vector3df(0, 0, 0));
	}
	//Create collision detection between camera and selectorso it doesnt pass through walls.


	//Create our player.
	Character::CharacterDesc playerDescription;
	playerDescription.sceneManager = smgr;
	playerDescription.AIManager = AIMgr;
	player = new CPlayerCharacter(playerDescription, camera, metaSelector, playerType);
	}
	if (!player)
	{
		printf("Player could not be created");
	}

	if (playerType == ECT_FLEEING) {
		playerTeam->addEntity(player->getAIEntity());
		player->getAIEntity()->setEnemyGroup(NPCTeam);
		//player->getAIEntity()->setAllyGroup(playerTeam);
	}
	else if (playerType == ECT_CHASING) {
		NPCTeam->addEntity(player->getAIEntity());
		player->getAIEntity()->setEnemyGroup(NPCTeam);
		player->getAIEntity()->setAllyGroup(playerTeam);
	}

	updateProgress(device, 70, &drawProgressBar);


	//Load projectile and explosion for when enemy dies
	//Load textures to be used in run-time, try to avoid loading stuff in
	//During game running, instead do it on start-up

	c8 path[100];
	for (s32 i = 1; i <= 11; ++i)
	{
		snprintf(path, 100, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Explosion\\%02d.jpg", i);

		driver->getTexture(path);

		updateProgress(device, (f32)(70 + i), &drawProgressBar);
	}
	
	for (s32 i = 1; i <= 7; ++i)
	{
		snprintf(path, 100, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Projectile\\portal%d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(81 + i), &drawProgressBar);
	}
		printf("explosions done");
		
	for (s32 i = 1; i <= 6; ++i)
	{
		snprintf(path, 100, "C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\Plasmaball\\%02d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(88 + i), &drawProgressBar);
	}

	updateProgress(device, 95, &drawProgressBar);


	//Set app info box top left..

	updateProgress(device, 100);
	device->sleep(500);

	engine->play2D(engine->getSoundSource("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\IrrlichtTheme.ogg"));

	s32 timeOfNPCEradication = 0;
	s32 lastFPS = -1;
	s32 startTime = device->getTimer()->getTime();
	s32 elapsedTime = 0;


	while (device->run() && !quitGame) //While our device is running..
	{
		//if (!device->isWindowActive())
		//continue;

		//Calculate elapsed time..
		elapsedTime = device->getTimer()->getTime() - startTime;
		startTime = device->getTimer()->getTime();

		//cout << "X: " << player->getAbsolutePosition().X << "Y: " << player->getAbsolutePosition().Y << "Z: " << player->getAbsolutePosition().Z << endl;
		if (NPCTeam->Entities.size() == 0)
			timeOfNPCEradication += elapsedTime;

		//Check if player is destroyed and end the game..
		if (!player || timeOfNPCEradication > 1000)
			break;

		//While game not paused
		if (!pauseGame)
		{
			//Update characters and AI Manager..

			AIMgr->update(elapsedTime);

			for (u32 c = 0; c < NPCharacters.size(); ++c)
			{
				if (NPCharacters[c]->update(elapsedTime))
				{
					delete NPCharacters[c];
					NPCharacters.erase(c);
					c--;
				}
			}

			if (player)
			{
				if (mouseIsDown)
				{
					//engine->setSoundVolume(0.2f);
					//engine->play2D(engine->getSoundSource("C:\\Users\\Leon\\Desktop\\irrlicht-1.8\\media\\shot.mp3"));
					player->fire();
				}
				if (player->update(elapsedTime))
				{
					delete player;
					player = NULL;
				}

			}
		}
	
		driver->beginScene(true, true, SColor(255, 255, 255, 255));

		smgr->drawAll();
		guienv->drawAll();


		if (player && player->getCharacterType() != ECT_SPECTATING)
		{
			//Draw player ammo and health bars to screen.

			s32 barWidth = 256;
			s32 barHeight = 16;
			s32 value = 0;
			f32 valueMax = 0.1f;
			SColor colours[4];
			stringc str;

			
			//Health bar.
			colours[0] = SColor(255, 0, 255, 0);
			colours[1] = SColor(255, 0, 255, 0);
			colours[2] = SColor(255, 0, 255, 0);
			colours[3] = SColor(255, 0, 255, 0);
			value = player->getHealth();
			valueMax = (f32)Character::MAX_HEALTH;
			str = "Health: ";

			core::rect<s32> clip(400 - (barWidth / 2), (600 - 50) - barHeight,
				(400 - (barWidth / 2)) + ((s32)(barWidth*(value / valueMax))), 600 - 50);
			driver->draw2DImage(driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\progress_bar.png"),
				core::rect<s32>(400 - (barWidth / 2), (600 - 50) - barHeight,
				400 + (barWidth / 2), 600 - 50),
				core::rect<s32>(0, 0, barWidth, barHeight),
				&clip, colours, true);
			driver->draw2DImage(driver->getTexture("C:\\Users\\Leon\\Documents\\Visual Studio 2010\\Projects\\IrrlichtProjectDemo\\media\\progress_box.png"), core::rect<s32>(400 - (barWidth / 2), (600 - 50) - barHeight, 400 + (barWidth / 2), 600 - 50), core::rect<s32>(0, 0, barWidth, barHeight), NULL, NULL, true);
			core::stringw strw = str.c_str();
			strw += value;
			guienv->getSkin()->getFont()->draw(strw.c_str(),
				core::rect<s32>(400 - (barWidth / 2), (600 - 50) - barHeight,
				400 + (barWidth / 2), 600 - 50), video::SColor(255, 255, 255, 255), true, true);
		}

		driver->endScene();
	}
}