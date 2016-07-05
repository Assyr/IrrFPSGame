#include "CIrrAIFileParser.h"
#include "CAIManager.h"
#include "Utility.h"
#include "IrrAI.h"
#include "CWaypoint.h"

//#define PARSER_DEBUG
#define COPY_BUFFER_SIZE 256

namespace IrrAI { 

bool CIrrAIFileParser::parseXML(CAIManager* aimgr, const c8* fileName) {
  
	#ifdef PARSER_DEBUG
		printf("\n**********\nParsing: %s\n", fileName);
	#endif

	// read configuration from xml file
	io::IrrXMLReader* xml = io::createIrrXMLReader(fileName);//aimgr->getDevice()->getFileSystem()->createXMLReader(fileName);
	SWaypointGroup* currentGroup = NULL;

	while(xml && xml->read()) {
              
		switch(xml->getNodeType()) {
			case io::EXN_TEXT:
				break;
			case io::EXN_ELEMENT:
				if (core::stringc("FileInfo") == xml->getNodeName()) {
					f32 fileVersion = xml->getAttributeValueAsFloat("fileVersion");
					#ifdef PARSER_DEBUG
						printf("FileVersion: %.2f\n", fileVersion);
					#endif
					if (fileVersion < 0.5 || fileVersion > IRRAI_VERSION) { // 0.50 was the first IrrAI version to use this file format
						printf("%s has wrong version number (%.2f) for use with IrrAI %.2f\n", fileName, fileVersion, IRRAI_VERSION);
						//xml->drop();
						return false;
					}
					if (fileVersion < IRRAI_VERSION) {
						printf("IrrAI %.2f isn't backwards compatible with %.2f\n", IRRAI_VERSION, fileVersion);
						//xml->drop();
						return false;
					}
					s32 numWaypointGroups = xml->getAttributeValueAsInt("numWaypointGroups");
					s32 numEntities = xml->getAttributeValueAsInt("numEntities");
				} else if (core::stringc("WaypointGroup") == xml->getNodeName()) {
					core::stringc name = xml->getAttributeValueSafe("name");
					//printf("name: %s\n", name.c_str());
					s32 waypointSize = xml->getAttributeValueAsInt("waypointSize");
					//printf("size: %i\n", waypointSize);
					core::stringc colour = xml->getAttributeValueSafe("colour");
					s32 numWaypoints = xml->getAttributeValueAsInt("numWaypoints");
					//printf("numWaypoints: %i\n", numWaypoints);
					currentGroup = aimgr->createWaypointGroup();
					if (currentGroup) {
						currentGroup->setName(name.c_str());
						currentGroup->WaypointSize = waypointSize;
						video::SColor col;
						//printf("get colour\n");
						getColourFrom(colour.c_str(), colour.size()-1, &col);
						//printf("set colour\n");
						currentGroup->setColour(col);
					}
				} else if (core::stringc("Waypoint") == xml->getNodeName()) {
					s32 id = xml->getAttributeValueAsInt("id");
					core::stringc neighbours = xml->getAttributeValueSafe("neighbours");
					core::stringc position = xml->getAttributeValueSafe("position");
					core::vector3df pos;
					getVector3dfFrom(position.c_str(), position.size()-1, &pos);
					IWaypoint* waypoint = aimgr->createWaypoint(currentGroup, id, pos);
					if (waypoint) {
						((CWaypoint*)waypoint)->setNeighbourString(neighbours.c_str());
					}
				} else if (core::stringc("Entity") == xml->getNodeName()) {
					E_AIENTITY_TYPE type = (E_AIENTITY_TYPE)xml->getAttributeValueAsInt("type");
					switch (type) {
						case EAIET_COMBATNPC: {
							SCombatNPCDesc desc(xml);
							aimgr->createCombatNPC(desc);
							break;
						}
						case EAIET_PATHFINDINGNPC: {
							SNPCDesc desc(xml);
							aimgr->createPathFindingNPC(desc);
							break;
						}
						case EAIET_PLAYER: {
							SAIEntityDesc desc(xml);
							aimgr->createPlayerAIEntity(desc);
							break;
						}
						case EAIET_ENTRYEXIT_SENSOR: {
							SAIEntityDesc desc(xml);
							aimgr->createEntryExitSensor(desc);
							break;
						}
						case EAIET_UNKNOWN: {
							printf("Unknown entity type in .irrai file, not loaded\n");
							break;
						}
					}  
				}
				break;
			case io::EXN_ELEMENT_END:
				if (core::stringc("WaypointGroup") == xml->getNodeName()) {
					aimgr->linkWaypoints(currentGroup);
					currentGroup = NULL;
				}  
				break;
		}
	}

	if (xml) {
		//xml->drop();
		#ifdef PARSER_DEBUG
			printf("ParseComplete\n**********\n");
		#endif
		return true;
	}

	return false;

}

} // end of IrrAI namespace
