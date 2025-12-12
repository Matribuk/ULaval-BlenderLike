#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Core/Entity.hpp"

struct DelaunayMesh {
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> generatedPoints;
    int numRandomPoints{15};
    glm::vec2 bounds{10.0f, 10.0f};
    unsigned int seed{42};
    enum class GenerationMode {
        RANDOM,
        GRID,
        CUSTOM
    };

    GenerationMode mode{GenerationMode::RANDOM};
    enum class DisplayMode {
        DELAUNAY_ONLY,
        VORONOI_ONLY,
        BOTH
    };

    DisplayMode displayMode{DisplayMode::DELAUNAY_ONLY};
    int gridResolution{5};
    float gridPerturbation{0.3f};

    std::vector<EntityID> controlPointEntities;
    bool showControlPoints{true};
    bool needsRegeneration{false};

    DelaunayMesh() = default;

    DelaunayMesh(int numPts, const glm::vec2& b, unsigned int s = 42)
        : numRandomPoints(numPts), bounds(b), seed(s), mode(GenerationMode::RANDOM) {}

    DelaunayMesh(const std::vector<glm::vec2>& customPoints)
        : points(customPoints), mode(GenerationMode::CUSTOM) {}
};
