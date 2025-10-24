#pragma once

#include "Core/EntityManager.hpp"
#include "Core/ComponentRegistry.hpp"

#include "Systems/TransformSystem.hpp"

#include "Events/EventManager.hpp"
#include "Events/EventTypes/SelectionEvent.hpp"

#include "Components/Primitive/Box.hpp"
#include "Components/Primitive/Sphere.hpp"
#include "Components/Primitive/Plane.hpp"
#include "Components/Primitive/Point.hpp"
#include "Components/Primitive/Line.hpp"
#include "Components/Primitive/Triangle.hpp"
#include "Components/Primitive/Rectangle.hpp"
#include "Components/Primitive/Circle.hpp"
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include "Components/Renderable.hpp"

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

        bool _showCreateEntityPopup = false;
        int _selectedEntityType = 0;
        char _entityNameBuffer[256] = "";

        bool _isDescendant(EntityID entityId, EntityID targetId) const;
        void _renderEntityNode(EntityID id, int depth = 0);
        void _renderCreateEntityPopup();
        void _createEntity(int type, const std::string& name);
        std::string _generateDefaultName(EntityID id);
        void _handleDragDrop(EntityID id);
};