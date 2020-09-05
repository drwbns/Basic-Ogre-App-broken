#ifndef characterOgre_h
#define characterOgre_h

#include "ICharacter.h"
#include "OgreAnimationState.h"
#include "OgreSkeleton.h"
#include "Common\Visualize\Shape\hkDisplayGeometry.h"

#include "IInputMessage.h"

#include "EngineManager.h"

using namespace Ogre;

using namespace GameEngine;


class CharacterOgre : public ICharacter {
	public:
		CharacterOgre(std::string charName, std::string camName);

		~CharacterOgre(){};

		void updatePhysics(const Ogre::Real & deltaTime);

		void updateAnimations(const Real & deltaTime);

		void updateBody(const Real & deltaTime);

		void setupBody();

		void setTopAnimation(AnimID id, bool reset = false);

		void setBaseAnimation(AnimID id, bool reset = false);

		void setupAnimations();

		void setupPhysics(hkpWorld* world);

		void injectMouseDown( const OIS::MouseEvent & evt, OIS::MouseButtonID id );

		void injectMouseMove(const OIS::MouseEvent & evt);

		void injectKeyUp(const OIS::KeyEvent & evt);

		void injectKeyDown(const OIS::KeyEvent & evt);

		void keyCheck();

		void fadeAnimations(Real deltaTime);

		 /// A static function that lets you call a member function
		static void StaticWrapper(void *obj, const Ogre::Real & deltaTime);

		void addTime(const Ogre::Real & deltaTime);

		void setProperty (const Ogre::String & key, const Ogre::String & value);

		ICharacter* createCharacterOgre();

		void receiveMessage(const IInputMessage & message, const InputAction & action);
		void sendMessage(const IInputMessage & message, const InputAction & action);

		void useCheck(){
			GameEngine::ControllableFactory::ControllableMap controllables = GameEngine::EngineManager::getInstance()->getControllableFactory()->getControllables();
			for(GameEngine::ControllableFactory::ControllableMap::iterator iter = controllables.begin(); iter != controllables.end(); iter++) {
				// Find usable objects that are near here and send a USE message to them, or closest object with a range
			}
		};
		void useCurrentItem(){};

		// Ogre Actions
		void swingSword(const OIS::MouseButtonID & id) {
				if (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)
				{
					if(!mSwordsDrawn) {
						// take swords out (or put them back, since it's the same animation but reversed)
						setTopAnimation(ANIM_DRAW_SWORDS, true);
						mTimer = 0;
					}
					// if swords are out, and character's not doing something weird, then SLICE!
					else if (id == OIS::MB_Left) setTopAnimation(ANIM_SLICE_VERTICAL, true);
					else if (id == OIS::MB_Right) setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
					mTimer = 0;
				}
		};

		bool mSwordsDrawn;
		Entity* mSword1;
		Entity* mSword2;
		RibbonTrail* mSwordTrail;
	protected:
		Ogre::Vector3 mGoalDirection;
		Ogre::Vector3 mFacingDirection;
		Ogre::Vector3 playerDir;
		Quaternion toGoal;
		size_t timer;
		size_t timer2;
		size_t timer3;
		hkpCharacterStateOnGround* myState;
};

#endif