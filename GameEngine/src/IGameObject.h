// A generic Game Object class
#ifndef IGameObject_h
#define IGameObject_h

#include <string>
#include <list>

#include "IComponent.h"

namespace GameEngine {
	class IGameObject {
		public:
			IGameObject(){
			}
			~IGameObject(){}
			void SetName(const std::string &name) { mName = name; }
			void addComponent(IComponent* aComponent) { mComponents.push_back(aComponent); }
			void removeComponent(IComponent * aComponent) { mComponents.remove(aComponent); }
		private:
			std::string mName;
			std::list <IComponent*> mComponents;
	};
}

#endif