#include "EngineManager.h"

namespace GameEngine {

	EngineManager * EngineManager::EngineMgr = EngineManager::getInstance();

	void EngineManager::destroyHavok()
	{
		// Cleanup physics
		{
			mWorld->markForWrite();
			mWorld->removeReference();
		}
		mVisualDebugger->removeReference();

		// Contexts are not reference counted at the base class level by the VDB as
		// they are just interfaces really. So only delete the context after you have
		// finished using the VDB.
		mContext->removeReference();

		delete mJobQueue;

		// Clean up the thread pool
		mThreadPool->removeReference();
	}

	void EngineManager::initHavok()
	{
		// Base havok init:
		hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo( 500* 1024 ) );
		hkBaseSystem::init( memoryRouter, HkOgre::hkOgreErrorReport );
		{
			// Initialize the multi-threading classes, hkJobQueue, and hkJobThreadPool
			hkJobThreadPool* threadPool;

			// We can cap the number of threads used - here we use the maximum for whatever multithreaded platform we are running on. This variable is
			// set in the following code sections.
			int totalNumThreadsUsed;

			// Get the number of physical threads available on the system
			hkHardwareInfo hwInfo;
			hkGetHardwareInfo(hwInfo);
			totalNumThreadsUsed = hwInfo.m_numThreads;

			// We use one less than this for our thread pool, because we must also use this thread for our simulation
			hkCpuJobThreadPoolCinfo threadPoolCinfo;
			threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

			// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
			// timer collection will not be enabled.
			threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
			threadPool = new hkCpuJobThreadPool( threadPoolCinfo );

			// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
			// Here we only use it for physics.
			hkJobQueueCinfo info;
			info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
			hkJobQueue* jobQueue = new hkJobQueue(info);

			// Enable monitors for this thread.
			// Monitors have been enabled for thread pool threads already (see above comment).
			hkMonitorStream::getInstance().resize(200000);

			// <PHYSICS-ONLY>: Create the physics world.
			// At this point you would initialize any other Havok modules you are using.
			hkpWorld* physicsWorld;
			{
				// The world cinfo contains global simulation parameters, including gravity, solver settings etc.
				hkpWorldCinfo worldInfo;
				worldInfo.m_collisionTolerance = 0.1f;
				worldInfo.m_gravity.set(0, -9.8f, 0);
				worldInfo.m_contactPointGeneration = hkpWorldCinfo::CONTACT_POINT_ACCEPT_ALWAYS;
				worldInfo.setBroadPhaseWorldSize(50000);

				// Set the simulation type of the world to multi-threaded.
				worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
				//worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_DISCRETE;

				// Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
				worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_FIX_ENTITY;//hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;

				physicsWorld = new hkpWorld(worldInfo);

				// Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
				physicsWorld->m_wantDeactivation = false;


				// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
				// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
				// must call markForRead / markForWrite before it modifies the world to enable these checks.
				physicsWorld->markForWrite();


				// Register all collision agents, even though only box - box will be used in this particular example.
				// It's important to register collision agents before adding any entities to the world.
				hkpAgentRegisterUtil::registerAllAgents( physicsWorld->getCollisionDispatcher() );

				// We need to register all modules we will be running multi-threaded with the job queue
				physicsWorld->registerWithJobQueue( jobQueue );
				//physicsWorld->setMultithreadedAccessChecking(hkpWorld::MT_ACCESS_CHECKING_DISABLED);
			}

			// Initialize the VDB
			hkArray<hkProcessContext*> contexts;


			// <PHYSICS-ONLY>: Register physics specific visual debugger processes
			// By default the VDB will show debug points and lines, however some products such as physics and cloth have additional viewers
			// that can show geometries etc and can be enabled and disabled by the VDB app.
			hkpPhysicsContext* context;
			{
				// The visual debugger so we can connect remotely to the simulation
				// The context must exist beyond the use of the VDB instance, and you can make
				// whatever contexts you like for your own viewer types.
				context = new hkpPhysicsContext();
				hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
				context->addWorld(physicsWorld); // add the physics world so the viewers can see it
				contexts.pushBack(context);

				// Now we have finished modifying the world, release our write marker.
				physicsWorld->unmarkForWrite();
			}

			hkVisualDebugger* vdb = new hkVisualDebugger(contexts);
			vdb->serve();

			// Set the local pointers
			mVisualDebugger = vdb;
			mWorld = physicsWorld;
			mJobQueue = jobQueue;
			mThreadPool = threadPool;
			mContext = context;

			setHvkWorld(mWorld);
		}
		mIsHvkInit = true;
	}

}