#ifndef _IControllable_h
#define _IControllable_h

#include <hkOgrePrerequisites.h>
#include <Physics/Vehicle/hkpVehicleInstance.h>
#include <Physics/Vehicle/Camera/hkp1dAngularFollowCam.h>
#include <Vehicle/VehicleSetup.h>
#include <Vehicle/VehicleUtils.h>
#include "Globals.h"
#include "IController.h"
#include "IUserInputHandler.h"
#include "IInputMessage.h"

typedef void (*CallbackType)(void * obj, const Ogre::Real & deltaTime);

namespace GameEngine {

//	typedef Ogre::SharedPtr<IControllable> SharedPtrControllable;

	class IControllable {
		public:
			IControllable(const std::string name, const std::string &camName): mCallback(NULL), mObj(NULL){


				setupBody();
				setupCamera(camName);
			};
			~IControllable(){};

			/// A type for the function signature that is accepted as a callback
			

			void SetCallback(void * obj, CallbackType cb)
			{
				mObj = obj;
				mCallback = cb;
			}
			/*
			void IControllable::DoAction()
			{
				if (mCallback)
				{
					(*mCallback)("Hello World", mClosure);
				}
			}
			*/
			virtual void receiveMessage(const IInputMessage & message, const InputAction & action) = 0;
			virtual void sendMessage(const IInputMessage & message, const InputAction & action) = 0;

			virtual void setupBody();

			void setCamHeight(float camHeight){ mCamHeight = camHeight; }
			const float getCamHeight() const { return mCamHeight; }
			void setBodyNode(Ogre::SceneNode * sceneNode){ mSceneNode = sceneNode; }
			virtual void updatePhysics(const Ogre::Real & deltaTime) = 0 ;
			virtual void updateBody(const Ogre::Real &deltaTime) = 0 ;
			virtual void updateAnimations(const Ogre::Real &deltaTime) = 0 ;
			virtual void injectKeyDown(const OIS::KeyEvent & evt) = 0;
			virtual void injectKeyUp(const OIS::KeyEvent &evt) = 0;
			virtual void injectMouseMove(const OIS::MouseEvent &evt) = 0;
			virtual void injectMouseDown( const OIS::MouseEvent &evt, OIS::MouseButtonID id ) = 0;

			void setControlledStatus(bool controlledStatus) { statusControlled = controlledStatus; }
			bool getControlledStatus() { return statusControlled; }

			hkp1dAngularFollowCam& getAngularFollowCam() { return mAngularCamera; };

			void detachCamera();

			void setCamera(Ogre::Camera * cam);

			bool keyPressed(const OIS::KeyEvent & arg );

			virtual void setProperty (const Ogre::String& key, const Ogre::String& value) = 0;

			void setupCamera(const std::string & camName);

			virtual void addTime(Ogre::Real deltaTime);

			void updateCamera(const Ogre::Real & deltaTime);

			void updateCameraGoal(const Ogre::Real & deltaYaw,const Ogre::Real & deltaPitch,const Ogre::Real & deltaZoom);

			Ogre::Camera * getCamera() { return mCamera; }
			Ogre::Entity* getEntity() const { return mEntity; }
			void setEntity(Ogre::Entity * entity) { mEntity = entity; }

			Ogre::SceneNode * getCameraNode() { return mCameraNode; }
			Ogre::SceneNode * getSceneNode() { return mSceneNode; }
			Ogre::SceneNode * getCameraPivot() { return mCameraPivot; }
			Ogre::SceneNode * getCameraGoal() { return mCameraGoal; }
			Ogre::SceneNode * getCameraGoalOpposite() { return mCameraGoalOpposite; }
		private:
			hkp1dAngularFollowCam mAngularCamera;
			Ogre::SceneNode* mCameraPivot;
			Ogre::SceneNode* mCameraGoal;
			Ogre::SceneNode* mCameraGoalOpposite;
			Ogre::SceneNode* mCameraNode;
			Ogre::Camera * mCamera;
			Ogre::SceneNode* mSceneNode;
			Ogre::Entity* mEntity;

			Ogre::Real mPivotPitch;
			float	mCamHeight;				// height of camera above character's center of mass
			bool statusControlled;
		private:

			CallbackType mCallback;
			void         *mObj;
	};

};
#endif
