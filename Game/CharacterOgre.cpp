#include "CharacterOgre.h"
#include "Options.h"
	
EngineManager * EngineMgr = GameEngine::EngineManager::getInstance();

CharacterOgre::CharacterOgre(std::string charName, std::string camName):timer(0), timer2(0), timer3(0), ICharacter(charName, camName){

	SetCallback(this, StaticWrapper);
	mNumAnims		= 13;
	mHeight			= 10;
	setCamHeight(3);
	mRunSpeed		= 15;
	mTurnSpeed		= 300.0f;
	mAnimFadeSpeed	= 7.5f;
	mAnims		= new AnimationState*[mNumAnims];
	mFadingIn	= new bool[mNumAnims];
	mFadingOut	= new bool[mNumAnims];

	setupBody();
	setupAnimations();
	setupPhysics(EngineMgr->getHvkWorld());
	//getSceneNode()->setInheritOrientation(false);
	//getSceneNode()->setAutoTracking(true,mCameraNode);
};

void CharacterOgre::sendMessage(const IInputMessage &message, const InputAction & action) {
	if(message == SWINGSWORD && action == NULLACTION) swingSword(OIS::MB_Left);
}

void CharacterOgre::receiveMessage(const IInputMessage &message, const InputAction & action) {

	if(message == USE && action == KEYDOWN) useCheck();
	if(message == RUNFORWARD && action == KEYDOWN) getDirection().z = -1;
	if(message == STRAFELEFT && action == KEYDOWN) getDirection().x = -1;
	if(message == RUNBACKWARDS && action == KEYDOWN) getDirection().z = 1;
	if(message == STRAFERIGHT && action == KEYDOWN) getDirection().x = 1;
	if (message == JUMP && action == KEYDOWN && (getTopAnim() == ANIM_IDLE_TOP || getTopAnim() == ANIM_RUN_TOP))
	{
		// jump if on ground
		setBaseAnimation(ANIM_JUMP_START, true);
		setTopAnimation(ANIM_NONE);
		getDirection().y = 1;
		getTimer() = 0;
	}
	if (!getDirection().isZeroLength() && getBottomAnim() == ANIM_IDLE_BASE)
	{
		// start running if not already moving and the player wants to move
		setBaseAnimation(ANIM_RUN_BASE, true);
		if (getTopAnim() == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
	}
	// keep track of the player's intended direction
	if (message == RUNFORWARD && action == KEYUP && getDirection().z == -1) getDirection().z = 0;
	if (message == STRAFELEFT && action == KEYUP && getDirection().x == -1) getDirection().x = 0;
	if (message == RUNBACKWARDS && action == KEYUP && getDirection().z == 1) getDirection().z = 0;
	if (message == STRAFERIGHT && action == KEYUP && getDirection().x == 1) getDirection().x = 0;
	if (message == JUMP && action == KEYUP && getDirection().y == 1) getDirection().y = 0;
	if (message == CROUCH  && action == KEYUP && getDirection().y == -1) getDirection().y = 0;

	if (getDirection().isZeroLength() && getBottomAnim() == ANIM_RUN_BASE)
	{
		// stop running if already moving and the player doesn't want to move
		setBaseAnimation(ANIM_IDLE_BASE);
		if (getTopAnim() == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
	}
	if (message == HOLSTER && (getTopAnim() == ANIM_IDLE_TOP || getTopAnim() == ANIM_RUN_TOP))
	{
		// take swords out (or put them back, since it's the same animation but reversed)
		setTopAnimation(ANIM_DRAW_SWORDS, true);
		getTimer() = 0;
	}
	if (message == DANCE && !mSwordsDrawn)
	{
		if (getTopAnim() == ANIM_IDLE_TOP || getTopAnim() == ANIM_RUN_TOP)
		{
			// start dancing
			setBaseAnimation(ANIM_DANCE, true);
			setTopAnimation(ANIM_NONE);
			// disable hand animation because the dance controls hands
			getAnimationStates()[ANIM_HANDS_RELAXED]->setEnabled(false);
		}
		else if (getBottomAnim() == ANIM_DANCE)
		{
			// stop dancing
			setBaseAnimation(ANIM_IDLE_BASE);
			setTopAnimation(ANIM_IDLE_TOP);
			// re-enable hand animation
			getAnimationStates()[ANIM_HANDS_RELAXED]->setEnabled(true);
		}
	}
	if (message == CROUCH)
	{
		getDirection().y = -1;
	}
};

ICharacter* CharacterOgre::createCharacterOgre() {
	return this;
};

void CharacterOgre::setProperty (const Ogre::String& key, const Ogre::String& value) {
	//if(key == ?? )
		//someFunction(value);
}

void CharacterOgre::StaticWrapper(void *obj, const Ogre::Real & deltaTime) {
	CharacterOgre *self = static_cast<CharacterOgre *>(obj);
	self->addTime(deltaTime);
}

void CharacterOgre::addTime(const Ogre::Real & deltaTime)
{
	updateAnimations(deltaTime);
	updateBody(deltaTime);	
	updatePhysics(deltaTime);
					
			
	//HkOgre::Vehicle::updateCamera(*getCharRigidBody()->getRigidBody(), deltaTime, getAngularFollowCam(), getCamera());
}

void CharacterOgre::fadeAnimations(Real deltaTime)
{
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}


void CharacterOgre::injectKeyDown(const OIS::KeyEvent& evt)
{
	
}

void CharacterOgre::injectKeyUp(const OIS::KeyEvent &evt){
	// keep track of the player's intended direction
	if (evt.key == OIS::KC_W) mKeyDirection.z = 0;
	if (evt.key == OIS::KC_A) mKeyDirection.x = 0;
	if (evt.key == OIS::KC_S) mKeyDirection.z = 0;
	if (evt.key == OIS::KC_D) mKeyDirection.x = 0;
	if (evt.key == OIS::KC_SPACE) mKeyDirection.y = 0;
	if (evt.key == OIS::KC_LCONTROL) mKeyDirection.y = 0;
}

void CharacterOgre::injectMouseMove(const OIS::MouseEvent &evt){
	// update camera goal based on mouse movement
	updateCameraGoal(-0.05f * evt.state.X.rel, -0.05f * evt.state.Y.rel, -0.0005f * evt.state.Z.rel);
}

void CharacterOgre::injectMouseDown( const OIS::MouseEvent &evt, OIS::MouseButtonID id ){
	swingSword(id);
}

void CharacterOgre::setupPhysics(hkpWorld* world)
{
	world->markForWrite();
	// Create the character collision shapes
	{
		hkVector4 top( 0.0f, mOptions.mCharacterCylinderHeight * 0.5f, 0.0f );
		hkVector4 bottom( 0.0f, -mOptions.mCharacterCylinderHeight * 0.5f, 0.0f );
		hkVector4 crouchTop( 0.0f, mOptions.mCharacterCylinderHeight * 0.5f / 2, 0.0f );

		mStandShape = new hkpCapsuleShape( top, bottom, mOptions.mCharacterRadius );
		mCrouchShape = new hkpCapsuleShape( crouchTop, bottom, mOptions.mCharacterRadius );
	}
	// Construct the character rigid body
	{
		hkpCharacterRigidBodyCinfo info;
		info.m_mass = 100.0f;
		info.m_shape = mStandShape;
		info.m_maxForce = 1000.0f;
		info.m_up.set( 0.0f, 1.0f, 0.0f );
		info.m_position.set( 0.0f, 20.0f, 1.0f );
		//info.m_position.set(0.0f, 0.0f, 0.0f);
		info.m_friction = (0.0f);
		info.m_maxSlope = hkMath::min2( mOptions.mMaxSlope, HK_REAL_PI * 0.5f );
		info.m_supportDistance = mOptions.mSupportDistance;
		info.m_hardSupportDistance = mOptions.mHardSupportDistance;
		info.m_allowedPenetrationDepth = mOptions.mAllowedPenetrationDepth;

		mCharRigidBody = new hkpCharacterRigidBody( info );
		{
			hkpCharacterRigidBodyListener* listener = new hkpCharacterRigidBodyListener();
			mCharRigidBody->setListener( listener );
			listener->removeReference();
		}

		world->addEntity( mCharRigidBody->m_character );
		// Add the renderable to synchronize the 
		// body with the graphical representation
		HkOgre::Renderable* rend = new HkOgre::Renderable(getSceneNode(), mCharRigidBody->m_character, world);
	}

	// Create the Character state machine and context
	{
		hkpCharacterState* state;
		hkpCharacterStateManager* manager = new hkpCharacterStateManager();

		state = new hkpCharacterStateOnGround();
		myState = static_cast<hkpCharacterStateOnGround*>(state);
		myState->setSpeed( getRunSpeed() );
		myState->setGain( 0.1f );
		
		manager->registerState( state,	HK_CHARACTER_ON_GROUND);

		state->removeReference();

		state = new hkpCharacterStateInAir();
		manager->registerState( state,	HK_CHARACTER_IN_AIR);
		state->removeReference();

		state = new hkpCharacterStateJumping();
		manager->registerState( state,	HK_CHARACTER_JUMPING);
		state->removeReference();

		state = new hkpCharacterStateClimbing();
		manager->registerState( state,	HK_CHARACTER_CLIMBING);
		state->removeReference();

		mCharContext = new hkpCharacterContext( manager, HK_CHARACTER_ON_GROUND );
		manager->removeReference();

		// Set character type
		mCharContext->setCharacterType(hkpCharacterContext::HK_CHARACTER_RIGIDBODY);
		//mCharContext->setFilterParameters(0.5f, 20.0f,10.0f);
	}
	world->unmarkForWrite();
}

void CharacterOgre::setupAnimations()
{
	// this is very important due to the nature of the exported animations
	getEntity()->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

	String animNames[] =
	{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
	"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		mAnims[i] = getEntity()->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

	mSwordsDrawn = false;
}

void CharacterOgre::setBaseAnimation(AnimID id, bool reset)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void CharacterOgre::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void CharacterOgre::setupBody()
{

	// add gizmo for debug
	#ifdef NDEBUG 
		Ogre::SceneNode * gizmo = mBodyNode->createChildSceneNode("Gizmo");
		gizmo->setInheritOrientation(false);
		Ogre::Entity* sphere = Globals::getInstance()->getSceneMgr()->createEntity("gizmo", "gizmo2.mesh");
		//sphere->setMaterialName("justred");
		gizmo->setScale(0.05,0.05,0.05);
		//gizmo->setPosition(-5,5,-5);
		gizmo->attachObject(sphere);
			
	#endif

	getSceneNode()->setScale(Ogre::Vector3(0.25));
	setEntity(Globals::getInstance()->getSceneMgr()->createEntity("SinbadBody", "Sinbad.mesh"));
	//mBodyEnt->setMaterialName("justred");
	getSceneNode()->attachObject(getEntity());
	//mHeight = mBodyEnt->getBoundingBox().getHalfSize().y - Ogre::MeshManager::getSingleton().getBoundsPaddingFactor();
	//mBodyNode->translate(Vector3::UNIT_Y * (mHeight * mBodyNode->getScale() - 0.54));

	// create swords and attach to sheath
	LogManager::getSingleton().logMessage("Creating swords");
	mSword1 = Globals::getInstance()->getSceneMgr()->createEntity("SinbadSword1", "Sword.mesh");
	mSword2 = Globals::getInstance()->getSceneMgr()->createEntity("SinbadSword2", "Sword.mesh");
	getEntity()->attachObjectToBone("Sheath.L", mSword1);
	getEntity()->attachObjectToBone("Sheath.R", mSword2);

	LogManager::getSingleton().logMessage("Creating the chains");
	// create a couple of ribbon trails for the swords, just for fun
	NameValuePairList params;
	params["numberOfChains"] = "2";
	params["maxElements"] = "80";
	mSwordTrail = (RibbonTrail*)Globals::getInstance()->getSceneMgr()->createMovableObject("RibbonTrail", &params);
	mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
	mSwordTrail->setTrailLength(20);
	mSwordTrail->setVisible(false);
	Globals::getInstance()->getSceneMgr()->getRootSceneNode()->attachObject(mSwordTrail);


	for (int i = 0; i < 2; i++)
	{
		mSwordTrail->setInitialColour(i, 1, 0.8, 0);
		mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
		mSwordTrail->setWidthChange(i, 1);
		mSwordTrail->setInitialWidth(i, 0.5);
	}

	mKeyDirection = Vector3::ZERO;
}

void CharacterOgre::updateBody(const Real & deltaTime)
{
	timer2++;
	
	
	if (mBaseAnimID != ANIM_DANCE)
	{
		// calculate actually goal direction in world based on player's key directions
		
		#ifdef FIXED_CAM
			mGoalDirection += mKeyDirection.z * getCameraGoalOpposite()->_getDerivedOrientation().zAxis();
			mGoalDirection += mKeyDirection.x * getCameraGoalOpposite()->_getDerivedOrientation().xAxis();
		#endif
		#ifndef FIXED_CAM
			mGoalDirection += mKeyDirection.z * getCamera()->getOrientation().zAxis();
			mGoalDirection += mKeyDirection.x * getCamera()->getOrientation().xAxis();
		#endif
			mGoalDirection.y = 0;
			mGoalDirection.normalise();

		Quaternion toGoal = getSceneNode()->getOrientation().zAxis().getRotationTo(-mGoalDirection);

		// calculate how much the character has to turn to face goal direction
		Real yawToGoal = toGoal.getYaw().valueDegrees();
		if( yawToGoal != 0 ) {
			if(timer2 % 500 == 0){
				timer2 = 0;
				std::cout << "yawToGoal : " << yawToGoal << std::endl;
			}
		}
		// this is how much the character CAN turn this frame
		Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
		// reduce "turnability" if we're in midair
		if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

		// turn as much as we can, but not more than we need to
		if (yawToGoal < 0) yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); 
		else if (yawToGoal > 0) yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); 
		getSceneNode()->yaw(Degree(yawToGoal));
	} else {
		getSceneNode()->setOrientation(toGoal);
		mFacingDirection = (mGoalDirection != Ogre::Vector3::ZERO) ? mGoalDirection : mFacingDirection;
		mGoalDirection = Vector3::ZERO;   // we will calculate this
	}
	
	if (mBaseAnimID == ANIM_JUMP_LOOP &&  mSurfaceInfo.m_supportedState == hkpSurfaceInfo::SUPPORTED)
	{
		// if we've hit the ground, change to landing state
		setBaseAnimation(ANIM_JUMP_END, true);
		mTimer = 0;
	}
}

void CharacterOgre::updateAnimations(const Real &deltaTime)
{
	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;

	mTimer += deltaTime;

	if (mTopAnimID == ANIM_DRAW_SWORDS)
	{
		// flip the draw swords animation if we need to put it back
		topAnimSpeed = mSwordsDrawn ? -1 : 1;

		// half-way through the animation is when the hand grasps the handles...
		if (mTimer >= mAnims[mTopAnimID]->getLength() / 2 &&
			mTimer - deltaTime < mAnims[mTopAnimID]->getLength() / 2)
		{
			// so transfer the swords from the sheaths to the hands
			getEntity()->detachAllObjectsFromBone();
			getEntity()->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mSword1);
			getEntity()->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mSword2);
			// change the hand state to grab or let go
			mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);

			// toggle sword trails
			if (mSwordsDrawn)
			{
				mSwordTrail->setVisible(false);
				mSwordTrail->removeNode(mSword1->getParentNode());
				mSwordTrail->removeNode(mSword2->getParentNode());
			}
			else
			{
				mSwordTrail->setVisible(true);
				mSwordTrail->addNode(mSword1->getParentNode());
				mSwordTrail->addNode(mSword2->getParentNode());
			}
		}

		if (mTimer >= mAnims[mTopAnimID]->getLength())
		{
			// animation is finished, so return to what we were doing before
			if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
			else
			{
				setTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
			mSwordsDrawn = !mSwordsDrawn;
		}
	}
	else if (mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL)
	{
		if (mTimer >= mAnims[mTopAnimID]->getLength())
		{
			// animation is finished, so return to what we were doing before
			if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
			else
			{
				setTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
		}

		// don't sway hips from side to side when slicing. that's just embarrassing.
		if (mBaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
	}
	else if (mBaseAnimID == ANIM_JUMP_START)
	{
		if (mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			// takeoff animation finished, so time to leave the ground!
			setBaseAnimation(ANIM_JUMP_LOOP, true);

		}
	}
	else if (mBaseAnimID == ANIM_JUMP_END)
	{
		if (mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			// safely landed, so go back to running or idling
			if (mKeyDirection == Vector3::ZERO)
			{
				setBaseAnimation(ANIM_IDLE_BASE);
				setTopAnimation(ANIM_IDLE_TOP);
			}
			else
			{
				setBaseAnimation(ANIM_RUN_BASE, true);
				setTopAnimation(ANIM_RUN_TOP, true);
			}
		}
	}
	if (mSurfaceInfo.m_supportedState == hkpSurfaceInfo::UNSUPPORTED && mBaseAnimID != ANIM_JUMP_LOOP)
	{
		// We're falling!!!! Panic!
		setTopAnimation(ANIM_NONE);
		setBaseAnimation(ANIM_JUMP_LOOP, true);
		mTimer = 0;
	}
	// increment the current base and top animation times
	if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	fadeAnimations(deltaTime);
}

void CharacterOgre::updatePhysics(const Ogre::Real & deltaTime)
{
	timer++;


	// Havok requires a time step greater than 0 for character simulation
	if (deltaTime > 0)
	{
		hkVector4  up (0.0f, 1.0f, 0.0f );
		{
			EngineMgr->getHvkWorld()->markForWrite();
					
			//	Get user input data
			hkReal posZ = mKeyDirection.z;
			hkReal posX = (posZ == 0) ? mKeyDirection.x : 0;

			//hkReal posZ = (mKeyDirection.z == 1) ?  mKeyDirection.z : -1;
			//hkReal posX = (mKeyDirection.x == 1) ?  mKeyDirection.x : -1;

			hkBool wantJump = mKeyDirection.y == 1;

			HK_TIMER_BEGIN( "set character state", HK_NULL );
			Ogre::Vector3 camDir = getCamera()->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
			Ogre::Vector3 camDerivedDir = getCameraGoalOpposite()->_getDerivedOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
			Ogre::Vector3 cameraPivot = getCameraPivot()->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
			Ogre::Vector3 cameraGoal = getCameraGoal()->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
//			Ogre::Vector3 cameraNode = mCameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
			Ogre::Vector3 fwd = getSceneNode()->getOrientation() * getCamera()->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
			Ogre::Quaternion q = getCamera()->getOrientation();
//			Ogre::Quaternion x = mCameraNode->getOrientation();
			hkpCharacterInput input;
			hkpCharacterOutput output;
			{
				input.m_inputLR = posX;
				input.m_inputUD = posZ;

				input.m_wantJump =  wantJump;
				if(wantJump) {
					input.m_inputLR = 0;
					input.m_inputUD = 0;
				}
				input.m_atLadder = false;
				up.setNeg4( EngineMgr->getHvkWorld()->getGravity() );
				up.normalize3();
				input.m_up = up;
				Ogre::Vector3 vec;
				if (posX == 0)
					vec = (mKeyDirection.z == 1) ? Ogre::Vector3::NEGATIVE_UNIT_Z : Ogre::Vector3::UNIT_Z;
				else
					vec = (mKeyDirection.x == 1) ? Ogre::Vector3::NEGATIVE_UNIT_X : Ogre::Vector3::UNIT_X;
				input.m_forward = HkOgre::Convert::ogreVec3ToHk(getSceneNode()->_getDerivedOrientation() * vec);
				//input.m_forward = HkOgre::Convert::ogreVec3ToHk(getSceneNode()->getOrientation() * vec);
				//input.m_forward = HkOgre::Convert::ogreVec3ToHk(getCamera()->getOrientation() * vec);
				getCharRigidBody()->getRigidBody()->setPositionAndRotation(HkOgre::Convert::ogreVec3ToHk(getSceneNode()->_getDerivedPosition()), HkOgre::Convert::ogreQuatToHk(getSceneNode()->_getDerivedOrientation()));
				//input.m_forward = HkOgre::Convert::ogreVec3ToHk(mGoalDirection);

				hkStepInfo stepInfo;
				stepInfo.m_deltaTime = deltaTime;

				stepInfo.m_invDeltaTime = 1.0f/deltaTime;

				input.m_stepInfo = stepInfo;
				input.m_characterGravity.setMul4( -16.0f, up );
				input.m_velocity = mCharRigidBody->getRigidBody()->getLinearVelocity();
				input.m_position = mCharRigidBody->getRigidBody()->getPosition();
				{	
					mCharRigidBody->checkSupport( stepInfo, input.m_surfaceInfo );

					mSurfaceInfo = input.m_surfaceInfo;

					// Change character rigid body color according to ground state
					switch( input.m_surfaceInfo.m_supportedState )
					{
					case hkpSurfaceInfo::SUPPORTED:
						HK_SET_OBJECT_COLOR( (hkUlong) mCharRigidBody->getRigidBody()->getCollidable(), hkColor::rgbFromChars( 0, 255, 0, 100 ) );
						break;
					case hkpSurfaceInfo::UNSUPPORTED:
						HK_SET_OBJECT_COLOR( (hkUlong) mCharRigidBody->getRigidBody()->getCollidable(), hkColor::rgbFromChars( 0, 0, 255, 100 ) );
						break;
					default:
						HK_SET_OBJECT_COLOR( (hkUlong) mCharRigidBody->getRigidBody()->getCollidable(), hkColor::rgbFromChars( 255, 0, 0, 100 ) );
						break;
					}
				}

				HK_TIMER_END();
			}

//			myState->update(*mCharContext,input, output );

				#ifdef DEBUG

					if(timer % 500 == 0){
						// Print input.m_forward for debugging
						timer = 0;
						std::cout << "Character Pos: " << getSceneNode()->_getDerivedPosition().x << " " << getSceneNode()->_getDerivedPosition().y << " " << getSceneNode()->_getDerivedPosition().z << std::endl;	
						std::cout << "Character direction quat: " << getSceneNode()->getOrientation().x << " " << getSceneNode()->getOrientation().y << " " << getSceneNode()->getOrientation().z << " " << getSceneNode()->getOrientation().w << std::endl;					
						std::cout << "Camera Direction Quat : " << q.x << " " << q.y  << " " << q.z << " " << q.w << std::endl;
					//	std::cout << "Camera Node Quat : " << x.x << " " << x.y  << " " << x.z << " " << x.w << std::endl;
						std::cout << " " << std::endl;
						std::cout << "Character Direction Vec : " << fwd.x << " " << fwd.y  << " " << fwd.z << std::endl;
						std::cout << "Camera Direction Vec : " << camDir.x << " " << camDir.y  << " " << camDir.z << std::endl;
						std::cout << "Camera Node Vec - Derived: " << camDerivedDir.x << " " << camDerivedDir.y  << " " << camDerivedDir.z << std::endl;
						std::cout << " " << std::endl;

						std::cout << "Camera Pivot Vec : " << cameraPivot.x << " " << cameraPivot.y  << " " << cameraPivot.z << std::endl;
						std::cout << "Camera Goal Vec : " << cameraGoal.x << " " << cameraGoal.y  << " " << cameraGoal.z << std::endl;
						std::cout << "mKeyDirection : " << mKeyDirection.x << " " << mKeyDirection.y  << " " << mKeyDirection.z << std::endl;
						std::cout << "mGoalDirection : " << mGoalDirection.x << " " << mGoalDirection.y  << " " << mGoalDirection.z << std::endl;
						std::cout << "input.m_forward : " << input.m_forward.m_quad.v[0] << " " << input.m_forward.m_quad.v[1] << " " << input.m_forward.m_quad.v[2]  << " " << input.m_forward.m_quad.v[3] << std::endl;
						std::cout << "input.m_velocity : " << input.m_velocity.m_quad.v[0] << " " << input.m_velocity.m_quad.v[1] << " " << input.m_velocity.m_quad.v[2]  << " " << input.m_velocity.m_quad.v[3] << std::endl;
						std::cout << "input.m_position : " << input.m_position.m_quad.v[0] << " " << input.m_position.m_quad.v[1] << " " << input.m_position.m_quad.v[2]  << " " << input.m_position.m_quad.v[3] << std::endl;
						std::cout << " " << std::endl;
					}
				#endif // DEBUG

			// Apply the character state machine
			{
				HK_TIMER_BEGIN( "update character state", HK_NULL );

				mCharContext->update( input, output );

				HK_TIMER_END();
			}

			
			//Apply the player character controller
			{
				HK_TIMER_BEGIN( "simulate character", HK_NULL );

				// Set output velocity from state machine into character rigid body
				mCharRigidBody->setLinearVelocity(output.m_velocity, deltaTime);

				HK_TIMER_END();

				EngineMgr->getHvkWorld()->unmarkForWrite();
			}
			
			//                           /\  //
			// change to the new shape	[  ] //
			//						     \/  //
			{
				if (mKeyDirection.y == -1)
				{
					EngineMgr->getHvkWorld()->markForWrite();
					if (mCharRigidBody->getRigidBody()->getCollidable()->getShape() == mCrouchShape)
					{
						mCharRigidBody->getRigidBody()->setShape(mStandShape);
					}
					else
					{
						mCharRigidBody->getRigidBody()->setShape(mCrouchShape);
					}
					EngineMgr->getHvkWorld()->unmarkForWrite();

				}
			}
					
				
		}
	}
}