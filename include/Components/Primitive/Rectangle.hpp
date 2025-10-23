#pragma once

#include <glm/glm.hpp>

struct Rectangle {
    float width{1.0f};
    float height{1.0f};

    Rectangle() = default;
    Rectangle(float w, float h) : width(w), height(h) {}
};
