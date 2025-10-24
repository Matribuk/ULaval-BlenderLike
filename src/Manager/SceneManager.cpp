#include "Manager/SceneManager.hpp"
#include "Manager/CameraManager.hpp"
#include "Events/EventTypes/SelectionEvent.hpp"
#include "Systems/SelectionSystem.hpp"

SceneManager::SceneManager(
    EntityManager& entityManager, ComponentRegistry& componentRegistry, EventManager& eventManager, TransformSystem& transformSystem )
    : _entityManager(entityManager), _componentRegistry(componentRegistry), _eventManager(eventManager), _transformSystem(transformSystem) {}

void SceneManager::registerEntity(EntityID id, const std::string& name)
{
    if (this->_entities.find(id) != this->_entities.end())
        return;

    EntityNode node;
    node.id = id;
    node.name = name.empty() ? this->_generateDefaultName(id) : name;
    node.visible = true;
    node.parent = INVALID_ENTITY;

    this->_entities[id] = node;
    this->_rootEntities.push_back(id);
}

void SceneManager::unregisterEntity(EntityID id)
{
    auto it = this->_entities.find(id);
    if (it == this->_entities.end())
        return;

    if (it->second.parent != INVALID_ENTITY)
        removeParent(id);

    auto rootIt = std::find(this->_rootEntities.begin(), this->_rootEntities.end(), id);
    if (rootIt != this->_rootEntities.end())
        this->_rootEntities.erase(rootIt);

    std::vector<EntityID> childrenCopy = it->second.children;
    for (EntityID childId : childrenCopy) {
        this->_entityManager.destroyEntity(childId);
        unregisterEntity(childId);
    }

    this->_entities.erase(it);
}

void SceneManager::setParent(EntityID child, EntityID parent)
{
    auto childIt = this->_entities.find(child);
    auto parentIt = this->_entities.find(parent);

    if (childIt == this->_entities.end() || parentIt == this->_entities.end())
        return;

    if (this->_isDescendant(parent, child))
        return;

    if (childIt->second.parent != INVALID_ENTITY)
        removeParent(child);

    auto rootIt = std::find(this->_rootEntities.begin(), this->_rootEntities.end(), child);
    if (rootIt != this->_rootEntities.end())
        this->_rootEntities.erase(rootIt);

    childIt->second.parent = parent;
    parentIt->second.children.push_back(child);

    this->_transformSystem.setParent(child, parent);
}

void SceneManager::removeParent(EntityID child)
{
    auto childIt = this->_entities.find(child);
    if (childIt == this->_entities.end() || childIt->second.parent == INVALID_ENTITY)
        return;

    EntityID parentId = childIt->second.parent;
    auto parentIt = this->_entities.find(parentId);

    if (parentIt != this->_entities.end()) {
        auto& children = parentIt->second.children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    childIt->second.parent = INVALID_ENTITY;
    this->_rootEntities.push_back(child);

    this->_transformSystem.removeParent(child);
}

void SceneManager::render()
{
    if (!this->_selectionSystem) return;

    EntityID selectedEntity = this->_selectionSystem->getSelectedEntity();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Create Entity")) {
        _showCreateEntityPopup = true;
        _selectedEntityType = 0;
        std::strcpy(_entityNameBuffer, "");
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete Selected")) {
        if (selectedEntity != INVALID_ENTITY) {
            bool isCamera = this->_componentRegistry.hasComponent<Camera>(selectedEntity);

            this->_entityManager.destroyEntity(selectedEntity);
            unregisterEntity(selectedEntity);
            this->_selectionSystem->clearSelection();

            if (isCamera && this->_cameraManager) {
                this->_cameraManager->removeCamera(selectedEntity);

                if (this->_cameraManager->getActiveCameraId() == INVALID_ENTITY) {
                    EntityID newCameraId = this->_cameraManager->addCamera(glm::vec3(0, 5, 10));
                    this->registerEntity(newCameraId, "Camera " + std::to_string(newCameraId));
                }
            }
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginChild("EntityTree", ImVec2(0, 0), true);

    for (EntityID rootId : this->_rootEntities)
        this->_renderEntityNode(rootId);

    ImGui::InvisibleButton("##RootDropZone", ImVec2(-1, 50));
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
            EntityID draggedId = *(const EntityID*)payload->Data;
            auto it = this->_entities.find(draggedId);
            if (it != this->_entities.end() && it->second.parent != INVALID_ENTITY) {
                removeParent(draggedId);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemHovered()) {
        ImGui::GetWindowDrawList()->AddRect(
            ImGui::GetItemRectMin(),
            ImGui::GetItemRectMax(),
            ImGui::GetColorU32(ImGuiCol_DragDropTarget),
            0.0f,
            0,
            2.0f
        );
    }

    ImGui::EndChild();

    _renderCreateEntityPopup();
}

void SceneManager::_renderCreateEntityPopup()
{
    if (!_showCreateEntityPopup) return;

    ImGui::OpenPopup("Create Entity");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Create Entity", &_showCreateEntityPopup, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Choose Entity Type");
        ImGui::Separator();
        ImGui::Spacing();

        const char* entityTypes[] = {
            "Empty Entity",
            "Camera",
            "Box",
            "Sphere",
            "Plane",
            "Point",
            "Line",
            "Triangle",
            "Rectangle",
            "Circle"
        };

        ImGui::Combo("Type", &_selectedEntityType, entityTypes, IM_ARRAYSIZE(entityTypes));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Entity Name (optional):");
        ImGui::InputText("##entityname", _entityNameBuffer, sizeof(_entityNameBuffer));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            std::string entityName = std::string(_entityNameBuffer);
            _createEntity(_selectedEntityType, entityName);
            _showCreateEntityPopup = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            _showCreateEntityPopup = false;
        }

        ImGui::EndPopup();
    }
}

void SceneManager::_createEntity(int type, const std::string& name)
{
    EntityID newEntityId = this->_entityManager.createEntity().getId();
    Transform transform(glm::vec3(0, 0, 0));
    this->_componentRegistry.registerComponent<Transform>(newEntityId, transform);

    std::string entityName = name.empty() ? "" : name;

    switch (type) {
        case 0:
            if (entityName.empty()) entityName = "Entity " + std::to_string(newEntityId);
            break;

        case 1:
            if (this->_cameraManager) {
                this->_entityManager.destroyEntity(newEntityId);
                newEntityId = this->_cameraManager->addCamera(glm::vec3(0, 5, 10));
                if (entityName.empty()) entityName = "Camera " + std::to_string(newEntityId);
            }
            break;

        case 2:
            this->_componentRegistry.registerComponent<Box>(newEntityId, Box());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Box " + std::to_string(newEntityId);
            break;

        case 3:
            this->_componentRegistry.registerComponent<Sphere>(newEntityId, Sphere());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Sphere " + std::to_string(newEntityId);
            break;

        case 4:
            this->_componentRegistry.registerComponent<Plane>(newEntityId, Plane());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Plane " + std::to_string(newEntityId);
            break;

        case 5:
            this->_componentRegistry.registerComponent<Point>(newEntityId, Point());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Point " + std::to_string(newEntityId);
            break;

        case 6:
            this->_componentRegistry.registerComponent<Line>(newEntityId, Line());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Line " + std::to_string(newEntityId);
            break;

        case 7:
            this->_componentRegistry.registerComponent<Triangle>(newEntityId, Triangle());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Triangle " + std::to_string(newEntityId);
            break;

        case 8:
            this->_componentRegistry.registerComponent<Rectangle>(newEntityId, Rectangle());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Rectangle " + std::to_string(newEntityId);
            break;

        case 9:
            this->_componentRegistry.registerComponent<Circle>(newEntityId, Circle());
            this->_componentRegistry.registerComponent<Renderable>(newEntityId, Renderable());
            if (entityName.empty()) entityName = "Circle " + std::to_string(newEntityId);
            break;
    }

    this->registerEntity(newEntityId, entityName);
    this->_transformSystem.markDirty(newEntityId);
}

bool SceneManager::_isDescendant(EntityID entityId, EntityID targetId) const
{
    auto it = this->_entities.find(entityId);
    if (it == this->_entities.end())
        return false;

    if (it->second.parent == INVALID_ENTITY)
        return false;

    if (it->second.parent == targetId)
        return true;

    return this->_isDescendant(it->second.parent, targetId);
}

void SceneManager::_renderEntityNode(EntityID id, int depth)
{
    if (!this->_selectionSystem) return;

    auto it = this->_entities.find(id);
    if (it == this->_entities.end())
        return;

    EntityNode& node = it->second;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                             | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (node.children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    if (this->_selectionSystem->isEntitySelected(id))
        flags |= ImGuiTreeNodeFlags_Selected;

    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)id, flags, "%s", node.name.c_str());

    if (ImGui::IsItemClicked()) {
        InputManager& input = InputManager::get();
        bool isCtrlPressed = input.isKeyPressed(OF_KEY_LEFT_CONTROL) || input.isKeyPressed(OF_KEY_CONTROL);

        if (isCtrlPressed)
            this->_selectionSystem->toggleSelection(id);
        else {
            this->_selectionSystem->clearSelection();
            this->_selectionSystem->addToSelection(id);
            this->_selectionSystem->setSelectedEntity(id);
        }
    }

    if (ImGui::BeginPopupContextItem()) {
        ImGui::Text("Entity: %s", node.name.c_str());
        ImGui::Separator();

        if (node.parent != INVALID_ENTITY) {
            if (ImGui::MenuItem("Remove Parent"))
                removeParent(id);
            ImGui::Separator();
        }

        if (ImGui::MenuItem("Delete")) {
            bool isCamera = this->_componentRegistry.hasComponent<Camera>(id);

            this->_entityManager.destroyEntity(id);
            unregisterEntity(id);

            if (this->_selectionSystem->isEntitySelected(id))
                this->_selectionSystem->removeFromSelection(id);

            if (isCamera && this->_cameraManager) {
                this->_cameraManager->removeCamera(id);

                if (this->_cameraManager->getActiveCameraId() == INVALID_ENTITY) {
                    EntityID newCameraId = this->_cameraManager->addCamera(glm::vec3(0, 5, 10));
                    this->registerEntity(newCameraId, "Camera " + std::to_string(newCameraId));
                }
            }
        }

        ImGui::Separator();
        ImGui::EndPopup();
    }

    this->_handleDragDrop(id);

    if (opened) {
        for (EntityID childId : node.children)
            this->_renderEntityNode(childId, depth + 1);

        ImGui::TreePop();
    }
}

void SceneManager::_handleDragDrop(EntityID id)
{
    Camera* camera = this->_componentRegistry.getComponent<Camera>(id);

    if (!camera &&ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("ENTITY_NODE", &id, sizeof(EntityID));

        auto it = this->_entities.find(id);
        if (it != this->_entities.end())
            ImGui::Text("Moving: %s", it->second.name.c_str());

        ImGui::EndDragDropSource();
    }

    if (!camera && ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
            EntityID draggedId = *(const EntityID*)payload->Data;

                if (draggedId != id)
                    if (!this->_isDescendant(id, draggedId))
                        setParent(draggedId, id);
        }
        ImGui::EndDragDropTarget();
    }
}

std::string SceneManager::_generateDefaultName(EntityID id)
{
    return std::to_string(id);
}

void SceneManager::setSelectionSystem(SelectionSystem& selectionSystem)
{
    this->_selectionSystem = &selectionSystem;
}

void SceneManager::setCameraManager(CameraManager& cameraManager)
{
    this->_cameraManager = &cameraManager;
}

std::string SceneManager::getEntityName(EntityID id) const
{
    auto it = this->_entities.find(id);
    if (it != this->_entities.end()) {
        return it->second.name;
    }
    return "Unknown Entity";
}
