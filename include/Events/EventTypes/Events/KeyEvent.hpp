#pragma once

// Events
#include "../EventTypes.hpp"

struct KeyEvent : public Event {
    int key;
    KeyEventType type;

    KeyEvent(int k, KeyEventType t)
        : Event(EventType::KEY), key(k), type(t) {}
};