#pragma once

#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraSystem {
public:
    CameraSystem(ComponentRegistry& registry, EntityManager& entityMgr);
    ~CameraSystem() = default;

    void update();

private:
    ComponentRegistry& _registry;
    EntityManager& _entityManager;
};
