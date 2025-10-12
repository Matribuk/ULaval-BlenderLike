#include "Systems/TransformSystem.hpp"

TransformSystem::TransformSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void TransformSystem::setPosition(EntityID entityId, glm::vec3 pos)
{
    Transform* t = this->_registry.getComponent<Transform>(entityId);
    if (!t) return;
    t->position = pos;
    markDirty(entityId, true);
}

void TransformSystem::setRotation(EntityID entityId, glm::vec3 rot)
{
    Transform* t = this->_registry.getComponent<Transform>(entityId);
    if (!t) return;
    t->rotation = rot;
    markDirty(entityId, true);
}

void TransformSystem::setScale(EntityID entityId, glm::vec3 scale)
{
    Transform* t = this->_registry.getComponent<Transform>(entityId);
    if (!t) return;
    t->scale = scale;
    markDirty(entityId, true);
}

void TransformSystem::setCameraPosition(EntityID entityId, const glm::vec3 movement)
{
    float deltaTime = ofGetLastFrameTime();
    Transform* transform = this->_registry.getComponent<Transform>(entityId);
    if (!transform) return;

    glm::vec3 right   = getRight(entityId);
    glm::vec3 up      = getUp(entityId);
    glm::vec3 forward = getForward(entityId);

    transform->position += movement.x * deltaTime * right
                         + movement.y * deltaTime * up
                         + movement.z * deltaTime * forward;

    transform->isDirty = true;
}

void TransformSystem::setCameraRotation(EntityID entityId, const glm::vec2 rotationDelta)
{
    float deltaTime = ofGetLastFrameTime();
    Transform* transform = this->_registry.getComponent<Transform>(entityId);
    if (!transform) return;

    transform->rotation.x += rotationDelta.x * deltaTime;
    transform->rotation.y += rotationDelta.y * deltaTime;
    transform->rotation.y = glm::clamp(transform->rotation.y, glm::radians(-89.0f), glm::radians(89.0f));

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

glm::vec3 TransformSystem::getForward(EntityID entityId) const
{
    Transform* t = this->_registry.getComponent<Transform>(entityId);
    if (!t) return glm::vec3(0,0,-1);

    float yaw   = t->rotation.x;
    float pitch = t->rotation.y;

    glm::vec3 forward;
    forward.x = cos(pitch) * sin(yaw);
    forward.y = sin(pitch);
    forward.z = cos(pitch) * cos(yaw);
    return glm::normalize(forward);
}

glm::vec3 TransformSystem::getRight(EntityID entityId) const
{
    glm::vec3 forward = getForward(entityId);
    return glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
}

glm::vec3 TransformSystem::getUp(EntityID entityId) const
{
    glm::vec3 forward = getForward(entityId);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
    return glm::cross(right, forward);
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