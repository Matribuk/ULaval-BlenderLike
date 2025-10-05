#pragma once

#include "Transform.hpp"
#include "ComponentRegistry.hpp"
#include "EntityManager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class TransformSystem {
    public:
        TransformSystem(ComponentRegistry& registry, EntityManager& entityMgr);
        ~TransformSystem() = default;

        void update();

        glm::mat4 calculateMatrix(
            const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);
    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;

};
