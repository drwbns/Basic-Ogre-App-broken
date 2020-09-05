#ifndef CONTROLLABLEFACTORY_H
#define CONTROLLABLEFACTORY_H

#include "IControllable.h"
#include <string>
#include <map>

///
/// A factory object that creates instances of different
/// 3D renderers. New renderers can be dynamically added
/// and removed from the factory object.
///
namespace GameEngine {


	class ControllableFactory
	{	

	public:
		typedef std::unordered_map<std::string, IControllable*> ControllableMap;
		typedef std::unordered_map<std::string, IControllable*>::const_iterator const_iterator;

		/// The type for the callback that creates an IRenderer instance

		/// Add a new 3D renderer to the system
		void RegisterControllable(const std::string &type,
									 IControllable* cb);
		/// Remove an existing 3D renderer from the system
		void UnregisterControllable(const std::string &type);

		/// Create an instance of a named 3D renderer
		IControllable *CreateControllable(const std::string &type);
		ControllableMap &getControllables() { return mControllables; }	
	protected:
		
		ControllableMap mControllables;
	};
	
}

#endif