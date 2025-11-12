#include "Systems/RaycastSystem.hpp"

RaycastHit::RaycastHit(EntityID id, const glm::vec3& p, const glm::vec3& n, float d)
    : entityId(id), point(p), normal(n), distance(d) {}

bool RaycastHit::isValid() const
{
    return entityId != INVALID_ENTITY && distance < std::numeric_limits<float>::max();
}

std::optional<RaycastHit> RaycastSystem::intersectRaySphere(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& sphereCenter,
    float sphereRadius,
    EntityID entityId
) {
    glm::vec3 oc = rayOrigin - sphereCenter;

    float a = glm::dot(rayDir, rayDir);
    float b = 2.0f * glm::dot(oc, rayDir);
    float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f)
        return std::nullopt;

    float sqrtDiscriminant = std::sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
    float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

    float t = (t1 >= EPSILON) ? t1 : (t2 >= EPSILON) ? t2 : -1.0f;
    if (t < 0.0f)
      return std::nullopt;

    glm::vec3 hitPoint = rayOrigin + t * rayDir;
    glm::vec3 normal = glm::normalize(hitPoint - sphereCenter);

    return RaycastHit(entityId, hitPoint, normal, t);
}

std::optional<RaycastHit> RaycastSystem::intersectRayBox(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& boxCenter,
    const glm::vec3& boxHalfExtents,
    const glm::mat3& boxRotation,
    EntityID entityId
) {
    glm::mat3 invRotation = glm::transpose(boxRotation);
    glm::vec3 localRayOrigin = invRotation * (rayOrigin - boxCenter);
    glm::vec3 localRayDir = invRotation * rayDir;

    glm::vec3 aabbMin = -boxHalfExtents;
    glm::vec3 aabbMax = boxHalfExtents;

    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    int normalAxis = -1;
    bool normalPositive = false;

    for (int i = 0; i < 3; ++i) {
        if (std::abs(localRayDir[i]) < EPSILON) {
            if (localRayOrigin[i] < aabbMin[i] || localRayOrigin[i] > aabbMax[i])
                return std::nullopt;
        } else {
            float invD = 1.0f / localRayDir[i];
            float t1 = (aabbMin[i] - localRayOrigin[i]) * invD;
            float t2 = (aabbMax[i] - localRayOrigin[i]) * invD;

            bool swapped = false;
            if (t1 > t2) {
                std::swap(t1, t2);
                swapped = true;
            }

            if (t1 > tmin) {
                tmin = t1;
                normalAxis = i;
                normalPositive = !swapped;
            }

            tmax = std::min(tmax, t2);

            if (tmin > tmax)
                return std::nullopt;
        }
    }

    if (tmax < EPSILON)
        return std::nullopt;

    float t = (tmin >= EPSILON) ? tmin : tmax;

    glm::vec3 localHitPoint = localRayOrigin + t * localRayDir;
    glm::vec3 hitPoint = boxRotation * localHitPoint + boxCenter;

    glm::vec3 localNormal(0.0f);
    if (normalAxis >= 0)
        localNormal[normalAxis] = normalPositive ? -1.0f : 1.0f;
    else
        localNormal = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 normal = glm::normalize(boxRotation * localNormal);

    return RaycastHit(entityId, hitPoint, normal, t);
}

std::optional<RaycastHit> RaycastSystem::intersectRayPlane(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& planePoint,
    const glm::vec3& planeNormal,
    EntityID entityId
) {
    float denom = glm::dot(planeNormal, rayDir);

    if (std::abs(denom) < EPSILON)
        return std::nullopt;

    glm::vec3 p0l0 = planePoint - rayOrigin;
    float t = glm::dot(p0l0, planeNormal) / denom;

    if (t < EPSILON)
        return std::nullopt;

    glm::vec3 hitPoint = rayOrigin + t * rayDir;

    glm::vec3 normal = planeNormal;
    if (glm::dot(rayDir, planeNormal) > 0.0f)
        normal = -normal;

    return RaycastHit(entityId, hitPoint, normal, t);
}

std::optional<RaycastHit> RaycastSystem::intersectRayAABB(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& aabbMin,
    const glm::vec3& aabbMax,
    EntityID entityId
) {
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    int normalAxis = -1;
    bool normalPositive = false;

    for (int i = 0; i < 3; ++i) {
        if (std::abs(rayDir[i]) < EPSILON) {
            if (rayOrigin[i] < aabbMin[i] || rayOrigin[i] > aabbMax[i])
                return std::nullopt;
        } else {
            float invD = 1.0f / rayDir[i];
            float t1 = (aabbMin[i] - rayOrigin[i]) * invD;
            float t2 = (aabbMax[i] - rayOrigin[i]) * invD;

            bool swapped = false;
            if (t1 > t2) {
                std::swap(t1, t2);
                swapped = true;
            }

            if (t1 > tmin) {
                tmin = t1;
                normalAxis = i;
                normalPositive = !swapped;
            }

            tmax = std::min(tmax, t2);

            if (tmin > tmax)
                return std::nullopt;
        }
    }

    if (tmax < EPSILON)
        return std::nullopt;

    float t = (tmin >= EPSILON) ? tmin : tmax;

    glm::vec3 hitPoint = rayOrigin + t * rayDir;

    glm::vec3 normal(0.0f);
    if (normalAxis >= 0)
        normal[normalAxis] = normalPositive ? -1.0f : 1.0f;
    else
        normal = glm::vec3(0.0f, 1.0f, 0.0f);

    return RaycastHit(entityId, hitPoint, normal, t);
}
