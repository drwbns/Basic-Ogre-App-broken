// Singleton class that holds all common globals for the game
#ifndef _Globals_h
#define _Globals_h

#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "OgreCamera.h"
#include "hkOgrePrerequisites.h"
#include "hkpWorld.h"

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkCameraMan.h>
#include <SdkTrays.h>

namespace GameEngine {
	class Globals
	{
		private:
    
			Globals()
			{
			}	
			static bool instanceFlag;
			static Globals *single;
		public:
			~Globals()
			{
				instanceFlag = false;
			}
			static Globals* getInstance()
			{
				if(! instanceFlag)
				{
					single = new Globals();
					instanceFlag = true;
					return single;
				}
				else
				{
					return single;
				}
			}
			void setSceneManager(Ogre::SceneManager * sm) { mSceneMgr = sm; }
			Ogre::SceneManager * getSceneMgr() { 
				return mSceneMgr; 
			}
			Ogre::SceneNode * getRootSceneNode() { return mSceneMgr->getRootSceneNode(); }
			void setInput(OIS::InputManager * input) { mInput = input; }
			OIS::InputManager * getInput() { return mInput; }
			void setKeyboard(OIS::Keyboard * keyboard) { mKeyboard = keyboard; }
			OIS::Keyboard * getKeyboard() { return mKeyboard; }

			void setRenderWindow(Ogre::RenderWindow *rw) { mRenderWindow = rw; }
			Ogre::RenderWindow * getRenderWindow() { return mRenderWindow; }
		protected:
			hkpWorld * mHvkWorld;
			Ogre::SceneManager * mSceneMgr;
			Ogre::RenderWindow * mRenderWindow;
			OIS::InputManager * mInput;
			OIS::Keyboard * mKeyboard;

	};
} // end namespace

#endif