#pragma once

#include <vector>
#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include "Systems/CameraSystem/CameraSystem.hpp"

class CameraManager {
    public:
        CameraManager(ComponentRegistry &componentRegistry, EntityManager &entityManager, CameraSystem &cameraSystem);
        ~CameraManager() = default;

        void addCamera(glm::vec3 pos);
        void setActiveCamera(EntityID id);
        void switchCamera();
        void focusTarget(EntityID camEntity);

        void updateZoom(float dir);
        void updatePan(float x, float y, float depth);
        void updateOrbit(float x, float y);

        void update(int viewportWidth, int viewportHeight);

        EntityManager &getEntityManager() const;
        ComponentRegistry &getComponentRegistry() const;
        Camera *getActiveCamera() const;
        EntityID getActiveCameraId() const;
        EntityID getCameraAtIndex(int index) const;

    private:
        ComponentRegistry &_componentRegistry;
        EntityManager &_entityManager;
        CameraSystem &_cameraSystem;

        std::vector<EntityID> _cameraEntities;
        EntityID _activeCamera = INVALID_ENTITY;
        int _activeCameraIndex = 0;
};
