#pragma once

#include <glm/glm.hpp>

struct Sphere {
    float radius{1.0f};

    Sphere() = default;
    explicit Sphere(float r) : radius(r) {}
};