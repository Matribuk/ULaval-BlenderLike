#pragma once

#include "ofMain.h"

// Components
#include "Components/Transform.hpp"

// Core
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <algorithm>

class TransformSystem {
    public:
        TransformSystem(ComponentRegistry& registry, EntityManager& entityMgr);
        ~TransformSystem() = default;

        void setPosition(EntityID entityId, float horizontal, float vertical, float depth);
        void setRotation(EntityID entityId, float horizontal, float vertical, const glm::vec3* pivot);
        void setScale(EntityID entityId, const glm::vec3& scale);

        void update();

        glm::mat4 calculateMatrix(
            const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);

        void setParent(EntityID child, EntityID parent);
        void removeParent(EntityID child);
        EntityID getParent(EntityID entity) const;
        const std::vector<EntityID>& getChildren(EntityID entity) const;
        void markDirty(EntityID entity, bool recursive = true);
        glm::mat4 getGlobalMatrix(EntityID entity) const;

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;

        void updateTransformHierarchy(EntityID entity, const glm::mat4& parentGlobalMatrix);
};
