#pragma once

#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "ofMain.h"

class CameraSystem
{
    public:
        CameraSystem(ComponentRegistry& registry);

        void handleCameraMovement(EntityID camEntity);
        void update(std::vector<EntityID> cameraEntities, int viewportWidth, int viewportHeight);

        void updateZoomInput(float zoom);
        void updatePanInput(float x, float y);
        void updateOrbitInput(float x, float y);

        ComponentRegistry& getRegistry() const;

    private:
        ComponentRegistry& _componentRegistry;

        float _zoomInput = 0.0f;
        glm::vec2 _panInput{0.0f, 0.0f};
        glm::vec2 _orbitInput{0.0f, 0.0f};

        glm::vec3 _getCameraForward(EntityID id) const;
        glm::vec3 _getCameraRight(EntityID id) const;

        void _panKeyboard(EntityID camEntity, float horizontal, float vertical);
        void _orbitKeyboard(EntityID camEntity, float horizontal, float vertical);
        void _zoom(EntityID camEntity, float amount);
};
