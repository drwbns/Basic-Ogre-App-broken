#ifndef _IVehicle_h
#define _IVehicle_h

#include "CMovable.h"
#include "IGameObject.h"
#include "VehicleType.h"
#include "IControllable.h"
#include "Globals.h"
#include <Physics/Vehicle/hkpVehicleInstance.h>
#include "hkOgreRenderable.h"

#include "EngineManager.h"

#include "ICharacter.h"
#include "IInputMessage.h"



namespace GameEngine {
	class IVehicle:  public IControllable, public IGameObject {
	public:
		IVehicle(const std::string name, VehicleType Vtype) : IControllable(name, "VehicleCam") {
			//create Vehicle based on type passed in - factory ?
			
			if ( Vtype == CAR ) {
				// Vehicle
				mSteering = Ogre::Vector3(0, 0, 0);
			}
			setupBody();
			// new method
			EngineManager::getInstance()->getControllableFactory()->RegisterControllable("Vehicle", this);
		};
		~IVehicle(){};

		void receiveMessage(const IInputMessage & message, const InputAction & action) {

			//KeyDown
			if		(message == DRIVEFORWARD && action == KEYDOWN)		getSteering().z = -1;
			if (message == DRIVELEFT && action == KEYDOWN)		getSteering().x = -1;
			if		(message == DRIVEREVERSE && action == KEYDOWN)		getSteering().z = 1;
			if (message == DRIVERIGHT && action == KEYDOWN)		getSteering().x = 1;
			if(message == DRIVEBRAKE && action == KEYDOWN)	getSteering().y = 1;
		};

		void sendMessage(const IInputMessage & message, const InputAction & action){};

		bool isPlayerInVehicle() { return mHasDriver; }

		void enterVehicle( ICharacter * driver )
		{
			mDriver = driver;
			mHasDriver = true;

			Globals::getInstance()->getSceneMgr()->getCurrentViewport()->setCamera(getCamera());
			HkOgre::Vehicle::createCamera(getAngularFollowCam()); // reset the camera
		}
		void exitVehicle()
		{
			Globals::getInstance()->getSceneMgr()->getCurrentViewport()->setCamera(getCamera());
			HkOgre::Vehicle::createCamera(getAngularFollowCam());
		}


		void injectKeyDown(const OIS::KeyEvent &evt);

		void injectKeyUp(const OIS::KeyEvent &evt);

		void injectMouseMove(const OIS::MouseEvent &evt){};

		void injectMouseDown( const OIS::MouseEvent &evt, OIS::MouseButtonID id ){};

		void updatePhysics(const Ogre::Real & deltaTime);

		void updateBody(const Ogre::Real & deltaTime){};

		void updateAnimations(const Ogre::Real & deltaTime){};

		void setProperty (const Ogre::String& key, const Ogre::String& value){}

		void setupBody();

		void addTime(const Ogre::Real & deltaTime);

		Ogre::Vector3& getSteering() { return mSteering; }

		std::vector<Ogre::SceneNode*> &getWheels() { return mWheels; }

		hkpVehicleInstance * getVehicleInstance() { return mVehicleInstance; }
	private:
		// User input to drive vehicle.
		Ogre::Vector3 mSteering;
		std::vector<Ogre::SceneNode*> mWheels;
		hkpRigidBody* chassisRigidBody;
		hkpRigidBodyCinfo chassisInfo;
		hkpVehicleInstance * mVehicleInstance;
		Movable * mMovable; // This vehicle's movable property
		ICharacter * mDriver;
		bool mHasDriver;
	};
}

#endif