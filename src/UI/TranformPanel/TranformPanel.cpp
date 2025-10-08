#include "UI/TranformPanel/TranformPanel.hpp"

TranformPanel::TranformPanel(ComponentRegistry& componentRegistry, EntityID entityId)
    : _entityId(entityId), _componentRegistry(componentRegistry)
{
    this->_entityId = entityId;
    if (entityId == INVALID_ENTITY)
        return;
    if (this->_componentRegistry.hasComponent<Transform>(entityId))
        this->_transform = this->_componentRegistry.getComponent<Transform>(entityId);
}

void TranformPanel::setSelectedEntity(EntityID id) {
    this->_entityId = id;
    if (id == INVALID_ENTITY)
        return;
    if (this->_componentRegistry.hasComponent<Transform>(id))
        this->_transform = this->_componentRegistry.getComponent<Transform>(id);
}

void TranformPanel::unsetSelectedEntity() {
    this->_entityId = INVALID_ENTITY;
    this->_transform = nullptr;
}

void TranformPanel::render()
{
    if (this->_entityId == INVALID_ENTITY)
        return;


    if (this->_transform) {
        bool edited = false;
        ImGui::Separator();
        ImGui::Text("Transform");
        ImGui::Text("( x )\t\t\t\t( y )\t\t\t\t( z )");
        edited |= ImGui::InputFloat3("Position", &this->_transform->position[0]);
        edited |= ImGui::InputFloat3("Rotation", &this->_transform->rotation[0]);
        edited |= ImGui::InputFloat3("Scale", &this->_transform->scale[0]);
        if (edited)
            this->_transform->isDirty = true;
    }
}