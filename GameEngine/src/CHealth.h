// Handles an objects health characteristics

#ifndef CHealth_h
#define CHealth_h

#include "IComponent.h"
namespace GameEngine {
	class CHealth : public IComponent {
		public:
			CHealth(){};
			void Update(){};
			void SetHealth(float health) { mHealth = health; }
			void DecreaseHealth(float decreaseAmt) {
				mHealth -= decreaseAmt;
			}
			void IncreaseHealth(float health) {
				mHealth += health;
			}
		private:
			float mHealth;

	};
}

#endif