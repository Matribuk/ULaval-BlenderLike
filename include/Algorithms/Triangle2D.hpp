#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <limits>

struct Triangle2D {
    glm::vec2 p1, p2, p3;
    glm::vec2 circumcenter;
    float circumradius;

    Triangle2D(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

    void computeCircumcircle();
    bool isPointInCircumcircle(const glm::vec2& point) const;
    bool hasVertex(const glm::vec2& vertex) const;
    bool operator==(const Triangle2D& other) const;
};
