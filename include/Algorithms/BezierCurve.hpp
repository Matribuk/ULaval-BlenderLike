#pragma once

#include <glm/glm.hpp>
#include <vector>

class BezierCurve {
    public:
        static glm::vec3 evaluateCubic(
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3,
            float t
        );

        static glm::vec3 tangentCubic(
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
