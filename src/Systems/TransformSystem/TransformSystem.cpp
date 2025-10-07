#include "Systems/TransformSystem/TransformSystem.hpp"

TransformSystem::TransformSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void TransformSystem::setPosition(EntityID entityId, const glm::vec3 position)
{
    float deltaTime = ofGetLastFrameTime();
    Transform *transform = this->_registry.getComponent<Transform>(entityId);

    glm::vec3 right = glm::vec3(1, 0, 0);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 forward = glm::vec3(0, 0, -1);

    transform->position += position.x * deltaTime * right + position.y * deltaTime * up + position.z * deltaTime * forward;
    transform->isDirty = true;
}

void TransformSystem::setRotation(EntityID entityId, const glm::vec2 rotation, const glm::vec3* pivot)
{
    float deltaTime = ofGetLastFrameTime();
    Transform* transform = this->_registry.getComponent<Transform>(entityId);
    glm::vec3 center = pivot ? *pivot : glm::vec3(0.0f);

    glm::vec3 offset = transform->position - center;
    float radius = glm::length(offset);
    if (radius == 0.0f) return;

    float yaw = atan2(offset.x, offset.z);
    float pitch = asin(offset.y / radius);

    yaw += rotation.x * deltaTime;
    pitch += rotation.y * deltaTime;

    float limit = glm::radians(89.0f);
    pitch = glm::clamp(pitch, -limit, limit);

    float cosPitch = cos(pitch);
    glm::vec3 newOffset;
    newOffset.x = radius * cosPitch * sin(yaw);
    newOffset.y = radius * sin(pitch);
    newOffset.z = radius * cosPitch * cos(yaw);

    transform->position = center + newOffset;
    transform->isDirty = true;
}

void TransformSystem::setScale(EntityID entityId, const glm::vec3& scale)
{
    Transform* transform = this->_registry.getComponent<Transform>(entityId);
    glm::vec3 clampedScale = glm::clamp(scale, glm::vec3(0.001f), glm::vec3(1000.0f));

    transform->scale = clampedScale;
    transform->isDirty = true;
}

void TransformSystem::update()
{
    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
        if (!transform) continue;

        if (transform->parent == INVALID_ENTITY) {
            updateTransformHierarchy(id, glm::mat4(1.0f));
        }
    }
}

void TransformSystem::updateTransformHierarchy(EntityID entity, const glm::mat4& parentGlobalMatrix)
{
    Transform* transform = this->_registry.getComponent<Transform>(entity);
    if (!transform) return;

    if (transform->isDirty) {
        transform->localMatrix = calculateMatrix(
            transform->position,
            transform->rotation,
            transform->scale
        );
        transform->isDirty = false;
    }

    transform->globalMatrix = parentGlobalMatrix * transform->localMatrix;
    transform->matrix = transform->globalMatrix;

    for (EntityID childId : transform->children) {
        updateTransformHierarchy(childId, transform->globalMatrix);
    }
}

inline glm::mat4 TransformSystem::calculateMatrix(
    const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scale)
{
    glm::mat4 matrice = glm::translate(glm::mat4(1.0f), pos);
    matrice = matrice * glm::yawPitchRoll(rot.y, rot.x, rot.z);
    matrice = glm::scale(matrice, scale);

    return matrice;
}

void TransformSystem::setParent(EntityID child, EntityID parent)
{
    Transform* childTransform = this->_registry.getComponent<Transform>(child);
    Transform* parentTransform = this->_registry.getComponent<Transform>(parent);

    if (!childTransform || !parentTransform || child == parent) return;

    if (childTransform->parent != INVALID_ENTITY) {
        removeParent(child);
    }

    childTransform->parent = parent;
    parentTransform->children.push_back(child);

    markDirty(child, true);
}

void TransformSystem::removeParent(EntityID child)
{
    Transform* childTransform = this->_registry.getComponent<Transform>(child);
    if (!childTransform || childTransform->parent == INVALID_ENTITY) return;

    Transform* parentTransform = this->_registry.getComponent<Transform>(childTransform->parent);
    if (parentTransform) {
        auto& children = parentTransform->children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    childTransform->parent = INVALID_ENTITY;
    markDirty(child, true);
}

EntityID TransformSystem::getParent(EntityID entity) const
{
    Transform* transform = this->_registry.getComponent<Transform>(entity);
    return transform ? transform->parent : INVALID_ENTITY;
}

const std::vector<EntityID>& TransformSystem::getChildren(EntityID entity) const
{
    static const std::vector<EntityID> empty;
    Transform* transform = this->_registry.getComponent<Transform>(entity);
    return transform ? transform->children : empty;
}

void TransformSystem::markDirty(EntityID entity, bool recursive)
{
    Transform* transform = this->_registry.getComponent<Transform>(entity);
    if (!transform) return;

    transform->isDirty = true;

    if (recursive) {
        for (EntityID childId : transform->children) {
            markDirty(childId, true);
        }
    }
}

glm::mat4 TransformSystem::getGlobalMatrix(EntityID entity) const
{
    Transform* transform = this->_registry.getComponent<Transform>(entity);
    return transform ? transform->globalMatrix : glm::mat4(1.0f);
}