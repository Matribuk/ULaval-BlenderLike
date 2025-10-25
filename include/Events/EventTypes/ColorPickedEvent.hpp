#pragma once

#include "Events/EventTypes.hpp"
#include "Core/Entity.hpp"
#include "ofColor.h"

struct ColorPickedEvent : public Event {
    ofColor pickedColor;
    EntityID sourceEntity;

    ColorPickedEvent(ofColor color, EntityID entity)
        : Event(EventType::COLOR_PICKED),
          pickedColor(color),
          sourceEntity(entity) {}
};
