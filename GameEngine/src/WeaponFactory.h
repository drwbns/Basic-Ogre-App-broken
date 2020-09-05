#ifndef WEAPONFACTORY_H
#define WEAPONFACTORY_H

#include "IWeapon.h"
#include <string>
#include <map>

///
/// A factory object that creates instances of different
/// 3D renderers. New renderers can be dynamically added
/// and removed from the factory object.
///
class WeaponFactory
{
public:
	/// The type for the callback that creates an IRenderer instance
	typedef IWeapon *(*CreateCallback)();

	/// Add a new 3D renderer to the system
	static void RegisterWeapon(const std::string &type,
								 CreateCallback cb);
	/// Remove an existing 3D renderer from the system
	static void UnregisterWeapon(const std::string &type);

	/// Create an instance of a named 3D renderer
	static IWeapon *CreateWeapon(const std::string &type);

private:
	typedef std::map<std::string, CreateCallback> CallbackMap;
	static CallbackMap mWeapons;
};

#endif