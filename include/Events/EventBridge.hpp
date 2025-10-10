#pragma once

#include "Events/EventManager.hpp"
#include "Events/EventTypes/MouseEvent.hpp"
#include "Events/EventTypes/KeyEvent.hpp"

#include "Manager/InputManager.hpp"

#include "ofEvents.h"

class EventBridge {
    public:
        EventBridge(EventManager& eventManager);
        ~EventBridge();

        void setup();
        void remove();

        void onKeyPressed(ofKeyEventArgs& args);
        void onKeyReleased(ofKeyEventArgs& args);

        void onMousePressed(ofMouseEventArgs& args);
        void onMouseReleased(ofMouseEventArgs& args);
        void onMouseMoved(ofMouseEventArgs& args);
        void onMouseDragged(ofMouseEventArgs& args);
        void onMouseScrolled(ofMouseEventArgs& args);

    private:
        EventManager& _eventManager;

};
