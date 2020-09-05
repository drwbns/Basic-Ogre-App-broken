// Handles an objects inventory characteristics
#ifndef CInventory_h
#define CInventory_h

#include "IComponent.h"
#include "ItemFactory.h"

class CInventory : public IComponent {
	public:
		CInventory(){}
		~CInventory(){}
		void Update(){};
		void AddItem(IItem & item) { mItem = item; }
		void DecreaseHealth(float decreaseAmt) {
			mHealth -= decreaseAmt;
		}
		void IncreaseHealth(float health) {
			mHealth += health;
		}
	private:
		IItem mItem;
		ItemFactory mItemFactory;

};

#endif