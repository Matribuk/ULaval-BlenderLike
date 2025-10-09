#pragma once

#include "Core/EntityManager/EntityManager.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"

#include "Systems/PrimitiveSystem/PrimitiveSystem.hpp"
#include "Systems/CameraSystem/CameraSystem.hpp"

#include "Manager/InputManager/InputManager.hpp"
#include "Manager/FileManager/FileManager.hpp"
#include "Manager/CameraManager/CameraManager.hpp"
#include "Manager/SceneManager/SceneManager.hpp"
#include "Manager/ViewportManager/ViewportManager.hpp"

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Primitive/Sphere.hpp"

#include "Events/EventTypes/Events/SelectionEvent.hpp"
#include "Events/EventManager/EventManager.hpp"

#include "UI/Toolbar/Toolbar.hpp"

#include <vector>
#include <functional>

class ActionManager {
    public:
        ActionManager(
            EntityManager& entityManager,
            ComponentRegistry& componentRegistry,
            PrimitiveSystem& primitiveSystem,
            FileManager& fileManager,
            EventManager& eventManager,
            ViewportManager& viewportManager,
            CameraManager& cameraManager,
            std::vector<EntityID>& testEntities
        );

        void toggleIsolateSelection();
        void updateCameraAction(Toolbar* toolbar = nullptr);

        void registerAllActions();

        void setSelectedEntity(EntityID entity);
        EntityID getSelectedEntity() const;

    private:
        bool _isIsolated = false;
        std::unordered_map<EntityID, bool> _savedVisibilityStates;

        void _exportSelectedEntity();
        void _selectEntity(int index);
        void _registerKeyboardActions();
        void _registerShortcuts();
        void _isolateEntity(EntityID entity);
        void _showAllEntities();

        EntityManager& _entityManager;
        ComponentRegistry& _componentRegistry;
        PrimitiveSystem& _primitiveSystem;
        FileManager& _fileManager;
        EventManager& _eventManager;
        ViewportManager& _viewportManager;
        CameraManager& _cameraManager;
        std::vector<EntityID>& _testEntities;

        EntityID _selectedEntity = INVALID_ENTITY;
};
