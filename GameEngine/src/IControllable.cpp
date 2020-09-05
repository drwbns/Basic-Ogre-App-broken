#include "IControllable.h"
#include "Options.h"

namespace GameEngine {

	void IControllable::detachCamera() {
		mCameraNode->detachAllObjects();
	}

	void IControllable::setCamera(Ogre::Camera * cam) { 
		mCamera = cam; 
		//mCameraNode->attachObject(mCamera);
	}

	bool IControllable::keyPressed(const OIS::KeyEvent & arg ) {
		if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
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
		}
		return true;
	}

	

	void IControllable::setupCamera(const std::string & camName)
	{
		mCamera = GameEngine::Globals::getInstance()->getSceneMgr()->createCamera(camName);		
		
		//mCameraPivot = GameEngine::Globals::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode();
		mCameraPivot = mSceneNode->createChildSceneNode();

		#if USE_GIZMOS
			mCameraPivot->setScale(0.1,0.1,0.1);

			Ogre::Entity * gizmo2 = GameEngine::Globals::getInstance()->getSceneMgr()->createEntity("Gizmo2", "gizmo2.mesh");
			Ogre::Entity * gizmo3 = GameEngine::Globals::getInstance()->getSceneMgr()->createEntity("Gizmo3", "gizmo2.mesh");
			Ogre::Entity * gizmo4 = GameEngine::Globals::getInstance()->getSceneMgr()->createEntity("Gizmo4", "gizmo2.mesh");
			
			mCameraPivot->attachObject(gizmo2);
		#endif
		// create a pivot at roughly the character's shoulder
		
		// this is where the camera should be soon, and it spins around the pivot
		#if USE_GIZMOS
			mCameraGoal = mCameraPivot->createChildSceneNode(Ogre::Vector3(0, 0, 30));
		#endif

		#if !USE_GIZMOS
			mCameraGoal = mCameraPivot->createChildSceneNode(Ogre::Vector3(0, 0, 300));
		#endif

		#if USE_GIZMOS
			mCameraGoal->setScale(0.1,0.1,0.1);
			mCameraGoal->attachObject(gizmo3);
		#endif

		#if !USE_GIZMOS
			mCameraGoalOpposite = mCameraPivot->createChildSceneNode(Ogre::Vector3(0, 0, -30));
		#endif

		#if USE_GIZMOS
			mCameraGoalOpposite = mCameraPivot->createChildSceneNode(Ogre::Vector3(0, 0, -300));
			mCameraGoalOpposite->setScale(0.1,0.1,0.1);
			mCameraGoalOpposite->attachObject(gizmo4);
		#endif

		// this is where the camera actually is
		mCameraNode = mCamera->getSceneManager()->getRootSceneNode()->createChildSceneNode();


		mCameraNode->setPosition(mCameraPivot->_getDerivedPosition() + mCameraGoal->_getDerivedPosition());

		mCameraPivot->setFixedYawAxis(true);
		mCameraGoal->setFixedYawAxis(true);
		mCameraNode->setFixedYawAxis(true);
		mCameraGoalOpposite->setFixedYawAxis(true);

		// our model is quite small, so reduce the clipping planes
		
		mCamera->setNearClipDistance(0.1);

		#if !FIXED_CAM
			mCamera->setPosition(0,-115,0); // Debug Fixed Point
		#endif

		#if FIXED_CAM
			mCameraPivot->setInheritOrientation(false);
			mCameraGoalOpposite->attachObject(mCamera);
			mCamera->setPosition(0,0,-10); // Debug Fixed Point
		#endif
		
		mPivotPitch = 0;

	}

	void IControllable::addTime(Ogre::Real deltaTime) {
		//if(mCamera)
		//mCamera->lookAt(mCameraGoal->getPosition());
		updateCamera(deltaTime);
		if (mCallback)
		{
			(*mCallback)(mObj,deltaTime);
		}
	}

	void IControllable::updateCamera(const Ogre::Real & deltaTime)
	{
		
		// place the camera pivot roughly at the character's shoulder
		//mCameraPivot->setPosition(getSceneNode()->getPosition());


		// move the camera smoothly to the goal
		//Ogre::Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
		Ogre::Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCamera->getPosition();
		mCameraNode->translate(goalOffset * deltaTime * 9.0f);
		//mCamera->setPosition(mCameraNode->getPosition());
		// always look at the pivot
		//mCameraNode->lookAt(mCameraGoal->_getDerivedPosition(), Ogre::Node::TS_WORLD);
		//mCamera->lookAt(mCameraGoal->_getDerivedPosition());
		mCamera->lookAt(mSceneNode->getPosition());
	}

	void IControllable::setupBody()
	{
		// create main model
		mSceneNode = GameEngine::Globals::getInstance()->getRootSceneNode()->createChildSceneNode();
	}

	void IControllable::updateCameraGoal(const Ogre::Real & deltaYaw, const Ogre::Real & deltaPitch, const Ogre::Real & deltaZoom)
	{
		mCameraPivot->yaw(Ogre::Degree(deltaYaw));

		// bound the pitch
		if (!(mPivotPitch + deltaPitch > 45 && deltaPitch > 0) &&
			!(mPivotPitch + deltaPitch < 0 && deltaPitch < 0))
		{
			mCameraGoalOpposite->pitch(Ogre::Degree(deltaPitch));
			mPivotPitch += deltaPitch;
		}

		Ogre::Real dist = mCameraGoalOpposite->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
		Ogre::Real distChange = deltaZoom * dist;

		// bound the zoom
		if (!(dist + distChange < 8 && distChange < 0) &&
			!(dist + distChange > 25 && distChange > 0))
		{

			mCameraGoalOpposite->translate(0, 0, distChange);
		}
		#if DEBUG
			std::cout << "mPivotPitch : " << mPivotPitch << std::endl;
		#endif
	}

}