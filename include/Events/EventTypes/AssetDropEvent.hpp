#pragma once

#include "Events/EventTypes.hpp"
#include "Manager/ViewportTypes.hpp"
#include "glm/glm.hpp"

struct AssetDropEvent : public Event {
    size_t assetIndex;
    glm::vec2 dropPosition;
    ViewportID viewportId;

    AssetDropEvent(size_t index, glm::vec2 pos, ViewportID vpId)
        : Event(EventType::ASSET_DROP),
          assetIndex(index),
          dropPosition(pos),
          viewportId(vpId) {}
};
