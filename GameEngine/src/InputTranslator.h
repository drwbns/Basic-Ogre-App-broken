#ifndef InputTranslator_h
#define InputTranslator_h

#include "OISKeyboard.h"
#include "IInputMessage.h"
#include <map>

namespace GameEngine {

	typedef  std::unordered_map < OIS::KeyCode, IInputMessage > InputMap;
	typedef  std::unordered_map < OIS::KeyCode, IInputMessage > InputKeyEventMap;

	class InputTranslator {
		public:
			InputTranslator(){};
			~InputTranslator(){};

			const IInputMessage getInputMessage(const OIS::KeyCode & keycode) {
				std::cout << "InputMap size : " << sizeof(inputMessageMap) << "KeyCode : " << keycode << std::endl;

				InputMap::iterator inputMessageMapIter = inputMessageMap.find(keycode);
				if (inputMessageMapIter == inputMessageMap.end()) return NULLEVENT;
				return (*inputMessageMapIter).second;
			}
			void mapInputMessage(OIS::KeyCode keycode, const IInputMessage & message) {
				inputMessageMap.insert(std::make_pair< OIS::KeyCode, IInputMessage >(keycode, message));
			}
			/*
			IInputMessage getMappedKeyEvent(OIS::KeyEvent keyEvent) {
				KeyEventMappingsIter = KeyEventMappings.find(keyEvent);
				if (KeyEventMappingsIter == KeyEventMappings.end()) return NULLEVENT;
				return (*KeyEventMappingsIter).second;
			}

			void mapKeyEvent(OIS::KeyEvent keyEvent, const IInputMessage& message) {
				KeyEventMappings.insert(std::make_pair< OIS::KeyEvent, IInputMessage >(keyEvent, message));
			}
			*/

		protected:
			InputMap inputMessageMap;

			InputKeyEventMap KeyEventMappings;
			InputKeyEventMap::const_iterator KeyEventMappingsIter;
	};
}

#endif