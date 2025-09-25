#pragma once

#include "../EventTypes.hpp"

struct MouseEvent : public Event {
    int x, y;
    int button;

    MouseEvent(int px, int py, int btn)
        : Event(EventType::MOUSE), x(px), y(py), button(btn) {}
};