// A Generic Player class
//test change
#ifndef _Player_h
#define _Player_h

#include "InputTranslator.h"
#include "Globals.h"
#include "IGameObject.h"
#include "CHealth.h"
#include "IControllable.h"

namespace GameEngine {

	class IPlayer : public IGameObject {
	public:
		IPlayer(){
			// Add player components
			addComponent(new CHealth);
			mITranslator = new InputTranslator();
			// map keys here
			
			mITranslator->mapInputMessage(OIS::KC_W, RUNFORWARD);
			mITranslator->mapInputMessage(OIS::KC_S, RUNBACKWARDS);
			mITranslator->mapInputMessage(OIS::KC_A, STRAFELEFT);
			mITranslator->mapInputMessage(OIS::KC_D, STRAFERIGHT);
			mITranslator->mapInputMessage(OIS::KC_UP, DRIVEFORWARD);
			mITranslator->mapInputMessage(OIS::KC_RIGHT, DRIVERIGHT);
			mITranslator->mapInputMessage(OIS::KC_LEFT, DRIVELEFT);
			mITranslator->mapInputMessage(OIS::KC_DOWN, DRIVEREVERSE);
			mITranslator->mapInputMessage(OIS::KC_SPACE, JUMP);
			mITranslator->mapInputMessage(OIS::KC_E, DANCE);
			mITranslator->mapInputMessage(OIS::KC_Q, HOLSTER);
			mITranslator->mapInputMessage(OIS::KC_C, CROUCH);
			mITranslator->mapInputMessage(OIS::KC_RETURN, USE);
			
			std::cout << "mITranslator size: " << sizeof(mITranslator) << std::endl;
		};
		~IPlayer(){};

		Ogre::Camera * getCamera() { return mCamera; }

		void setCamera(Ogre::Camera * cam) { mCamera = cam; }

		void setActiveControllable(IControllable* controllable) { mActiveControllable = controllable; }
		IControllable * getActiveControllable() { return mActiveControllable; }

		void setDefaultControllable(IControllable * controllable) { mDefaultControllable = controllable; }
		IControllable * getDefaultControllable() { return mDefaultControllable; }

		void update() {
			/*
			if(engineMgr->isKeyDown(OIS::KC_W))
				SendInputMessage(MITranslator->getMessage(OIS::KC_W));
				*/
		}

		void sendInputMessage(const IInputMessage &message, const InputAction & action) {
			getActiveControllable()->receiveMessage(message, action);
		}

		InputTranslator * getInputTranslator() { return mITranslator; }

	private:
		Ogre::String mName;		

		Ogre::Camera * mCamera;
		InputTranslator * mITranslator;
		IControllable * mActiveControllable;
		IControllable * mDefaultControllable;
	};
}

#endif