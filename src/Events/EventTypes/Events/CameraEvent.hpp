#pragma once

#include "../EventTypes.hpp"
#include <glm/vec3.hpp>

struct CameraEvent : public Event
{
    glm::vec3 position{0.0f, 1.0f, 0.0f};
    glm::vec3 target{0.0f, 1.0f, 0.0f};

    CameraEvent(const glm::vec3& pos, const glm::vec3& tgt)
        : Event(EventType::CAMERA), position(pos), target(tgt) {}
};
