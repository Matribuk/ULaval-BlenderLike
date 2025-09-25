#pragma once

#include <glm/glm.hpp>

struct Plane {
    glm::vec2 size{1.0f, 1.0f};

    Plane() = default;
    Plane(const glm::vec2& s) : size(s) {}
};
