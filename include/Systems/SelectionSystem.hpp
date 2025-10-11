#pragma once

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/MouseEvent.hpp"
#include "Events/EventTypes/SelectionEvent.hpp"

#include "Manager/CameraManager.hpp"
#include "Manager/ViewportManager.hpp"

#include "Components/Selectable.hpp"
#include "Components/Transform.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Box.hpp"

#include "UI/Viewport.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

#include <limits>
#include <iostream>

class ViewportManager;
class Viewport;

class SelectionSystem {
    public:
        SelectionSystem(
            ComponentRegistry& componentRegistry,
            EntityManager& entityManager,
            EventManager& eventManager,
            CameraManager& cameraManager,
            ViewportManager& viewportManager
        );

        ~SelectionSystem() = default;

        void setup();

        EntityID getSelectedEntity();
        void setSelectedEntity(EntityID selectedEntity);

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        EventManager& _eventManager;
        CameraManager& _cameraManager;
        ViewportManager& _viewportManager;

        EntityID _selectedEntity = 0;

        void _handleMouseEvent(const MouseEvent& e);
        EntityID _performRaycastInActiveViewport(const glm::vec2& mouseGlobalPos);
        void _updateSelection(EntityID selected);
        bool _intersectsRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                            const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& outT) const;

        glm::mat4 _getOrComputeTransformMatrix(Transform* t) const;
        void _transformAABB(const glm::vec3& localMin, const glm::vec3& localMax,
                           const glm::mat4& transform,
                           glm::vec3& outMin, glm::vec3& outMax) const;
};
