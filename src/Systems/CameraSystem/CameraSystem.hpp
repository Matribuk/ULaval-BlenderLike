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

        void update();

        void panKeyboard(float horizontal, float vertical);
        void orbitKeyboard(float horizontal, float vertical);
        void zoom(float amount);
        void focusTarget();

        void addCamera(EntityID cameraID);
        void setActiveCamera(EntityID id);
        void switchCamera();

        Camera* getActiveCamera() const;
        EntityID getActiveCameraId() const;

    private:
        ComponentRegistry& _registry;
        EntityManager& _entityManager;

        EntityID _activeCamera{INVALID_ENTITY};
        std::vector<EntityID> _cameraEntities;
        int _activeCameraIndex{0};

        glm::vec3 _getCameraForward(EntityID id) const;
        glm::vec3 _getCameraRight(EntityID id) const;
};
