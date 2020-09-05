#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include "IComponent.h"
#include <string>
#include <map>

///
/// A factory object that creates instances of different
/// 3D renderers. New renderers can be dynamically added
/// and removed from the factory object.
///
namespace GameEngine {
	class ComponentFactory
	{
	public:
		/// The type for the callback that creates an IRenderer instance
		typedef IComponent *(*CreateCallback)();

		/// Add a new 3D renderer to the system
		static void RegisterComponent(const std::string &type,
									 CreateCallback cb);
		/// Remove an existing 3D renderer from the system
		static void UnregisterComponent(const std::string &type);

		/// Create an instance of a named 3D renderer
		static IComponent *CreateComponent(const std::string &type);

	private:
		typedef std::map<std::string, CreateCallback> CallbackMap;
		static CallbackMap mComponents;
	};

}

#endif