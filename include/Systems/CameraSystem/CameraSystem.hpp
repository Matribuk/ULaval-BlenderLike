#pragma once

// Components
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"

// Core
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"

// Systems
#include "Systems/TransformSystem/TransformSystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
class CameraSystem
{
    public:
        CameraSystem(ComponentRegistry& registry, TransformSystem& transformSystem);

        void update(std::vector<EntityID> cameraEntities, int viewportWidth, int viewportHeight);

        void pan(EntityID camEntity, const glm::vec3 vect);
        void rotate(EntityID camEntity, const glm::vec2 vect);
        void zoom(EntityID camEntity, float amount);

        ComponentRegistry& getRegistry() const;

    private:
        ComponentRegistry& _componentRegistry;
        TransformSystem& _transformSystem;

        glm::vec3 _getCameraForward(EntityID id) const;
        glm::vec3 _getCameraRight(EntityID id) const;

};