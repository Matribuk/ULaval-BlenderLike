#pragma once

#include <glm/glm.hpp>

struct Line {
    glm::vec3 start{0.0f, 0.0f, 0.0f};
    glm::vec3 end{1.0f, 0.0f, 0.0f};

    Line() = default;
    Line(const glm::vec3& s, const glm::vec3& e) : start(s), end(e) {}
};
