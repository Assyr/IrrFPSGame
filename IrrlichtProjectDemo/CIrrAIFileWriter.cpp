#include "CIrrAIFileWriter.h"
#include "CAIManager.h"
#include "IrrAI.h"
#include "CWaypoint.h"

#define WRITER_DEBUG

namespace IrrAI {

bool CIrrAIFileWriter::writeToXML(CAIManager* aimgr, const c8* fileName) {

	io::IXMLWriter* xml = FileSystem->createXMLWriter(fileName);
  
	if (xml) {
		core::stringw strw;
		core::array<core::stringw> names;
		core::array<core::stringw> values;
		xml->writeXMLHeader();
   
		// Write out file info; version, num waypoints, num entities
		names.push_back(core::stringw(L"fileVersion"));
		c8 str[5];
		snprintf(str, 5, "%.2f", IRRAI_VERSION);
		str[4] = '\0';
		strw = str;
		values.push_back(strw);
		names.push_back(core::stringw(L"numWaypointGroups"));
		strw = core::stringw(aimgr->getWaypointGroups().size());
		values.push_back(strw);
		names.push_back(core::stringw(L"numEntities"));
		strw = core::stringw(aimgr->getEntities()->Entities.size());
		values.push_back(strw);
		xml->writeElement(L"FileInfo", false, names, values);
		xml->writeLineBreak();
		names.clear();
		values.clear();
	
		// Write out waypoint groups
		for (u32 wg = 0 ; wg < aimgr->getWaypointGroups().size() ; ++wg) {
			SWaypointGroup* group = aimgr->getWaypointGroups()[wg];
			names.push_back(core::stringw(L"name"));
			strw = group->Name.c_str();
			values.push_back(strw);
			names.push_back(core::stringw(L"waypointSize"));
			strw = core::stringw(group->WaypointSize);
			values.push_back(strw);
			names.push_back(core::stringw(L"colour"));
			video::SColor colour = group->Colour;
			strw = core::stringw(colour.getAlpha());
			strw += ",";
			strw += colour.getRed();
			strw += ",";
			strw += colour.getGreen();
			strw += ",";
			strw += colour.getBlue();
			values.push_back(strw);
			names.push_back(core::stringw(L"numWaypoints"));
			strw = core::stringw(group->Waypoints.size());
			values.push_back(strw);
			xml->writeElement(L"WaypointGroup", false, names, values);
			xml->writeLineBreak();
			names.clear();
			values.clear();

			// Write out waypoints
			for (u32 w = 0 ; w < group->Waypoints.size() ; ++w) {
				names.push_back(core::stringw(L"id"));
				strw = core::stringw(group->Waypoints[w]->getID());
				values.push_back(strw);
				names.push_back(core::stringw(L"neighbours"));
				strw = ((CWaypoint*)group->Waypoints[w])->getNeighbourString();
				values.push_back(strw);
				names.push_back(core::stringw(L"position"));
				const core::vector3df& pos = group->Waypoints[w]->getPosition();
				strw = core::stringw(pos.X);
				strw += ",";
				strw += pos.Y;
				strw += ",";
				strw += pos.Z;
				values.push_back(strw);
				xml->writeElement(L"Waypoint", true, names, values);
				xml->writeLineBreak();
				names.clear();
				values.clear();
			}

			xml->writeClosingTag(L"WaypointGroup");
			xml->writeLineBreak();
		}

		// Write out entities
		for (u32 e = 0 ; e < aimgr->getEntities()->Entities.size() ; ++e) {
			IAIEntity* entity = aimgr->getEntities()->Entities[e];
			names.push_back(core::stringw(L"type"));
			strw = core::stringw(entity->getType());
			values.push_back(strw);
			entity->writeOutXMLDescription(names, values);	
			xml->writeElement(L"Entity", true, names, values);
			xml->writeLineBreak();
			names.clear();
			values.clear();
		}

		xml->writeClosingTag(L"FileInfo"); 

		xml->drop();

		return true;
	}

	return false;
  
}

} // end of IrrAI namespace
