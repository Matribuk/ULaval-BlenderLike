#pragma once

#include "Core/ComponentRegistry.hpp"
#include "Core/EntityManager.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/MouseEvent.hpp"
#include "Events/EventTypes/SelectionEvent.hpp"

#include "Manager/CameraManager.hpp"
#include "Manager/ViewportManager.hpp"
#include "Manager/InputManager.hpp"

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
#include <set>

class ViewportManager;

class SelectionSystem {
    public:
        SelectionSystem(
            ComponentRegistry& componentRegistry,
            EntityManager& entityManager,
            EventManager& eventManager
        );

        ~SelectionSystem() = default;

        void setupManagers(CameraManager& cameraManager, ViewportManager& viewportManager);

        EntityID getSelectedEntity();
        void setSelectedEntity(EntityID selectedEntity);
        void setSelectMode(bool activateSelectMode);

        const std::set<EntityID>& getSelectedEntities() const;
        bool isEntitySelected(EntityID entityId) const;
        void clearSelection();
        void addToSelection(EntityID entityId);
        void removeFromSelection(EntityID entityId);
        void toggleSelection(EntityID entityId);

        static glm::mat4 getOrComputeTransformMatrix(Transform* t);
        static void transformAABB(const glm::vec3& localMin, const glm::vec3& localMax,
                                 const glm::mat4& transform,
                                 glm::vec3& outMin, glm::vec3& outMax);
        static bool intersectsRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                     const glm::vec3& aabbMin, const glm::vec3& aabbMax,
                                     float& outT);

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        EventManager& _eventManager;
        CameraManager* _cameraManager = nullptr;
        ViewportManager* _viewportManager = nullptr;

        bool _isSelectMode = true;
        EntityID _selectedEntity = 0;
        std::set<EntityID> _selectedEntities;

        void _handleMouseEvent(const MouseEvent& e);
        EntityID _performRaycastInActiveViewport(const glm::vec2& mouseGlobalPos);
        void _updateSelection(EntityID selected);
        void _updateMultiSelection();
};
