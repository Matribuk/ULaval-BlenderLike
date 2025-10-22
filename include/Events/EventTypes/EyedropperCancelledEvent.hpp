#pragma once

#include "Events/EventTypes.hpp"

struct EyedropperCancelledEvent : public Event {
    EyedropperCancelledEvent()
        : Event(EventType::EYEDROPPER_CANCELLED) {}
};
