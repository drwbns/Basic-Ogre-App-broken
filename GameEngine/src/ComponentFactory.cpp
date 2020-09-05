#include "ComponentFactory.h"
#include <iostream>

namespace GameEngine {
	// instantiate the static variable in ComponentFactory
	ComponentFactory::CallbackMap ComponentFactory::mComponents;

	void ComponentFactory::RegisterComponent(const std::string &type,
										   CreateCallback cb)
	{
		mComponents[type] = cb;
	}

	void ComponentFactory::UnregisterComponent(const std::string &type)
	{
		mComponents.erase(type);
	}

	IComponent *ComponentFactory::CreateComponent(const std::string &type)
	{
		CallbackMap::iterator it = mComponents.find(type);
		if (it != mComponents.end())
		{
			// call the creation callback to construct this derived type
			return (it->second)();
		}

		return NULL;
	}

}