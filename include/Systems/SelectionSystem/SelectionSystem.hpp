#pragma once

#include "Core/ComponentRegistry/ComponentRegistry.hpp"
#include "Core/EntityManager/EntityManager.hpp"

#include "Events/EventManager/EventManager.hpp"
#include "Events/EventTypes/Events/MouseEvent.hpp"
#include "Events/EventTypes/Events/SelectionEvent.hpp"

#include "Manager/CameraManager/CameraManager.hpp"
#include "Manager/ViewportManager/ViewportManager.hpp"

#include "Components/Selectable.hpp"
#include "Components/Transform.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Box.hpp"

#include <glm/glm.hpp>

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
