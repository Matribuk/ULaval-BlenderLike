#pragma once

#include <vector>
#include "Core/ComponentRegistry.hpp"
#include "Core//EntityManager.hpp"

#include "Components/Camera.hpp"
#include "Components/Transform.hpp"

#include "Systems/CameraSystem.hpp"

class CameraManager {
    public:
        CameraManager(ComponentRegistry &componentRegistry, EntityManager &entityManager, CameraSystem &cameraSystem);
        ~CameraManager() = default;

        EntityID addCamera(glm::vec3 pos);
        void removeCamera(EntityID cameraId);

        void setActiveCamera(EntityID id);
        void switchCamera();
        void focusTarget(EntityID camEntity, EntityID targetEntity = INVALID_ENTITY);

        void zoom(float dir);
        void rotate(const glm::vec2 vect);
        void pan(const glm::vec3 vect);

        void update(int viewportWidth, int viewportHeight);

        EntityManager &getEntityManager() const;
        ComponentRegistry &getComponentRegistry() const;
        Camera *getActiveCamera() const;
        EntityID getActiveCameraId() const;
        EntityID getCameraAtIndex(int index) const;

        void toggleProjection(EntityID cameraId);
        void setProjection(EntityID cameraId, bool isOrtho);

    private:
        ComponentRegistry &_componentRegistry;
        EntityManager &_entityManager;
        CameraSystem &_cameraSystem;

        std::vector<EntityID> _cameraEntities;
        EntityID _activeCamera = INVALID_ENTITY;
        int _activeCameraIndex = 0;
};
