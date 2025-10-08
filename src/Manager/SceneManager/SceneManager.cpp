#include "Manager/SceneManager/SceneManager.hpp"
#include "Events/EventTypes/Events/SelectionEvent.hpp"

SceneManager::SceneManager(
    EntityManager& entityManager, ComponentRegistry& componentRegistry, EventManager& eventManager)
    : _entityManager(entityManager), _componentRegistry(componentRegistry), _eventManager(eventManager) {}

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

    for (EntityID childId : it->second.children) {
        auto childIt = this->_entities.find(childId);
        if (childIt != this->_entities.end()) {
            childIt->second.parent = INVALID_ENTITY;
            this->_rootEntities.push_back(childId);
        }
    }

    this->_entities.erase(it);
}

void SceneManager::setParent(EntityID child, EntityID parent)
{
    auto childIt = this->_entities.find(child);
    auto parentIt = this->_entities.find(parent);

    if (childIt == this->_entities.end() || parentIt == this->_entities.end())
        return;

    if (childIt->second.parent != INVALID_ENTITY)
        removeParent(child);

    auto rootIt = std::find(this->_rootEntities.begin(), this->_rootEntities.end(), child);
    if (rootIt != this->_rootEntities.end())
        this->_rootEntities.erase(rootIt);

    childIt->second.parent = parent;
    parentIt->second.children.push_back(child);
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
}

void SceneManager::selectEntity(EntityID id)
{
    this->_selectedEntity = id;
    this->_eventManager.emit(SelectionEvent(id, true));
}

void SceneManager::render()
{
    ImGui::Spacing();

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SameLine();

    if (ImGui::Button("create Entity")) {
        if (_selectedEntity != INVALID_ENTITY) {
            // do noting for now
            std::cout << "nice try man" << std::endl;
        }
    }
    if (ImGui::Button("Delete Selected")) {
        if (this->_selectedEntity != INVALID_ENTITY) {
            this->_entityManager.destroyEntity(this->_selectedEntity);
            unregisterEntity(this->_selectedEntity);
            this->_selectedEntity = INVALID_ENTITY;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginChild("EntityTree", ImVec2(0, 0), true);

    for (EntityID rootId : this->_rootEntities)
        this->_renderEntityNode(rootId);

    ImGui::EndChild();
}

void SceneManager::_renderEntityNode(EntityID id, int depth)
{
    auto it = this->_entities.find(id);
    if (it == this->_entities.end())
        return;

    EntityNode& node = it->second;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                             | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (node.children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    if (id == this->_selectedEntity)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool opened = ImGui::TreeNodeEx((void*)(intptr_t)id, flags, "%s", node.name.c_str());

    if (ImGui::IsItemClicked())
        selectEntity(id);

    if (ImGui::BeginPopupContextItem()) {
        ImGui::Text("Entity: %s", node.name.c_str());
        ImGui::Separator();

        if (ImGui::MenuItem("Delete")) {
            this->_entityManager.destroyEntity(id);
            unregisterEntity(id);
            if (this->_selectedEntity == id)
                this->_selectedEntity = INVALID_ENTITY;
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
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("ENTITY_NODE", &id, sizeof(EntityID));
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE")) {
            EntityID draggedId = *(const EntityID*)payload->Data;
            if (draggedId != id) {
                setParent(draggedId, id);
                ofLogNotice("SceneManager") << "Set parent: " << draggedId << " -> " << id;
            }
        }
        ImGui::EndDragDropTarget();
    }
}

std::string SceneManager::_generateDefaultName(EntityID id)
{
    return std::to_string(id);
}