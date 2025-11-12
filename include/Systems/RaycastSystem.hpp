#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <optional>
#include <cmath>

#include "Core/Entity.hpp"

struct RaycastHit {
    EntityID entityId = INVALID_ENTITY;
    glm::vec3 point = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
    float distance = std::numeric_limits<float>::max();

    RaycastHit() = default;
    RaycastHit(EntityID id, const glm::vec3& p, const glm::vec3& n, float d);

    bool isValid() const;
};

class RaycastSystem {
    public:
        static std::optional<RaycastHit> intersectRaySphere(
            const glm::vec3& rayOrigin,
            const glm::vec3& rayDir,
            const glm::vec3& sphereCenter,
            float sphereRadius,
            EntityID entityId = INVALID_ENTITY
        );

        static std::optional<RaycastHit> intersectRayBox(
            const glm::vec3& rayOrigin,
            const glm::vec3& rayDir,
            const glm::vec3& boxCenter,
            const glm::vec3& boxHalfExtents,
            const glm::mat3& boxRotation,
            EntityID entityId = INVALID_ENTITY
        );

        static std::optional<RaycastHit> intersectRayPlane(
            const glm::vec3& rayOrigin,
            const glm::vec3& rayDir,
            const glm::vec3& planePoint,
            const glm::vec3& planeNormal,
            EntityID entityId = INVALID_ENTITY
        );

        static std::optional<RaycastHit> intersectRayAABB(
            const glm::vec3& rayOrigin,
            const glm::vec3& rayDir,
            const glm::vec3& aabbMin,
            const glm::vec3& aabbMax,
            EntityID entityId = INVALID_ENTITY
        );

    private:
        static constexpr float EPSILON = 1e-6f;
};
