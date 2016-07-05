#ifndef INC_CIRRAIFILEWRITER_H
#define INC_CIRRAIFILEWRITER_H

#include <irrlicht.h>

using namespace irr;

namespace IrrAI {

class CAIManager;

/*! Class for writing the IrrAI file format which describes waypoint graphs etc.*/
class CIrrAIFileWriter {
   
	public:
		/**
		\brief Constructor
		\param fileSystem - the file system to write the file with
		*/
		CIrrAIFileWriter(io::IFileSystem* fileSystem) { FileSystem = fileSystem; }
		/**
		\brief Writes the contents of the aimgr to the specified file. Returns true if the process was successful, false otherwise.
		\param aimgr - the aimgr containing the waypoints etc
		\param fileName - file name to save to
		\return a bool
		*/
		bool writeToXML(CAIManager* aimgr, const c8* fileName);
   
	private:
		io::IFileSystem* FileSystem;
             
};

} // end of IrrAI namespace

#endif /* INC_CIRRAIFILEWRITER_H */
