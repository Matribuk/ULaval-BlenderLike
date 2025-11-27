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
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    glm::vec3 forward = getForward(entityId);

    transform->position += movement.x * deltaTime * right
                         + movement.y * deltaTime * worldUp
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

    if (transform->parent != INVALID_ENTITY) {
        glm::vec3 parentPos, parentRot, parentScale;
        decomposeMatrix(parentGlobalMatrix, parentPos, parentRot, parentScale);

        glm::mat4 parentTR = glm::translate(glm::mat4(1.0f), parentPos);
        parentTR = parentTR * glm::eulerAngleXYZ(parentRot.x, parentRot.y, parentRot.z);

        glm::mat4 childLocal = glm::translate(glm::mat4(1.0f), transform->position);
        childLocal = childLocal * glm::eulerAngleXYZ(transform->rotation.x, transform->rotation.y, transform->rotation.z);
        childLocal = childLocal * glm::scale(glm::mat4(1.0f), transform->scale);

        transform->globalMatrix = parentTR * childLocal;
    } else {
        transform->globalMatrix = transform->localMatrix;
    }

    transform->matrix = transform->globalMatrix;

    for (EntityID childId : transform->children) {
        updateTransformHierarchy(childId, transform->globalMatrix);
    }
}

inline glm::mat4 TransformSystem::calculateMatrix(
    const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scale)
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 rotationMatrix = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

void TransformSystem::setParent(EntityID child, EntityID parent)
{
    Transform* childTransform = this->_registry.getComponent<Transform>(child);
    Transform* parentTransform = this->_registry.getComponent<Transform>(parent);

    if (!childTransform || !parentTransform || child == parent) return;

    glm::vec3 childGlobalPos, childGlobalRot, childGlobalScale;

    if (childTransform->parent != INVALID_ENTITY) {
        removeParent(child);
        decomposeMatrix(childTransform->globalMatrix, childGlobalPos, childGlobalRot, childGlobalScale);
    } else {
        childGlobalPos = childTransform->position;
        childGlobalRot = childTransform->rotation;
        childGlobalScale = childTransform->scale;
    }

    childTransform->parent = parent;
    parentTransform->children.push_back(child);

    glm::vec3 parentPos, parentRot, parentScale;
    decomposeMatrix(parentTransform->globalMatrix, parentPos, parentRot, parentScale);

    glm::mat4 parentTR = glm::translate(glm::mat4(1.0f), parentPos);
    parentTR = parentTR * glm::eulerAngleXYZ(parentRot.x, parentRot.y, parentRot.z);

    glm::vec3 localPos = glm::vec3(glm::inverse(parentTR) * glm::vec4(childGlobalPos, 1.0f));

    glm::vec3 localRot = childGlobalRot - parentRot;

    childTransform->position = localPos;
    childTransform->rotation = localRot;

    markDirty(child, true);
}

void TransformSystem::removeParent(EntityID child)
{
    Transform* childTransform = this->_registry.getComponent<Transform>(child);
    if (!childTransform || childTransform->parent == INVALID_ENTITY) return;

    glm::mat4 childGlobalMatrix = childTransform->globalMatrix;
    glm::vec3 globalPos, globalRot, globalScale;
    decomposeMatrix(childGlobalMatrix, globalPos, globalRot, globalScale);

    Transform* parentTransform = this->_registry.getComponent<Transform>(childTransform->parent);
    if (parentTransform) {
        auto& children = parentTransform->children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    childTransform->parent = INVALID_ENTITY;

    childTransform->position = globalPos;
    childTransform->rotation = globalRot;

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

void TransformSystem::decomposeMatrix(const glm::mat4& matrix, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale)
{
    outPosition = glm::vec3(matrix[3]);

    glm::vec3 col0 = glm::vec3(matrix[0]);
    glm::vec3 col1 = glm::vec3(matrix[1]);
    glm::vec3 col2 = glm::vec3(matrix[2]);

    float scaleX = glm::length(col0);
    float scaleY = glm::length(col1);
    float scaleZ = glm::length(col2);

    glm::mat3 upperLeft(matrix);
    if (glm::determinant(upperLeft) < 0.0f) {
        scaleX = -scaleX;
    }

    outScale.x = scaleX;
    outScale.y = scaleY;
    outScale.z = scaleZ;

    float absScaleX = glm::abs(scaleX);
    float absScaleY = glm::abs(scaleY);
    float absScaleZ = glm::abs(scaleZ);

    if (absScaleX < 0.0001f) absScaleX = 1.0f;
    if (absScaleY < 0.0001f) absScaleY = 1.0f;
    if (absScaleZ < 0.0001f) absScaleZ = 1.0f;

    glm::mat3 rotationMatrix;
    rotationMatrix[0] = col0 / (scaleX >= 0.0f ? absScaleX : -absScaleX);
    rotationMatrix[1] = col1 / absScaleY;
    rotationMatrix[2] = col2 / absScaleZ;

    float sy = rotationMatrix[0][2];

    if (glm::abs(sy) >= 0.999f) {
        outRotation.y = glm::asin(glm::clamp(sy, -1.0f, 1.0f));
        outRotation.x = 0.0f;
        outRotation.z = glm::atan2(-rotationMatrix[1][0], rotationMatrix[1][1]);
    } else {
        outRotation.y = glm::asin(glm::clamp(sy, -1.0f, 1.0f));
        outRotation.x = glm::atan2(-rotationMatrix[1][2], rotationMatrix[2][2]);
        outRotation.z = glm::atan2(-rotationMatrix[0][1], rotationMatrix[0][0]);
    }
}
