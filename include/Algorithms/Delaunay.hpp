#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

#include "Algorithms/Triangle2D.hpp"
#include "Algorithms/Edge2D.hpp"
#include "Algorithms/VoronoiCell.hpp"

class Delaunay {
public:
    Delaunay() = default;
    ~Delaunay() = default;

    std::vector<Triangle2D> triangulate(const std::vector<glm::vec2>& points);
    std::vector<VoronoiCell> computeVoronoi(const std::vector<glm::vec2>& points);

private:
    Triangle2D _createSuperTriangle(const std::vector<glm::vec2>& points);
    std::vector<glm::vec2> _sortVerticesCounterClockwise(
        const std::vector<glm::vec2>& vertices,
        const glm::vec2& center
    );
};
