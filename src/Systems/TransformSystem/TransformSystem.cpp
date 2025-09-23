#include "TransformSystem.hpp"

TransformSystem::TransformSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void TransformSystem::update()
{
    for (EntityID id : _entityManager.getAllEntities()) {
        Transform* transform = _registry.getComponent<Transform>(id);
        if (transform)
            transform->matrix = calculateMatrix(
                transform->position,
                transform->rotation,
                transform->scale
            );
    }
}

inline glm::mat4 TransformSystem::calculateMatrix(
    const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scale)
{
    glm::mat4 rotationMat = glm::yawPitchRoll(
        glm::radians(rot.y),
        glm::radians(rot.x),
        glm::radians(rot.z)
    );

    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), pos);

    glm::mat4 matrice = translationMat * rotationMat * scaleMat;
    return matrice;
}
