#include "Utility.h"

namespace IrrAI { 

void getColourFrom(const c8* readBuffer, s32 bufferSize, video::SColor* col) {

	if (!readBuffer || bufferSize == 0) return;

	const c8* pReadBuffer = readBuffer;
	s32 colour[4] = {0};
	s32 i = 0;
  
	// Parse out the colour, should be a comma delimted list of 4 integers
	while (pReadBuffer-readBuffer < bufferSize && i < 4) {
		colour[i++] = atoi(pReadBuffer);
		if (i > 3) break;
		while (pReadBuffer[0] != ',') ++pReadBuffer; // find the next comma
		++pReadBuffer; // skip the comma      
	}
  
	if (col) col->set(colour[0],colour[1],colour[2],colour[3]); 
    
}

void getVector3dfFrom(const c8* readBuffer, s32 bufferSize, core::vector3df* vec) {

	if (!readBuffer || bufferSize == 0) return;
            
	const c8* pReadBuffer = readBuffer;
	f32 vector[3] = {0};
	s32 i = 0;
  
	// Parse out the vector, should be a comma delimted list of 3 floats
	while (pReadBuffer-readBuffer < bufferSize && i < 3) {
		vector[i++] = (f32)atof(pReadBuffer);
		if (i > 2) break;
		while (pReadBuffer[0] != ',') ++pReadBuffer; // find the next comma
		++pReadBuffer; // skip the comma      
	}
  
	if (vec) vec->set(vector[0],vector[1],vector[2]);
                 
}

void getDimension2dfFrom(const c8* readBuffer, s32 bufferSize, core::dimension2df* dim) {

	if (!readBuffer || bufferSize == 0) return;
            
	const c8* pReadBuffer = readBuffer;
	f32 dimension[2] = {0};
	s32 i = 0;
  
	// Parse out the vector, should be a comma delimted list of 3 floats
	while (pReadBuffer-readBuffer < bufferSize && i < 2) {
		dimension[i++] = (f32)atof(pReadBuffer);
		if (i > 1) break;
		while (pReadBuffer[0] != ',') ++pReadBuffer; // find the next comma
		++pReadBuffer; // skip the comma      
	}
  
	if (dim) dim->set(dimension[0],dimension[1]);
                 
}

} // end of IrrAI namespace
