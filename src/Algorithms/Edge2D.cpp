#include "Algorithms/Edge2D.hpp"

Edge2D::Edge2D(const glm::vec2& a, const glm::vec2& b)
    : p1(a), p2(b)
{
}

bool Edge2D::operator==(const Edge2D& other) const
{
    return (glm::distance(p1, other.p1) < 1e-6f && glm::distance(p2, other.p2) < 1e-6f) ||
           (glm::distance(p1, other.p2) < 1e-6f && glm::distance(p2, other.p1) < 1e-6f);
}
