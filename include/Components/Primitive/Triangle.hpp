#pragma once

#include <glm/glm.hpp>

struct Triangle {
    glm::vec3 vertex1{-0.5f, -0.5f, 0.0f};
    glm::vec3 vertex2{0.5f, -0.5f, 0.0f};
    glm::vec3 vertex3{0.0f, 0.5f, 0.0f};

    Triangle() = default;
    Triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
        : vertex1(v1), vertex2(v2), vertex3(v3) {}
};
