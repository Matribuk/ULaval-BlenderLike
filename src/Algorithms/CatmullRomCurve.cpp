#include "Algorithms/CatmullRomCurve.hpp"

glm::vec3 CatmullRomCurve::evaluate(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3,
    float t
)
{
    float t2 = t * t;
    float t3 = t2 * t;

    glm::vec3 point = 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );

    return point;
}

glm::vec3 CatmullRomCurve::tangent(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3,
    float t
)
{
    float t2 = t * t;

    glm::vec3 tangent = 0.5f * (
        (-p0 + p2) +
        2.0f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t +
        3.0f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t2
    );

    return glm::normalize(tangent);
}

std::vector<glm::vec3> CatmullRomCurve::generateCurve(
    const std::vector<glm::vec3>& controlPoints,
    int resolution
)
{
    std::vector<glm::vec3> curvePoints;

    if (controlPoints.size() < 4) {
        return curvePoints;
    }

    int numSegments = controlPoints.size() - 3;
    curvePoints.reserve(numSegments * resolution + 1);

    for (int seg = 0; seg < numSegments; ++seg) {
        const glm::vec3& p0 = controlPoints[seg];
        const glm::vec3& p1 = controlPoints[seg + 1];
        const glm::vec3& p2 = controlPoints[seg + 2];
        const glm::vec3& p3 = controlPoints[seg + 3];

        for (int i = 0; i < resolution; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(resolution);
            curvePoints.push_back(evaluate(p0, p1, p2, p3, t));
        }
    }

    curvePoints.push_back(controlPoints[controlPoints.size() - 2]);

    return curvePoints;
}
