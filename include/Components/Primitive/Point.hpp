#pragma once

#include <glm/glm.hpp>

struct Point {
    float size{1.0f};

    Point() = default;
    explicit Point(float s) : size(s) {}
};
