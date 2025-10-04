#pragma once

#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

class CameraSystem {
    public:
        CameraSystem(ComponentRegistry& registry, EntityManager& entityMgr);

        Camera* getActiveCamera() const;
        EntityID getActiveCameraId() const;
        EntityID getCameraAtIndex(int index) const;

        void addCamera(glm::vec3 pos);
        void setActiveCamera(EntityID id);
        void switchCamera();

        void panKeyboard(float horizontal, float vertical);
        void orbitKeyboard(float horizontal, float vertical);
        void zoom(float amount);
        void focusTarget();

        void update();

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        std::vector<EntityID> _cameraEntities;
        EntityID _activeCamera = INVALID_ENTITY;
        int _activeCameraIndex = 0;

        glm::vec3 _getCameraForward(EntityID id) const;
        glm::vec3 _getCameraRight(EntityID id) const;
};
