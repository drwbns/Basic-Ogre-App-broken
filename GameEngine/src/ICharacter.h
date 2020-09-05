#ifndef Character_h
#define Character_h

#include "Globals.h"

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "CMovable.h"
#include "OgreCamera.h"
#include "OgreAnimationState.h"
#include "OgreMath.h"
#include "OgreVector3.h"
#include "CharacterAnimsEnum.h"

#include "hkOgreRenderable.h"
#include <hkOgrePrerequisites.h>

#include "IUserInputHandler.h"

// CharacterRigidBodies
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBodyListener.h>

// For ground
#include <Physics/Utilities/CharacterControl/hkpCharacterControl.h>

// State machine
#include <Physics/Utilities/CharacterControl/StateMachine/hkpDefaultCharacterStates.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterStateManager.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>

// Shapes
#include <Physics\Collide\Shape\Convex\Capsule\hkpCapsuleShape.h>

#include "IControllable.h"

#define NUM_ANIMS 13           // number of animations the character has
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second

namespace GameEngine {
	static struct Options
	{
		Options()
			:	mWorldCollisionTolerance( 0.1f ),
			mCharacterCylinderHeight( 1.3f	),
			mCharacterRadius( 0.6f ),
			mAllowedPenetrationDepth( -0.1f ),
			mSupportDistance( 0.1f ),
			mHardSupportDistance( 0.0f ),
			mMaxSlope( 1.05f),
			mNumStaircases( 5 ),
			mNumStepsStart( 5 ),
			mNumStepsIncrement( 4 ),
			mStairsRun( 5.0f ),
			mStairsRise( 3.0f ),
			mStepConvexRadius( hkConvexShapeDefaultRadius )
		{}

		hkReal	mWorldCollisionTolerance;	//+default(0.1f) +absmin(0.0) +absmax(1.0f)

		/// The tolerance used by the demo tolerance hack.
		hkReal	mCharacterCylinderHeight;	//+default(0.8f) +absmin(0.1f) +absmax(20.0f)
		hkReal	mCharacterRadius;			//+default(0.6f) +absmin(0.1f) +absmax(20.0f)
		hkReal	mAllowedPenetrationDepth;	//+default(-0.1f) +absmin(-5.0f) +absmax(5.0f)
		hkReal	mSupportDistance;			//+default(0.1f) +absmin(0.0) +absmax(1.0f)
		hkReal	mHardSupportDistance;		//+default(0.0f) +absmin(-0.1f) +absmax(1.0f)
		hkReal	mMaxSlope;					//+default(1.05f) +absmin(0.5f) +absmax(1.57f)
		int		mNumStaircases;				//+default(5) +absmin(2) +absmax(20)
		int		mNumStepsStart;				//+default(5) +absmin(2) +absmax(40)
		int		mNumStepsIncrement;			//+default(4) +absmin(1) +absmax(40)

		/// The angle at which to tilt the left half of the floor in radians.
		hkReal	mStairsRun;					//+default(5.f) +absmin(0.5f) +absmax(20.f)
		hkReal	mStairsRise;				//+default(3.f) +absmin(0.5f) +absmax(20.f)
		hkReal	mStepConvexRadius;			//+default(0.05f) +absmin(0.0) +absmax(1.0)
	} mOptions;

	class ICharacter: public IControllable {
		public:
			ICharacter(std::string charName, std::string camName):mRunSpeed(10), IControllable(charName, camName) {
			}
			~ICharacter(){};

			ICharacter& operator=(ICharacter& rhs);

			void setNumAnims(int numAnims){ mNumAnims = numAnims; }
			void setHeight(float height){ mHeight = height; }
			
			void setRunSpeed(float runSpeed){ mRunSpeed = runSpeed; }
			void setTurnSpeed(float turnSpeed){ mTurnSpeed = turnSpeed; }
			void setAnimFadeSpeed(float animFadeSpeed){ mAnimFadeSpeed = animFadeSpeed; }
			
			
			void setStandShape(hkpShape* standShape){ mStandShape = standShape; }
			void setCrouchShape(hkpShape* crouchShape){ mCrouchShape = crouchShape; }
			void setCharacterContext(hkpCharacterContext* characterContext){ mCharContext = characterContext; }
			void setCharRigidBody(hkpCharacterRigidBody* charRigidBody){ mCharRigidBody = charRigidBody; }

			Ogre::Vector3 & getDirection() { return mKeyDirection; }
			AnimID & getTopAnim() { return mTopAnimID; }
			AnimID & getBottomAnim() { return mBaseAnimID; }
			Ogre::AnimationState ** getAnimationStates() { return mAnims; }
			Ogre::Real & getTimer() { return mTimer; }

			const int getNumAnims() const { return mNumAnims; }
			const float getHeight() const { return mHeight; }
			
			const float getRunSpeed() const { return mRunSpeed; }
			const float getTurnSpeed() const { return mTurnSpeed; }
			const float getAnimFadeSpeed() const { return mAnimFadeSpeed; }

			hkpShape*			getStandShape() const  { return	mStandShape; }
			hkpShape*			getCrouchShape() const { return	mCrouchShape; }
			hkpCharacterRigidBody* getCharRigidBody() const { return	mCharRigidBody; }
			hkpSurfaceInfo&		getSurfaceInfo() { return	mSurfaceInfo; }
			hkpCharacterContext* getCharContext() const { return mCharContext; }


			virtual void addTime(const Ogre::Real & deltaTime) = 0;		

			virtual void setBaseAnimation(AnimID id, bool reset = false) = 0;
			virtual void setTopAnimation(AnimID id, bool reset = false) = 0;


			virtual void updatePhysics(const Ogre::Real & deltaTime) = 0 ;
			virtual void updateBody(const Ogre::Real & deltaTime) = 0 ;
			virtual void updateAnimations(const Ogre::Real & deltaTime) = 0 ;
			virtual void setupPhysics(hkpWorld* world) = 0;

			void resetCamera(Ogre::Camera* cam,const Ogre::Vector3 * position)
			{
				cam->setPosition(*position + Ogre::Vector3(0, 30, 30) );
				cam->lookAt(*position);
			}

			Ogre::String name;
				// Update of the original class


	protected:

		int		mNumAnims;				// number of animations the character has
		float	mHeight;				// height of character's center of mass above ground
		
		float	mRunSpeed;				// character running speed in units per second
		float	mTurnSpeed;				// character turning in degrees per second
		float	mAnimFadeSpeed;			// animation crossfade speed in % of full weight per second


		hkpShape		*		mStandShape;
		hkpShape		*		mCrouchShape;

		hkpCharacterRigidBody*	mCharRigidBody;
		hkpCharacterContext*	mCharContext;

		hkpSurfaceInfo		mSurfaceInfo;

		Ogre::AnimationState ** mAnims;				// master animation list
		AnimID mBaseAnimID;						// current base (full- or lower-body) animation
		AnimID mTopAnimID;						// current top (upper-body) animation
		bool * mFadingIn;						// which animations are fading in
		bool * mFadingOut;						// which animations are fading out

		Ogre::Vector3 mKeyDirection;					// player's local intended direction based on WASD keys
		Ogre::Real mTimer;							// general timer to see how long animations have been playing
		std::string mName;
	};

} // end namespace GameEngine
#endif
