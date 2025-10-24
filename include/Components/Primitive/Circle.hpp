#pragma once

#include <glm/glm.hpp>

struct Circle {
    float radius{1.0f};
    int segments{32};

    Circle() = default;
    explicit Circle(float r, int segs = 32) : radius(r), segments(segs) {}
};
