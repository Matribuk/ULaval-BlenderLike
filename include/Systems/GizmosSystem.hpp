#pragma once

#include "glm/glm.hpp"
#include "ofMain.h"

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "Components/Transform.hpp"

#include "Systems/SelectionSystem.hpp"

#include "Manager/CameraManager.hpp"

class SelectionSystem;
class CameraManager;

class Gizmo {
    public:
        glm::vec3 position;
        float size = 3.0f;
        bool isActive = false;

        void render(const ofCamera& cam);
        bool isHovered(const ofCamera& cam, const glm::vec2& mousePos);
};

class GizmosSystem {
    public:
        GizmosSystem(
            ComponentRegistry& componentRegistry,
            EntityManager& entityManager,
            SelectionSystem& selectionSystem,
            CameraManager& cameraManager
        );

        void update();
        void render();

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        SelectionSystem& _selectionSystem;
        CameraManager& _cameraManager;

        Gizmo _gizmo;
};
