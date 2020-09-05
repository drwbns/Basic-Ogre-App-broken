// A Generic aircraft implementation

#ifndef _IAircraft_h
#define _IAircraft_h

#include "MovableObject.h"
#include "AircraftType.h"

namespace GameEngine {
	class IAircraft: public MovableObject {
		IAircraft(AircraftType type){
			// create Aircraft based on type passed in - factory ?
		};
		~IAircraft(){};
	};
}

#endif