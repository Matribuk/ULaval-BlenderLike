#pragma once

#include "../EventTypes.hpp"

struct KeyEvent : public Event {
    int key;
    bool pressed;

    KeyEvent(int k, bool p)
        : Event(EventType::KEY), key(k), pressed(p) {}
};