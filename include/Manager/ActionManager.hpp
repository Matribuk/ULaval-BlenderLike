#pragma once

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Manager/CameraManager.hpp"
#include "Manager/ViewportManager.hpp"
#include "Manager/InputManager.hpp"

#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/Renderable.hpp"

#include "Systems/SelectionSystem.hpp"

#include "Events/EventManager.hpp"

#include "UI/Toolbar.hpp"

#include <vector>
#include <unordered_map>
#include <ofxImGui.h>

class ActionManager {
    public:
        ActionManager(
            ComponentRegistry& componentRegistry,
            EntityManager& entityManager,
            EventManager& eventManager,
            CameraManager& cameraManager,
            ViewportManager& viewportManager,
            SelectionSystem& selectionSystem
        );

        ~ActionManager() = default;

        void registerAllActions();
        void updateCameraControls(Toolbar* toolbar = nullptr);
        void toggleIsolateSelection();

    private:
        ComponentRegistry& _componentRegistry;
        EntityManager& _entityManager;
        EventManager& _eventManager;
        CameraManager& _cameraManager;
        ViewportManager& _viewportManager;
        SelectionSystem& _selectionSystem;

        bool _isIsolated = false;
        std::unordered_map<EntityID, bool> _savedVisibilityStates;

        void _registerKeyboardActions();
        void _registerShortcuts();
        void _isolateEntity(EntityID entity);
        void _showAllEntities();
        void _handleCameraMovement(EntityID cameraId, Toolbar* toolbar);
        void _handleKeyboardCameraControls();
};
