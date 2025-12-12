#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Core/Entity.hpp"

struct ParametricCurve {
    enum class Type {
        BEZIER_CUBIC,
        CATMULL_ROM
    };

    Type type{Type::BEZIER_CUBIC};
    std::vector<glm::vec3> controlPoints;
    int resolution{50};

    std::vector<EntityID> controlPointEntities;
    bool showControlPoints{true};
    bool showTangents{false};
    bool needsRegeneration{false};

    ParametricCurve() = default;

    ParametricCurve(Type t, const std::vector<glm::vec3>& points, int res = 50)
        : type(t), controlPoints(points), resolution(res) {}

    ParametricCurve(Type t, int res = 50)
        : type(t), resolution(res) {}
};
