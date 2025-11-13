#pragma once

#include <glm/glm.hpp>

struct Edge2D {
    glm::vec2 p1, p2;

    Edge2D(const glm::vec2& a, const glm::vec2& b);
    bool operator==(const Edge2D& other) const;
};
