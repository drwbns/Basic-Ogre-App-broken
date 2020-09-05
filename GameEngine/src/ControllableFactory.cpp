#include "ControllableFactory.h"
#include <iostream>

namespace GameEngine {


	void ControllableFactory::RegisterControllable(const std::string &type,
										   IControllable* cb)
	{
		mControllables[type] = cb;
	}

	void ControllableFactory::UnregisterControllable(const std::string &type)
	{
		mControllables.erase(type);
	}

	IControllable *ControllableFactory::CreateControllable(const std::string &type)
	{
		ControllableMap::iterator it = mControllables.find(type);

		if (it != mControllables.end())
		{
			// call the creation callback to construct this derived type
			return (it->second);
		}

		return NULL;
	}

}