#include "NPCharacter.h"


NPCharacter::NPCharacter(const CharacterDesc& desc, s32 type, c8* meshLocation, c8* textureLocation) : Character(desc,type)
{
	//Load MD2 animated mesh to use for our NPCharacter, this is provided in meshLocation

	IAnimatedMesh* npcMesh = sceneManager->getMesh(meshLocation);

	if (!npcMesh)
	{
		printf("npcMesh has failed to load, please check NPCharacter.cpp");
		return;
	}

	characterNode = sceneManager->addAnimatedMeshSceneNode(npcMesh);//Add our mesh to a animated mesh scene node but also
	//Pass character node this value so we know what node its in as well as represent our animated node where our mesh is

	if (!characterNode)
	{
		printf("NPC character node couldn't be created, please check NPCharacter.cpp");
		return;
	}
	characterNode->setScale(core::vector3df(0.7f, 0.7f, 0.7f));
	characterNode->setMD2Animation(EMAT_STAND); //Set the mesh in our node to a standing position
	characterNode->setMaterialFlag(video::EMF_LIGHTING, false);
	currAnimation = EMAT_STAND; //To keep track of what animation we are currently in..
	//Set texture to character node.
	characterNode->setMaterialTexture(0, sceneManager->getVideoDriver()->getTexture(textureLocation));


	//Get NPC bounding box so we can give the NPC  text scene node when it spots an enemy above there head..
	characterNode->OnAnimate(0);
	core::aabbox3d<f32> box = characterNode->getTransformedBoundingBox();

	//Add a text label using the current GUIEnvironement font which will be set in main, provide the text to be displayed, the color of it and which node to apply it to,
	//the node 
	enemyLabel = sceneManager->addTextSceneNode(sceneManager->getGUIEnvironment()->getSkin()->getFont(),
		L"Engaging Enemy", video::SColor(255, 0, 255, 0), characterNode);

	//Set the label above the NPC node head.
	enemyLabel->setPosition(core::vector3df(0.0f, (box.MaxEdge.Y - box.MinEdge.Y) / 2.0f, 0)); //Grab Max and Min Y position of the bounding box we created, and half it to find the centre.. and set the label there
	enemyLabel->setVisible(false); //Dont show this straight away, only show this when the NPC is triggered to attack the character.

	//Setup NPC so we can feed its properties to our AIManager which will create a npc using the properties we specified.
	//The properties are fed in through the constructor of this class under the name 'desc', this is a struct which contains all the properties
	//We must parse to our AIManager to handle the NPC creation
	SCombatNPCDesc npcDesc; 
	npcDesc.Range = 200.0f;
	npcDesc.UserData = this;
	npcDesc.StartWaypointID = desc.startWaypointID;
	npcDesc.WaypointGroupName = desc.WaypointGroupName;
	npcDesc.Scale = box.MaxEdge - box.MinEdge;
	npcDesc.Offset = core::vector3df(0, (box.MaxEdge.Y - box.MinEdge.Y) / 2.0f, 0);
	AIEntity = AIManager->createCombatNPC(npcDesc); //Create an NPC using the description
	//AIEntity is a entity to resemble this NPC.


	//Check to see if creation of NPC was succesful

	if (!AIEntity)
	{
		printf("Failed to create NPC, please look at NPCharacter.cpp");
		return;
	}
}


NPCharacter::~NPCharacter()
{
	//Remove any existing character nodes..

	if (characterNode)
	{
		characterNode->remove();//Remove instance
		characterNode = NULL; //Avoid memory leak.
	}

}

bool NPCharacter::update(s32 elapsedTime)
{

	if (Character::update(elapsedTime) || !((INPC*)AIEntity))
		return true; //Update just as many times as our 'Character' does...
	//Easier to keep things updated this way and in sync. No need to re-write a method we have
	//implemented.

	characterNode->setPosition(AIEntity->getAbsolutePosition()); //Move the character depending on where the entity is..
	characterNode->setRotation(AIEntity->getNode()->getRotation()); //Rotate the character depending on the entity

	//The entity is a way of resembling unique NPC's so they dont all act the same, instead they act individually.

	//Each NPC is of its own. //Once this has been updated..

	pos = characterNode->getAbsolutePosition(); //Set the 'pos' to the characterNode's absolute position

	//If debug feature has been enabled we must set the character node to true, else leave it false
	//Debug mode is built into the AiManager class which is part of IrrAI wrapper.
	if (AIManager->isDebugVisible())
	{
		if (((INPC*)AIEntity)->isVisibleToOtherEntity())
			characterNode->setMaterialFlag(video::EMF_WIREFRAME, true);
		else
			characterNode->setMaterialFlag(video::EMF_WIREFRAME, false);
	}
	else characterNode->setMaterialFlag(video::EMF_WIREFRAME, false);

	return false;
}


void NPCharacter::setAnimation(E_NPC_STATE_TYPE state)
{
	EMD2_ANIMATION_TYPE animation; //Create an animation type instance.

	switch (state) //Switch statement to check NPC state and set an animation depending on it.
	{
	case ENST_MOVING: //If NPC is moving..
		animation = EMAT_RUN; //Set the animation to a running animation
		break;
	case ENST_WAITING: //If NPC is stationary..
		animation = EMAT_STAND; //Set the animation to the standing animation...
		break;
	}

	if (currAnimation != animation) //If the current animation is not equal to animation(Are up to date animation)
	{
		characterNode->setMD2Animation(animation); //Access the node the NPC is part of and set the MD2 animation to the new animation
		//Update currAnimation so we can resume running checks with it and keep it updated.
		currAnimation = animation;
	}
}
