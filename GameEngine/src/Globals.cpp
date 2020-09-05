#include "Globals.h"

namespace GameEngine {
GameEngine::Globals* Globals::single = NULL;
bool Globals::instanceFlag = false;
GameEngine::Globals* myGlobals = GameEngine::Globals::getInstance();

	
}