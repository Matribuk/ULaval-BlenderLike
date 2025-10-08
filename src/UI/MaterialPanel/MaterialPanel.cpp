#include "UI/MaterialPanel/MaterialPanel.hpp"

MaterialPanel::MaterialPanel(ComponentRegistry& componentRegistry, EntityID entityId)
    : _entityId(entityId), _componentRegistry(componentRegistry)
{
    this->_entityId = entityId;
    if (entityId == INVALID_ENTITY)
        return;
    if (this->_componentRegistry.hasComponent<Renderable>(entityId))
        this->_renderable = this->_componentRegistry.getComponent<Renderable>(entityId);
}

void MaterialPanel::setSelectedEntity(EntityID id) {
    this->_entityId = id;
    if (id == INVALID_ENTITY)
        return;
    if (this->_componentRegistry.hasComponent<Renderable>(id))
        this->_renderable = this->_componentRegistry.getComponent<Renderable>(id);
}

void MaterialPanel::unsetSelectedEntity() {
    this->_entityId = INVALID_ENTITY;
    this->_renderable = nullptr;
}

void MaterialPanel::render()
{
    if (this->_entityId == INVALID_ENTITY)
        return;

    if (this->_renderable && this->_renderable->material) {
        ImGui::Text("Material:");

        if (this->_renderable->material->shader)
            ImGui::Text(" - Shader: Set");
        else
            ImGui::Text(" - Shader: None");

        if (this->_renderable->material->texture)
            ImGui::Text(" - Texture: Set");
        else
            ImGui::Text(" - Texture: None");

        ImGui::Separator();

        ImGui::Checkbox("Visible", &this->_renderable->visible);
    }
}