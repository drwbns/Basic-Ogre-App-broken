//generic item class
#ifndef IITEM_H
#define IITEM_H

#include <string>

class IItem {
	public:
		IItem(std::string & name):mName(name){}
		~IItem(){}

		std::string GetName(){ return mName; }
	private:
		std::string mName;
};


#endif