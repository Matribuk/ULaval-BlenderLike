#include "Algorithms/Delaunay.hpp"


std::vector<Triangle2D> Delaunay::triangulate(const std::vector<glm::vec2>& points)
{
    if (points.size() < 3) {
        return {};
    }

    std::vector<glm::vec2> workingPoints = points;
    Triangle2D superTriangle = this->_createSuperTriangle(workingPoints);
    std::vector<Triangle2D> triangulation = { superTriangle };

    for (const auto& point : workingPoints) {
        std::vector<Triangle2D> badTriangles;

        for (const auto& triangle : triangulation) {
            if (triangle.isPointInCircumcircle(point)) {
                badTriangles.push_back(triangle);
            }
        }

        std::vector<Edge2D> polygon;
        for (const auto& triangle : badTriangles) {
            Edge2D edges[3] = {
                Edge2D(triangle.p1, triangle.p2),
                Edge2D(triangle.p2, triangle.p3),
                Edge2D(triangle.p3, triangle.p1)
            };

            for (const auto& edge : edges) {
                bool isShared = false;
                for (const auto& otherTriangle : badTriangles) {
                    if (triangle == otherTriangle) continue;

                    if (otherTriangle.hasVertex(edge.p1) && otherTriangle.hasVertex(edge.p2)) {
                        isShared = true;
                        break;
                    }
                }

                if (!isShared) {
                    polygon.push_back(edge);
                }
            }
        }

        triangulation.erase(
            std::remove_if(triangulation.begin(), triangulation.end(),
                [&badTriangles](const Triangle2D& t) {
                    return std::find(badTriangles.begin(), badTriangles.end(), t) != badTriangles.end();
                }),
            triangulation.end()
        );

        for (const auto& edge : polygon) {
            Triangle2D newTriangle(edge.p1, edge.p2, point);
            triangulation.push_back(newTriangle);
        }
    }

    triangulation.erase(
        std::remove_if(triangulation.begin(), triangulation.end(),
            [&superTriangle](const Triangle2D& t) {
                return t.hasVertex(superTriangle.p1) ||
                       t.hasVertex(superTriangle.p2) ||
                       t.hasVertex(superTriangle.p3);
            }),
        triangulation.end()
    );

    return triangulation;
}

std::vector<VoronoiCell> Delaunay::computeVoronoi(const std::vector<glm::vec2>& points)
{
    if (points.size() < 3) {
        return {};
    }

    std::vector<Triangle2D> triangles = triangulate(points);
    std::vector<VoronoiCell> cells;
    cells.reserve(points.size());

    for (const auto& point : points) {
        VoronoiCell cell(point);

        std::vector<Triangle2D> adjacentTriangles;
        for (const auto& tri : triangles) {
            if (tri.hasVertex(point)) {
                adjacentTriangles.push_back(tri);
            }
        }

        std::vector<glm::vec2> circumcenters;
        circumcenters.reserve(adjacentTriangles.size());
        for (const auto& tri : adjacentTriangles) {
            circumcenters.push_back(tri.circumcenter);
        }

        cell.vertices = this->_sortVerticesCounterClockwise(circumcenters, point);
        cells.push_back(cell);
    }

    return cells;
}

Triangle2D Delaunay::_createSuperTriangle(const std::vector<glm::vec2>& points)
{
    float minX = points[0].x, maxX = points[0].x;
    float minY = points[0].y, maxY = points[0].y;

    for (const auto& p : points) {
        minX = std::min(minX, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }

    float dx = maxX - minX;
    float dy = maxY - minY;
    float deltaMax = std::max(dx, dy);
    float midX = (minX + maxX) * 0.5f;
    float midY = (minY + maxY) * 0.5f;

    glm::vec2 p1(midX - 20.0f * deltaMax, midY - deltaMax);
    glm::vec2 p2(midX, midY + 20.0f * deltaMax);
    glm::vec2 p3(midX + 20.0f * deltaMax, midY - deltaMax);

    return Triangle2D(p1, p2, p3);
}

std::vector<glm::vec2> Delaunay::_sortVerticesCounterClockwise(
    const std::vector<glm::vec2>& vertices,
    const glm::vec2& center)
{
    if (vertices.empty()) return {};

    std::vector<std::pair<float, glm::vec2>> angledVertices;
    angledVertices.reserve(vertices.size());

    for (const auto& v : vertices) {
        glm::vec2 diff = v - center;
        float angle = std::atan2(diff.y, diff.x);
        angledVertices.push_back({angle, v});
    }

    std::sort(angledVertices.begin(), angledVertices.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    std::vector<glm::vec2> sorted;
    sorted.reserve(vertices.size());
    for (const auto& av : angledVertices) {
        sorted.push_back(av.second);
    }

    return sorted;
}
