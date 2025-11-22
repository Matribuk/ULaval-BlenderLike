#include "Algorithms/BezierCurve.hpp"

glm::vec3 BezierCurve::evaluateCubic(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3,
    float t
)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * p0;
    point += 3.0f * uu * t * p1;
    point += 3.0f * u * tt * p2;
    point += ttt * p3;

    return point;
}

glm::vec3 BezierCurve::tangentCubic(
    const glm::vec3& p0,
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec3& p3,
    float t
)
{
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;

    glm::vec3 tangent = 3.0f * uu * (p1 - p0);
    tangent += 6.0f * u * t * (p2 - p1);
    tangent += 3.0f * tt * (p3 - p2);

    return glm::normalize(tangent);
}

std::vector<glm::vec3> BezierCurve::generateCurve(
    const std::vector<glm::vec3>& controlPoints,
    int resolution
)
{
    std::vector<glm::vec3> curvePoints;

    if (controlPoints.size() < 4) {
        return curvePoints;
    }

    curvePoints.reserve(resolution + 1);

    const glm::vec3& p0 = controlPoints[0];
    const glm::vec3& p1 = controlPoints[1];
    const glm::vec3& p2 = controlPoints[2];
    const glm::vec3& p3 = controlPoints[3];

    for (int i = 0; i <= resolution; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(resolution);
        curvePoints.push_back(evaluateCubic(p0, p1, p2, p3, t));
    }

    return curvePoints;
}
