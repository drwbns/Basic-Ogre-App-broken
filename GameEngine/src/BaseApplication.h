#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include "Globals.h"
#include "Options.h"

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

//#include "InputContext.h" // 1.9 usage only
//#include <SdkTrays.h> // Testing whether or not this is causing problems
#include <SdkCameraMan.h>



#include "RTShaderSystem\OgreRTShaderSystem.h"

namespace GameEngine {

	class RTSSDefaultTechniqueListener: public Ogre::MaterialManager::Listener
	{
	public:
	   RTSSDefaultTechniqueListener::RTSSDefaultTechniqueListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator)
	   {
		  m_ShaderGenerator = pShaderGenerator;
	   }
	   //---------------------------------------------------------------------------
	   virtual RTSSDefaultTechniqueListener::~RTSSDefaultTechniqueListener()
	   {
	   }
	   //-----

	   /** This is the hook point where shader based technique will be created.
	   It will be called whenever the material manager won't find appropriate technique
	   that satisfy the target scheme name. If the scheme name is out target RT Shader System
	   scheme name we will try to create shader generated technique for it. 
	   */
	   virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, 
		  const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, 
		  const Ogre::Renderable* rend)
	   {   
		  Ogre::Technique* generatedTech = NULL;

		  // Case this is the default shader generator scheme.
		  if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
		  {
			 bool techniqueCreated;

			 // Create shader generated technique for this material.
			 techniqueCreated = m_ShaderGenerator->createShaderBasedTechnique(
				originalMaterial->getName(), 
				Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
				schemeName);   

			 // Case technique registration succeeded.
			 if (techniqueCreated)
			 {
				// Force creating the shaders for the generated technique.
				m_ShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());
            
				// Grab the generated technique.
				Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();

				while (itTech.hasMoreElements())
				{
				   Ogre::Technique* curTech = itTech.getNext();

				   if (curTech->getSchemeName() == schemeName)
				   {
					  generatedTech = curTech;
					  break;
				   }
				}            
			 }
		  }

		  return generatedTech;
	   }

	private:
	   Ogre::RTShader::ShaderGenerator*   m_ShaderGenerator;         // The shader generator instance.      


	};
	// end class

	/* BaseApplication - The initial abstract class that a new Application should hold a pointer or reference to.
	*/
	class BaseApplication : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
	{
		public:

			/* BaseApplication Destructor */
			~BaseApplication(void)
			{
				#ifdef USETRAYMGR
					if (mTrayMgr) delete mTrayMgr;
				#endif

				//Remove ourself as a Window listener
				Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
				windowClosed(mWindow);
				delete mRoot;
			}

			BaseApplication():mShutDown(false) {};

			virtual void go();

		
				// Make this object be noncopyable because it holds a pointer
			BaseApplication(const BaseApplication &);
			const BaseApplication &operator =(const BaseApplication &);

			// Private Class Methods
			bool configure(void);

			void chooseSceneManager();

			void createCamera();

			void createFrameListener();

			void destroyScene();

			void createViewports();

			void setupResources();

			void createResourceListener();

			void loadResources();



			bool initRTSS();

			bool setup();

			void createScene(bool createLevel);

			bool frameRenderingQueued(const Ogre::FrameEvent& evt);

			bool keyPressed( const OIS::KeyEvent &arg );

			bool keyReleased( const OIS::KeyEvent &arg );

			bool mouseMoved( const OIS::MouseEvent &arg );

			bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

			bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

			void windowResized(Ogre::RenderWindow* rw);

			void windowClosed(Ogre::RenderWindow* rw);

			Ogre::SceneManager & getSceneMgr() { return *mSceneMgr; }


			void setCamera(Ogre::Camera * camera) { mCamera = camera; }
			Ogre::Camera * getCamera() { return mCamera; }
			Ogre::Root * getRoot() { return mRoot; }
			Ogre::RenderWindow * getRenderWindow() { return mWindow; }

		private:



			Ogre::Root *mRoot;
			Ogre::Camera* mCamera;
			Ogre::SceneManager* mSceneMgr;
			Ogre::RenderWindow* mWindow;
			Ogre::String mResourcesCfg;
			Ogre::String mPluginsCfg;

			#ifdef USETRAYMGR
				OgreBites::SdkTrayManager* mTrayMgr;
				OgreBites::ParamsPanel* mDetailsPanel;   // sample details panel
			#endif
			bool mCursorWasVisible;                  // was cursor visible before dialog appeared

			bool mShutDown;

			//OIS Input devices
			//OgreBites::InputContext iContext; // 1.9 usage, deprecated?
			OIS::InputManager* mInputManager;
			OIS::Mouse*    mMouse;
			OIS::Keyboard* mKeyboard;

			Ogre::RTShader::ShaderGenerator * mShaderGenerator;
			RTSSDefaultTechniqueListener * mTechniqueListener;


	};


}

#endif // #ifndef __BaseApplication_h_
