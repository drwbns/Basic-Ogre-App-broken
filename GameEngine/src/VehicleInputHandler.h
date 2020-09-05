#ifndef VehicleStyleControl_h
#define VehicleStyleControl_h


#include "IUserInputHandler.h"
#include "IControllable.h"

namespace GameEngine {
	class VehicleInputHandler
		: public IUserInputHandler
	{ 
	public:
		VehicleInputHandler() {
		}
		~VehicleInputHandler();



	};
}
#endif
