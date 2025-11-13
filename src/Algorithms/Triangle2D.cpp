#include "Algorithms/Triangle2D.hpp"

Triangle2D::Triangle2D(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
    : p1(a), p2(b), p3(c)
{
    computeCircumcircle();
}

void Triangle2D::computeCircumcircle()
{
    float ax = p1.x, ay = p1.y;
    float bx = p2.x, by = p2.y;
    float cx = p3.x, cy = p3.y;

    float d = 2.0f * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));

    if (std::abs(d) < 1e-6f) {
        circumcenter = (p1 + p2 + p3) / 3.0f;
        circumradius = std::numeric_limits<float>::max();
        return;
    }

    float ux = ((ax * ax + ay * ay) * (by - cy) +
                (bx * bx + by * by) * (cy - ay) +
                (cx * cx + cy * cy) * (ay - by)) / d;

    float uy = ((ax * ax + ay * ay) * (cx - bx) +
                (bx * bx + by * by) * (ax - cx) +
                (cx * cx + cy * cy) * (bx - ax)) / d;

    circumcenter = glm::vec2(ux, uy);
    circumradius = glm::distance(circumcenter, p1);
}

bool Triangle2D::isPointInCircumcircle(const glm::vec2& point) const
{
    return glm::distance(circumcenter, point) <= circumradius + 1e-6f;
}

bool Triangle2D::hasVertex(const glm::vec2& vertex) const
{
    return glm::distance(p1, vertex) < 1e-6f ||
           glm::distance(p2, vertex) < 1e-6f ||
           glm::distance(p3, vertex) < 1e-6f;
}

bool Triangle2D::operator==(const Triangle2D& other) const
{
    return (hasVertex(other.p1) && hasVertex(other.p2) && hasVertex(other.p3));
}
