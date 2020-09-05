#include "IVehicle.h"

#include "EngineManager.h"

static GameEngine::EngineManager * EngineMgr = GameEngine::EngineManager::getInstance();

namespace GameEngine {
	

	void IVehicle::injectKeyDown(const OIS::KeyEvent &evt) {
		if		(evt.key == OIS::KC_W)		mSteering.z = -1;
		else if (evt.key == OIS::KC_A)		mSteering.x = -1;
		if		(evt.key == OIS::KC_S)		mSteering.z = 1;
		else if (evt.key == OIS::KC_D)		mSteering.x = 1;
		if		(evt.key == OIS::KC_SPACE)	mSteering.y = 1;
	}

	void IVehicle::injectKeyUp(const OIS::KeyEvent &evt){
		if		(evt.key == OIS::KC_W		&& mSteering.z == -1)	mSteering.z = 0;
		else if (evt.key == OIS::KC_A		&& mSteering.x == -1)	mSteering.x = 0;
		if		(evt.key == OIS::KC_S		&& mSteering.z == 1)	mSteering.z = 0;
		else if (evt.key == OIS::KC_D		&& mSteering.x == 1)	mSteering.x = 0;
		if		(evt.key == OIS::KC_SPACE	&& mSteering.y == 1)	mSteering.y = 0;
	
		else if (evt.key == OIS::KC_T)
		{
			EngineMgr->getHvkWorld()->markForWrite();
			chassisRigidBody->setRotation(hkQuaternion::getIdentity());
			chassisRigidBody->setPosition(hkVector4(0, 5, 0));
			chassisRigidBody->setLinearVelocity(hkVector4(0, 0, 0));
			chassisRigidBody->setAngularVelocity(hkVector4(0, 0, 0, 0));
			EngineMgr->getHvkWorld()->unmarkForWrite();
		}
	}
		
	void IVehicle::updatePhysics(const Ogre::Real & deltaTime){
					// Updating the demo vehicle takes 3 steps:
				//		* Steering it.
		HkOgre::Vehicle::steer(*getVehicleInstance(), getSteering(), deltaTime);

				//		* Update the vehicle position and orientation.
		HkOgre::Vehicle::syncWheels(*getVehicleInstance(), getWheels());	
	}


	void IVehicle::setupBody() {
		// Create the actual vehicle.
		// Create the graphical represenatation of the chassis.mesh
			

		Ogre::SceneNode* chassisNode = getSceneNode()->createChildSceneNode("chassis");
		chassisNode->attachObject(Globals::getInstance()->getSceneMgr()->createEntity("evo chassis", "Evo-Chassis.mesh"));


		// Since this is a demo, the chassis is created in the demo utilities' createDemoChassis function.
		hkpConvexVerticesShape* chassisShape = HkOgre::Vehicle::createDemoChassis(); 

		// The inertia value is reset by the vehicle SDK.  However we give it a
		// reasonable value so that the hkpRigidBody does not assert on construction. See
		// VehicleSetup for the yaw, pitch and roll values used by hkVehicle.
		chassisInfo.m_mass = 750.0f;	
		chassisInfo.m_shape = chassisShape;
		chassisInfo.m_friction = 0.4f;

		// The chassis MUST have m_motionType hkpMotion::MOTION_BOX_INERTIA to correctly simulate
		// vehicle roll, pitch and yaw.
		chassisInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
		chassisInfo.m_position = hkVector4(0, 10, 0);
		hkpInertiaTensorComputer::setShapeVolumeMassProperties(chassisInfo.m_shape,
			chassisInfo.m_mass,
			chassisInfo);

		chassisRigidBody = new hkpRigidBody(chassisInfo);
		
		EngineMgr->getHvkWorld()->markForWrite();
		
		// Now create the graphical representations of the wheels.
		for (int i = 0; i < 4; i++)
		{
			mWheels.push_back(Globals::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode());
			mWheels[i]->attachObject(Globals::getInstance()->getSceneMgr()->createEntity("Evo-Wheel.mesh"));
		}

		// Create a HkOgre::Renderable so that the graphical
		// representation is synchronized with the physical one.
		HkOgre::Renderable* carRend = new HkOgre::Renderable(getSceneNode(), chassisRigidBody, EngineMgr->getHvkWorld()); 
		mVehicleInstance = new hkpVehicleInstance(chassisRigidBody);
			
		// No longer need references to these as the vehicle now has them
		chassisShape->removeReference();
		chassisRigidBody->removeReference();
		EngineMgr->getHvkWorld()->unmarkForWrite();

			
	}

	void IVehicle::addTime(const Ogre::Real & deltaTime)
	{		
		//		* Updating the camera, since Havok is controlling it.
		IControllable::addTime(deltaTime);
		updatePhysics(deltaTime);
		updateBody(deltaTime);			
		updateAnimations(deltaTime);
		HkOgre::Vehicle::updateCamera(*chassisRigidBody, deltaTime, getAngularFollowCam(), getCamera());
	}

}