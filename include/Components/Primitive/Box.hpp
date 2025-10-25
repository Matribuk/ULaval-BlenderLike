#pragma once

#include <glm/glm.hpp>

struct Box {
    glm::vec3 dimensions{1.0f, 1.0f, 1.0f};

    Box() = default;
    Box(const glm::vec3& dims) : dimensions(dims) {}
};
