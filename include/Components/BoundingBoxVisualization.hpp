#pragma once

#include <glm/glm.hpp>

struct BoundingBoxVisualization {
    enum class Type {
        NONE,
        AABB,
        SPHERE
    };

    Type type = Type::AABB;
    bool visible = false;
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

    BoundingBoxVisualization() = default;

    BoundingBoxVisualization(Type t, bool vis = true, glm::vec4 col = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f))
        : type(t), visible(vis), color(col) {}
};
