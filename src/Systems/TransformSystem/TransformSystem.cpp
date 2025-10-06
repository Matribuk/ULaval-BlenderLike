#include "Systems/TransformSystem/TransformSystem.hpp"

TransformSystem::TransformSystem(ComponentRegistry& registry, EntityManager& entityMgr)
    : _registry(registry), _entityManager(entityMgr) {}

void TransformSystem::update()
{
    for (EntityID id : this->_entityManager.getAllEntities()) {
        Transform* transform = this->_registry.getComponent<Transform>(id);
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
    glm::mat4 matrice = glm::translate(glm::mat4(1.0f), pos);
    matrice = matrice * glm::yawPitchRoll(rot.y, rot.x, rot.z);
    matrice = glm::scale(matrice, scale);

    return matrice;
}