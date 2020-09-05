// A Generic Component class
#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include <string>

// So far we're decing whether an object is a component or a property based on
// whether or not the Component is indirectly or derictly a part of the object

namespace GameEngine {
	enum ComponentType{
		MESH,TRANSFORM, RIGIDBODY, PROPERTY};

	class IComponent {
	public:
		IComponent(){}
		~IComponent(){}

		void SetType(const ComponentType cType) { mType = cType; }
		void SetName(const std::string &name) { mName = name; }

		const ComponentType& GetType() { return mType; }
		virtual void Update() = 0;

	private:
		std::string mName;
		ComponentType mType;
	};

}
#endif