#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct Transform {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::mat4 matrix{1.0f};

    Transform() = default;
    Transform(const glm::vec3& pos) : position(pos) {}
    Transform(const glm::vec3& pos, const glm::vec3& scl)
        : position(pos), scale(scl) {}
    Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
};