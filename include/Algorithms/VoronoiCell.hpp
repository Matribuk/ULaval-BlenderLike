#pragma once

#include <glm/glm.hpp>
#include <vector>

struct VoronoiCell {
    glm::vec2 site;
    std::vector<glm::vec2> vertices;

    VoronoiCell() = default;
    explicit VoronoiCell(const glm::vec2& s);
};
