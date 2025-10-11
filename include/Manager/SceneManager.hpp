#pragma once

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Systems/TransformSystem.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/SelectionEvent.hpp"

#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"

#include "Manager/InputManager.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include "ofxImGui.h"

class SelectionSystem;

struct EntityNode {
    EntityID id;
    std::string name;
    bool visible = true;
    EntityID parent = INVALID_ENTITY;
    std::vector<EntityID> children;
};

class SceneManager {
    public:
        SceneManager(EntityManager& entityManager, ComponentRegistry& componentRegistry, EventManager& eventManager, TransformSystem& transformSystem );
        ~SceneManager() = default;

        void render();

        void registerEntity(EntityID id, const std::string& name = "");
        void unregisterEntity(EntityID id);

        void setParent(EntityID child, EntityID parent);
        void removeParent(EntityID child);

        void selectEntity(EntityID id);
        EntityID getSelectedEntity() const { return _selectedEntity; }

        const std::set<EntityID>& getSelectedEntities() const { return _selectedEntities; }
        void toggleEntitySelection(EntityID id);
        bool isEntitySelected(EntityID id) const;
        void clearSelection();

        const std::vector<EntityID>& getRootEntities() const { return _rootEntities; }

        void setSelectionSystem(SelectionSystem& selectionSystem);

    private:
        EntityManager& _entityManager;
        ComponentRegistry& _componentRegistry;
        EventManager& _eventManager;
        TransformSystem& _transformSystem;
        SelectionSystem* _selectionSystem = nullptr;

        std::unordered_map<EntityID, EntityNode> _entities;
        std::vector<EntityID> _rootEntities;
        EntityID _selectedEntity = INVALID_ENTITY;
        std::set<EntityID> _selectedEntities;

        bool _isDescendant(EntityID entityId, EntityID targetId) const;
        void _renderEntityNode(EntityID id, int depth = 0);
        std::string _generateDefaultName(EntityID id);
        void _handleDragDrop(EntityID id);
};