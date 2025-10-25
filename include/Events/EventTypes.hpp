#pragma once

#include <chrono>

enum class KeyEventType {
    Pressed,
    Released
};

enum class MouseEventType {
    Pressed,
    Released,
    Moved,
    Dragged,
    Scrolled
};

enum class EventType {
    INPUT,
    SELECTION,
    CAMERA,
    MOUSE,
    KEY,
    WINDOW,
    ASSET_DROP,
    COLOR_PREVIEW,
    COLOR_PICKED,
    EYEDROPPER_CANCELLED
};

struct Event {
    EventType type;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;

    Event(EventType t)
        : type(t), timestamp(std::chrono::steady_clock::now()) {}

    virtual ~Event() = default;
};
