#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

struct Triangle2D {
    glm::vec2 p1, p2, p3;
    glm::vec2 circumcenter;
    float circumradius;

    Triangle2D(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
        : p1(a), p2(b), p3(c)
    {
        computeCircumcircle();
    }

    /**
     * @brief Calcule le cercle circonscrit du triangle
     */
    void computeCircumcircle()
    {
        float ax = p1.x, ay = p1.y;
        float bx = p2.x, by = p2.y;
        float cx = p3.x, cy = p3.y;

        float d = 2.0f * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));

        if (std::abs(d) < 1e-6f) {
            // Triangle dégénéré (points colinéaires)
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

    /**
     * @brief Vérifie si un point est à l'intérieur du cercle circonscrit
     */
    bool isPointInCircumcircle(const glm::vec2& point) const
    {
        return glm::distance(circumcenter, point) <= circumradius + 1e-6f;
    }

    /**
     * @brief Vérifie si le triangle contient un sommet donné
     */
    bool hasVertex(const glm::vec2& vertex) const
    {
        return glm::distance(p1, vertex) < 1e-6f ||
               glm::distance(p2, vertex) < 1e-6f ||
               glm::distance(p3, vertex) < 1e-6f;
    }

    /**
     * @brief Vérifie l'égalité entre deux triangles
     */
    bool operator==(const Triangle2D& other) const
    {
        return (hasVertex(other.p1) && hasVertex(other.p2) && hasVertex(other.p3));
    }
};

struct Edge2D {
    glm::vec2 p1, p2;

    Edge2D(const glm::vec2& a, const glm::vec2& b) : p1(a), p2(b) {}

    bool operator==(const Edge2D& other) const
    {
        return (glm::distance(p1, other.p1) < 1e-6f && glm::distance(p2, other.p2) < 1e-6f) ||
               (glm::distance(p1, other.p2) < 1e-6f && glm::distance(p2, other.p1) < 1e-6f);
    }
};

/**
 * @brief Structure représentant une cellule de Voronoï
 */
struct VoronoiCell {
    glm::vec2 site;
    std::vector<glm::vec2> vertices;

    VoronoiCell() = default;
    explicit VoronoiCell(const glm::vec2& s) : site(s) {}
};

/**
 * @brief Classe implémentant l'algorithme de triangulation de Delaunay
 *
 * Utilise l'algorithme Bowyer-Watson pour calculer une triangulation de Delaunay
 * à partir d'un ensemble de points 2D.
 */
class Delaunay {
public:
    Delaunay() = default;
    ~Delaunay() = default;

    /**
     * @brief Calcule la triangulation de Delaunay d'un ensemble de points
     *
     * @param points Liste des points à trianguler
     * @return std::vector<Triangle2D> Liste des triangles de Delaunay
     */
    std::vector<Triangle2D> triangulate(const std::vector<glm::vec2>& points)
    {
        if (points.size() < 3) {
            return {};
        }

        std::vector<glm::vec2> workingPoints = points;
        Triangle2D superTriangle = createSuperTriangle(workingPoints);
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

    std::vector<VoronoiCell> computeVoronoi(const std::vector<glm::vec2>& points)
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

            cell.vertices = sortVerticesCounterClockwise(circumcenters, point);
            cells.push_back(cell);
        }

        return cells;
    }

private:
    /**
     * @brief Crée un super-triangle qui englobe tous les points
     */
    Triangle2D createSuperTriangle(const std::vector<glm::vec2>& points)
    {
        // Trouver la bounding box
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

    /**
     * @brief Trie les sommets dans le sens trigonométrique autour d'un centre
     */
    std::vector<glm::vec2> sortVerticesCounterClockwise(const std::vector<glm::vec2>& vertices, const glm::vec2& center)
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
};
