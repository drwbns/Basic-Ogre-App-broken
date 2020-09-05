#ifndef ENGINEMANAGER_H
#define ENGINEMANAGER_H

// Havok and HkOgre includes
#include <hkOgreCooker.h>
#include <hkOgreRenderable.h>
#include "ControllableFactory.h"

namespace GameEngine {
	
	class EngineManager {		
	
		private:
			EngineManager() {
				mControllableFactory= new ControllableFactory();
			}

			// Havok members
			hkpWorld*			mWorld;
			hkpPhysicsContext*	mContext;
			hkVisualDebugger*	mVisualDebugger;
			hkJobQueue*			mJobQueue;
			hkJobThreadPool*	mThreadPool;
			bool				mIsHvkInit;
			std::vector<HkOgre::Renderable*>	mRenderables;
			static EngineManager * EngineMgr;
			ControllableFactory * mControllableFactory;
		public:
			static EngineManager* getInstance() {
				if(EngineMgr)
					return EngineMgr;
				else {
					EngineMgr = new EngineManager();
					return EngineMgr;
				}
			}

			~EngineManager() {
				destroyHavok();
			}


			void initHavok();

			void destroyHavok();

			bool isHvkInitialized() { return mIsHvkInit; }

			void setHavokInit(bool value) { 
				mIsHvkInit = value; 
			}

			void setHvkWorld(hkpWorld * hvkWorld) { mWorld = hvkWorld; }
			hkpWorld * getHvkWorld() { return mWorld; }

			void setHvkPhysicsContext(hkpPhysicsContext * context) { mContext = context; }
			void setHvkVisualDebugger( hkVisualDebugger * debugger) { mVisualDebugger = debugger; }
			void setHvkJobQueue( hkJobQueue* jobQueue) { mJobQueue = jobQueue; }
			void setHvkJobThreadPool( hkJobThreadPool * pool) { mThreadPool = pool; }

			hkpPhysicsContext * gethkpPhysicsContext() { return mContext; }
			hkVisualDebugger * gethkVisualDebugger() { return mVisualDebugger; }
			hkJobQueue * gethkJobQueue() { return mJobQueue; }
			hkJobThreadPool * gethkJobThreadPool() { return mThreadPool; }
			ControllableFactory * getControllableFactory() { return mControllableFactory; }

	};
	
}

#endif