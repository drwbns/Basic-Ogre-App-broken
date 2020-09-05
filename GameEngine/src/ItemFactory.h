#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include "IItem.h"
#include <string>
#include <map>

///
/// A factory object that creates instances of different
/// 3D renderers. New renderers can be dynamically added
/// and removed from the factory object.
///
class ItemFactory
{
public:
	/// The type for the callback that creates an IRenderer instance
	typedef ItemFactory *(*CreateCallback)();

	/// Add a new 3D renderer to the system
	static void RegisterItem(const std::string &type,
								 CreateCallback cb);
	/// Remove an existing 3D renderer from the system
	static void UnregisterItem(const std::string &type);

	/// Create an instance of a named 3D renderer
	static IItem *CreateItem(const std::string &type);

private:
	typedef std::map<std::string, CreateCallback> CallbackMap;
	static CallbackMap mItems;
};

#endif