// Default app starting point, serves as a base to interface with the GameEngine-----------------------------------------------------------------------------

#include "BaseApplication.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../res/resource.h"
#endif

namespace GameEngine {

	//-------------------------------------------------------------------------------------
	bool BaseApplication::configure()
	{
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->showConfigDialog())
		{
			// If returned true, user clicked OK so initialise
			// Here we choose to let the system create a default rendering window by passing 'true'
			mRoot->initialise(false, "demo");			 
			Ogre::NameValuePairList params;
			 params["border"] = "fixed";
			 mWindow = mRoot->createRenderWindow("Title", 640, 480, false, &params);

			//mWindow->create("Title", 640, 480, false, &params);
			GameEngine::Globals::getInstance()->setRenderWindow(mWindow);

			// Let's add a nice window icon
	#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			HWND hwnd;
			mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
			LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
			SetClassLong( hwnd, GCL_HICON, iconID );
			SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_THICKFRAME);
	#endif
			return true;
		}
		else
		{
			return false;
		}
	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::chooseSceneManager()
	{
		// Get the SceneManager, in this case a generic one
		mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
		GameEngine::Globals::getInstance()->setSceneManager(mSceneMgr);
	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::createCamera()
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		// Position it at 500 in Z direction
		mCamera->setPosition(Ogre::Vector3(0,0,0));
		// Look back along -Z
		mCamera->lookAt(Ogre::Vector3(0,0,-300));
		mCamera->setNearClipDistance(0.5);
		//mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
		//GameEngine::Globals::getInstance()->setCameraMan(mCameraMan);
	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::createFrameListener()
	{

		Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
		OIS::ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		mWindow->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = OIS::InputManager::createInputSystem( pl );
		GameEngine::Globals::getInstance()->setInput(mInputManager);

		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

		//iContext.mKeyboard = mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true )); // 1.9 usage
		//iContext.mMouse = mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true )); // 1.9 usage

		GameEngine::Globals::getInstance()->setKeyboard(mKeyboard);
		mMouse->setEventCallback(this);
		mKeyboard->setEventCallback(this);

		//Set initial mouse clipping size
		windowResized(mWindow);

		//Register as a Window listener
		Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

		//Ogre::OverlayManager * overlayman = new Ogre::OverlayManager(); // 1.9 only

		#ifdef USETRAYMGR
			mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mMouse, this); // 1.8 usage
			//mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, iContext, this); // 1.9 usage
			mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
			mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
			mTrayMgr->hideCursor();
		#endif

		// create a params panel for displaying sample details
		Ogre::StringVector items;
		items.push_back("cam.pX");
		items.push_back("cam.pY");
		items.push_back("cam.pZ");
		items.push_back("");
		items.push_back("cam.oW");
		items.push_back("cam.oX");
		items.push_back("cam.oY");
		items.push_back("cam.oZ");
		items.push_back("");
		items.push_back("Filtering");
		items.push_back("Poly Mode");

		#ifdef USETRAYMGR
			mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
			mDetailsPanel->setParamValue(9, "Bilinear");
			mDetailsPanel->setParamValue(10, "Solid");
			mDetailsPanel->hide();
		#endif

		mRoot->addFrameListener(this);
	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::destroyScene()
	{
		
	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::createViewports()
	{
		// Create one viewport, entire window
		Ogre::Viewport* vp = mWindow->addViewport(mCamera);
		vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

		//RTSS
		vp->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

		// Alter the camera aspect ratio to match the viewport
		mCamera->setAspectRatio(
			Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::setupResources()
	{
		// Load resource paths from config file
		Ogre::ConfigFile cf;
		cf.load(mResourcesCfg);

		// Go through all sections & settings in the file
		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

		Ogre::String secName, typeName, archName;
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
			}
		}
	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::createResourceListener()
	{

	}

	//-------------------------------------------------------------------------------------
	void BaseApplication::loadResources()
	{
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	//-------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------


	//-------------------------------------------------------------------------------------
	void BaseApplication::go()
	{
	#ifdef _DEBUG
		mResourcesCfg = "resources_d.cfg";
		mPluginsCfg = "plugins_d.cfg";
	#else
		mResourcesCfg = "resources.cfg";
		mPluginsCfg = "plugins.cfg";
	#endif

		if (!setup())
			return;


	}

	bool BaseApplication::initRTSS() {
		if (Ogre::RTShader::ShaderGenerator::initialize())
		{
			// Grab the shader generator pointer.
			mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
 
			// Add the shader libs resource location.
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("cache", "FileSystem");
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("C:\\Users\\Andrew\\Downloads\\Ogre-1_9bitbucket-latest\\SDK\\media\\RTShaderLib\\Cg", "FileSystem");
 
			// Set shader cache path.
			mShaderGenerator->setVertexShaderProfiles("VS_5_0");
			mShaderGenerator->setShaderCachePath("cache");		
 
			// Set the scene manager.
			mShaderGenerator->addSceneManager(mSceneMgr);
 
			// Add a specialized sub-render (per-pixel lighting) state to the default scheme render state
			Ogre::RTShader::RenderState* pMainRenderState = 
				mShaderGenerator->createOrRetrieveRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
			pMainRenderState->reset();

			//added
			mTechniqueListener = new RTSSDefaultTechniqueListener(mShaderGenerator);
			Ogre::MaterialManager::getSingleton().addListener(mTechniqueListener);
 
			return true;
		}
		else return false;
	}

	//-------------------------------------------------------------------------------------
	bool BaseApplication::setup()
	{
		mRoot = new Ogre::Root(mPluginsCfg);

		setupResources();

		bool carryOn = configure();
		if (!carryOn) return false;

		chooseSceneManager();
		
		createCamera();

		if(USE_RTSS)
			initRTSS();

		createViewports();

		if(USE_PAGED_G){

			//Load media (trees, grass, etc.)
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/trees", "FileSystem");
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/trees2", "FileSystem");
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/terrains", "FileSystem");
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/grass", "FileSystem");
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./", "FileSystem");
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./", "FileSystem", "BinFolder");
		}



		// Set default mipmap level (NB some APIs ignore this)
		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

		// Create any resource listeners (for loading screens)
		createResourceListener();
		// Load resources
		loadResources();
	
	

		// Create the scene
		createScene(true);

		createFrameListener();
    
		return true;
	};

	//-------------------------------------------------------------------------------------
	void BaseApplication::createScene(bool createLevel)
	{
		// Setup the scenery - lighting and skybox.
		{
			mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");

			Ogre::Light* light = mSceneMgr->createLight("sunLight");
			light->setType(Ogre::Light::LT_DIRECTIONAL);
			light->setDirection(Ogre::Vector3( 0, -1, 1 ));

			light->setDiffuseColour(Ogre::ColourValue::White);
			light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

			mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.25));
		}

		// If it's required - create the level, as it's a static mesh that won't move
		// we can create it using static geometry to optimize performance.
		/*
		if (createLevel)
		{
			Ogre::Entity* ground = mSceneMgr->createEntity("ground", "Level.mesh");
			Ogre::StaticGeometry *sg = mSceneMgr->createStaticGeometry("City");
			const int size = 1300;
			sg->setRegionDimensions(Ogre::Vector3(size));
			sg->setOrigin(Ogre::Vector3(-size/2, 0, -size/2));
			sg->addEntity(ground, Ogre::Vector3::ZERO);
			sg->build();

			// Create the collision shape and rigid body of our level.
			hkpBvTreeShape* groundShape = HkOgre::Cooker::processOgreMesh(ground);
			hkpRigidBodyCinfo ci;
			ci.m_shape = groundShape;
			ci.m_motionType = hkpMotion::MOTION_FIXED;
			ci.m_position = hkVector4( 0.0f, 0.0f, 0.0f );
			ci.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
			hkpRigidBody* groundBody = new hkpRigidBody(ci);

			mWorld->markForWrite();
			// Add the ground geometry to the physical world.
			mWorld->addEntity(groundBody)->removeReference();

			// Remove the reference to our physical
			// shape because it's unneeded.
			groundShape->removeReference();
			mWorld->unmarkForWrite();
		}
		*/
	}

	//-------------------------------------------------------------------------------------
	bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
	{
		if(mWindow->isClosed())
			return false;

		if(mShutDown)
			return false;

		//Need to capture/update each device
		mKeyboard->capture();
		mMouse->capture();

		#ifdef USETRAYMGR
			mTrayMgr->frameRenderingQueued(evt);

			if (!mTrayMgr->isDialogVisible())
			{
				//GameEngine::Globals::getInstance()->getCameraMan()->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera
				if (mDetailsPanel->isVisible())   // if details panel is visible, then update its contents
				{
					mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
					mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
					mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
					mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
					mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
					mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
					mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
				}
			}
		#endif

		return true;
	}

	//-------------------------------------------------------------------------------------
	bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
	{
		#ifdef USETRAYMGR
			if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

			if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
			{
				mTrayMgr->toggleAdvancedFrameStats();
			}
			else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
			{
				if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
				{
					mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
					mDetailsPanel->show();
				}
				else
				{
					mTrayMgr->removeWidgetFromTray(mDetailsPanel);
					mDetailsPanel->hide();
				}
			}
			else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
			{
				Ogre::String newVal;
				Ogre::TextureFilterOptions tfo;
				unsigned int aniso;

				switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
				{
				case 'B':
					newVal = "Trilinear";
					tfo = Ogre::TFO_TRILINEAR;
					aniso = 1;
					break;
				case 'T':
					newVal = "Anisotropic";
					tfo = Ogre::TFO_ANISOTROPIC;
					aniso = 8;
					break;
				case 'A':
					newVal = "None";
					tfo = Ogre::TFO_NONE;
					aniso = 1;
					break;
				default:
					newVal = "Bilinear";
					tfo = Ogre::TFO_BILINEAR;
					aniso = 1;
				}

				Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
				Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
				mDetailsPanel->setParamValue(9, newVal);
			}
			else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
			{
				Ogre::String newVal;
				Ogre::PolygonMode pm;

				switch (mCamera->getPolygonMode())
				{
				case Ogre::PM_SOLID:
					newVal = "Wireframe";
					pm = Ogre::PM_WIREFRAME;
					break;
				case Ogre::PM_WIREFRAME:
					newVal = "Points";
					pm = Ogre::PM_POINTS;
					break;
				default:
					newVal = "Solid";
					pm = Ogre::PM_SOLID;
				}

				mCamera->setPolygonMode(pm);
				mDetailsPanel->setParamValue(10, newVal);
			}
			else if(arg.key == OIS::KC_F5)   // refresh all textures
			{
				Ogre::TextureManager::getSingleton().reloadAll();
			}
			else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
			{
				mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
			}
		#endif
		if (arg.key == OIS::KC_ESCAPE)
		{
			mShutDown = true;
		}


	//    GameEngine::Globals::getInstance()->getCameraMan()->injectKeyDown(arg);
		return true;
	}

	bool BaseApplication::keyReleased( const OIS::KeyEvent &arg )
	{
		// GameEngine::Globals::getInstance()->getCameraMan()->injectKeyUp(arg);
		return true;
	}

	bool BaseApplication::mouseMoved( const OIS::MouseEvent &arg )
	{
		#ifdef USETRAYMGR
			if (mTrayMgr->injectMouseMove(arg)) return true;
		#endif
		//  GameEngine::Globals::getInstance()->getCameraMan()->injectMouseMove(arg);
		return true;
	}

	bool BaseApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		#ifdef USETRAYMGR
			if (mTrayMgr->injectMouseDown(arg, id)) return true;
		#endif

		return true;
	}

	bool BaseApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		#ifdef USETRAYMGR
			if (mTrayMgr->injectMouseUp(arg, id)) return true;
		#endif
		//GameEngine::Globals::getInstance()->getCameraMan()->injectMouseUp(arg, id);
		return true;
	}

	//Adjust mouse clipping area
	void BaseApplication::windowResized(Ogre::RenderWindow* rw)
	{
		unsigned int width, height, depth;
		int left, top;
		rw->getMetrics(width, height, depth, left, top);

		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = width;
		ms.height = height;
	}

	//Unattach OIS before window shutdown (very important under Linux)
	void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
	{
		//Only close for window that created OIS (the main window in these demos)
		if( rw == mWindow )
		{
			if( mInputManager )
			{
				mInputManager->destroyInputObject( mMouse );
				mInputManager->destroyInputObject( mKeyboard );

				OIS::InputManager::destroyInputSystem(mInputManager);
				mInputManager = 0;
			}
		}
	}

}