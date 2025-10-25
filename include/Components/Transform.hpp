#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#include "Core/Entity.hpp"

struct Transform {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::mat4 matrix{1.0f};

    EntityID parent = INVALID_ENTITY;
    std::vector<EntityID> children;

    glm::mat4 localMatrix{1.0f};
    glm::mat4 globalMatrix{1.0f};

    bool isDirty = true;

    Transform() = default;
    Transform(const glm::vec3& pos) : position(pos) {}
    Transform(const glm::vec3& pos, const glm::vec3& scl)
        : position(pos), scale(scl) {}
    Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
};