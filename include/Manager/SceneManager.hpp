#pragma once

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Systems/TransformSystem.hpp"
#include "Systems/SelectionSystem.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/SelectionEvent.hpp"

#include "Components/Camera.hpp"
#include "Components/Transform.hpp"

#include "Manager/CameraManager.hpp"
#include "Manager/InputManager.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include "ofxImGui.h"

class SelectionSystem;
class CameraManager;

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

        const std::vector<EntityID>& getRootEntities() const { return _rootEntities; }
        std::string getEntityName(EntityID id) const;

        void setSelectionSystem(SelectionSystem& selectionSystem);
        void setCameraManager(CameraManager& cameraManager);

    private:
        EntityManager& _entityManager;
        ComponentRegistry& _componentRegistry;
        EventManager& _eventManager;
        TransformSystem& _transformSystem;
        SelectionSystem* _selectionSystem = nullptr;
        CameraManager* _cameraManager = nullptr;

        std::unordered_map<EntityID, EntityNode> _entities;
        std::vector<EntityID> _rootEntities;

        bool _isDescendant(EntityID entityId, EntityID targetId) const;
        void _renderEntityNode(EntityID id, int depth = 0);
        void _createCamera();
        std::string _generateDefaultName(EntityID id);
        void _handleDragDrop(EntityID id);
};