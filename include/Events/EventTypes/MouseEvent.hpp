#pragma once

#include "Events/EventTypes.hpp"

struct MouseEvent : public Event {
    int x;
    int y;
    int button;
    MouseEventType type;

    MouseEvent(int px, int py, int btn, MouseEventType t)
        : Event(EventType::MOUSE), x(px), y(py), button(btn), type(t) {}
};