#pragma once

#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "ofMain.h"

class CameraSystem {
    public:
        CameraSystem(ComponentRegistry& registry, EntityManager& entityMgr);

        Camera* getActiveCamera() const;
        EntityID getActiveCameraId() const;
        EntityID getCameraAtIndex(int index) const;
        EntityManager& getEntityManager();
        ComponentRegistry& getRegistry();

        void addCamera(glm::vec3 pos);
        void setActiveCamera(EntityID id);
        void switchCamera();

        void panKeyboard(float horizontal, float vertical);
        void orbitKeyboard(float horizontal, float vertical);
        void zoom(float amount);
        void focusTarget();

        void applyZoom(float dir) { _zoomInput += dir; }
        void applyPan(float x, float y) { _panInput += glm::vec2(x, y); }
        void applyOrbit(float x, float y) { _orbitInput += glm::vec2(x, y); }

        void update(int viewportWidth, int viewportHeight);

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        std::vector<EntityID> _cameraEntities;
        EntityID _activeCamera = INVALID_ENTITY;
        int _activeCameraIndex = 0;

        float _zoomInput = 0.0f;
        glm::vec2 _panInput {0.0f, 0.0f};
        glm::vec2 _orbitInput {0.0f, 0.0f};

        glm::vec3 _getCameraForward(EntityID id) const;
        glm::vec3 _getCameraRight(EntityID id) const;
};
