#ifndef INC_CIRRAIFILEPARSER_H
#define INC_CIRRAIFILEPARSER_H

#include <irrlicht.h>

using namespace irr;

namespace IrrAI {

class CAIManager;

/*! Class for parsing the IrrAI file format which describes waypoint graphs etc.*/
class CIrrAIFileParser {
   
	public:
		/**
		\brief Constructor
		*/
		CIrrAIFileParser() {}
		/**
		 \brief Loads the specified file and parses its contents. Returns true if the process was successful, false otherwise.
		 \param aimgr - the aimgr to receive the waypoints etc
		 \param fileName - file name of the file to parse
		 \return a bool
		 */
		 bool parseXML(CAIManager* aimgr, const c8* fileName);
    
};

} // end of IrrAI namespace

#endif /* INC_CIRRAIFILEPARSER_H */
