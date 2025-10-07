#pragma once

#include "Core/EntityManager/EntityManager.hpp"
#include "Core/ComponentRegistry/ComponentRegistry.hpp"

#include "Systems/PrimitiveSystem/PrimitiveSystem.hpp"
#include "Systems/CameraSystem/CameraSystem.hpp"

#include "Manager/InputManager/InputManager.hpp"
#include "Manager/FileManager/FileManager.hpp"
#include "Manager/CameraManager/CameraManager.hpp"

#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"
#include "Components/Primitive/Sphere.hpp"

#include "Events/EventTypes/Events/SelectionEvent.hpp"
#include "Events/EventManager/EventManager.hpp"

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
            std::vector<EntityID>& testEntities
        );

        void updateCameraAction(std::unique_ptr<CameraManager>& camera);

        void registerAllActions();

        void setSelectedEntity(EntityID entity);
        EntityID getSelectedEntity() const;

    private:
        void _exportSelectedEntity();
        void _selectEntity(int index);
        void _registerKeyboardActions();
        void _registerShortcuts();

        EntityManager& _entityManager;
        ComponentRegistry& _componentRegistry;
        PrimitiveSystem& _primitiveSystem;
        FileManager& _fileManager;
        EventManager& _eventManager;
        std::vector<EntityID>& _testEntities;

        EntityID _selectedEntity = INVALID_ENTITY;
};
