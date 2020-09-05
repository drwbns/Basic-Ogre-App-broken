// Handles the movable component of an object

#ifndef _Movable_h
#define _Movable_h

#include "IComponent.h"
#include "IGameObject.h"

namespace GameEngine {
	class Movable : public IComponent {
		public:
			Movable(){
			};
			~Movable(){};
	};
}

#endif