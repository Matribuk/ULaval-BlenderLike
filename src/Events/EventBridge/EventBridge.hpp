#pragma once

#include "../EventManager/EventManager.hpp"
#include "../EventTypes/Events/MouseEvent.hpp"
#include "../EventTypes/Events/KeyEvent.hpp"
#include "ofEvents.h"

class EventBridge {
    public:
        EventBridge(EventManager& eventManager);
        ~EventBridge() = default;

        void onKeyPressed(int key);
        void onKeyReleased(int key);
        void onMousePressed(int x, int y, int button);
        void onMouseReleased(int x, int y, int button);
        void onMouseMoved(int x, int y);
        void onMouseDragged(int x, int y, int button);
        void onMouseScrolled(int x, int y, float sx, float sy);

    private:
        EventManager& _eventManager;

};
