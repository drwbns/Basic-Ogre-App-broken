//generic weapon class
#ifndef IWEAPON_H
#define IWEAPON_H

#include <string>

class IWeapon {
	public:
		IWeapon(std::string & name):mName(name){}
		~IWeapon(){}

		std::string GetName(){ return mName; }
	private:
		std::string mName;
};


#endif