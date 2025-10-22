#pragma once

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/MouseEvent.hpp"
#include "Events/EventTypes/ColorPreviewEvent.hpp"
#include "Events/EventTypes/ColorPickedEvent.hpp"

#include "Manager/CameraManager.hpp"
#include "Manager/ViewportManager.hpp"

#include "Components/Renderable.hpp"
#include "Components/Transform.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Box.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

#include <limits>

class ViewportManager;

class EyedropperSystem {
    public:
        EyedropperSystem(
            ComponentRegistry& componentRegistry,
            EntityManager& entityManager,
            EventManager& eventManager
        );

        ~EyedropperSystem() = default;

        void setupManagers(CameraManager& cameraManager, ViewportManager& viewportManager);
        void setEyedropperMode(bool activateEyedropperMode);

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        EventManager& _eventManager;
        CameraManager* _cameraManager = nullptr;
        ViewportManager* _viewportManager = nullptr;

        bool _isEyedropperMode = false;

        void _handleMouseMove(const MouseEvent& e);
        void _handleMousePressed(const MouseEvent& e);
        EntityID _performRaycast(const glm::vec2& mouseGlobalPos);
};
