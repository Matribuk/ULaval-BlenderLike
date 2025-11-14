#pragma once

#include <glm/glm.hpp>
#include <vector>

class CatmullRomCurve {
public:
    static glm::vec3 evaluate(
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3,
        float t
    );

    static glm::vec3 tangent(
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3,
        float t
    );

    static std::vector<glm::vec3> generateCurve(
        const std::vector<glm::vec3>& controlPoints,
        int resolution
    );
};
