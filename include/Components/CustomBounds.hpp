#pragma once

#include <glm/glm.hpp>

struct CustomBounds {
    glm::vec3 min;
    glm::vec3 max;

    CustomBounds() : min(0.0f), max(0.0f) {}

    CustomBounds(const glm::vec3& minBounds, const glm::vec3& maxBounds)
        : min(minBounds), max(maxBounds) {}

    static CustomBounds fromPoints2D(const std::vector<glm::vec2>& points, float yPadding = 1.0f) {
        if (points.empty()) {
            return CustomBounds();
        }

        glm::vec2 min2D = points[0];
        glm::vec2 max2D = points[0];

        for (const auto& p : points) {
            min2D.x = std::min(min2D.x, p.x);
            min2D.y = std::min(min2D.y, p.y);
            max2D.x = std::max(max2D.x, p.x);
            max2D.y = std::max(max2D.y, p.y);
        }

        return CustomBounds(
            glm::vec3(min2D.x, -yPadding, min2D.y),
            glm::vec3(max2D.x, yPadding, max2D.y)
        );
    }
};
