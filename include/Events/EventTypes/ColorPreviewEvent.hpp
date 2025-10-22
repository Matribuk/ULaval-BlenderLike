#pragma once

#include "Events/EventTypes.hpp"
#include "Core/Entity.hpp"
#include "ofColor.h"

struct ColorPreviewEvent : public Event {
    ofColor previewColor;
    EntityID sourceEntity;
    bool hasColor;

    ColorPreviewEvent(ofColor color, EntityID entity, bool valid)
        : Event(EventType::COLOR_PREVIEW),
          previewColor(color),
          sourceEntity(entity),
          hasColor(valid) {}
};
