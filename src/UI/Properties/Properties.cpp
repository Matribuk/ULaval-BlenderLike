#include "UI/Properties/Properties.hpp"

Properties::Properties(ComponentRegistry& componentRegistry, EntityID entityId)
    : _entityId(entityId), _componentRegistry(componentRegistry)
{
    this->_entityId = entityId;
    if (entityId == INVALID_ENTITY)
        return;
    if (this->_componentRegistry.hasComponent<Transform>(entityId))
        this->_transform = _componentRegistry.getComponent<Transform>(entityId);
    if (this->_componentRegistry.hasComponent<Renderable>(entityId))
        this->_renderable = _componentRegistry.getComponent<Renderable>(entityId);
}

void Properties::setSelectedEntity(EntityID id) {
    this->_entityId = id;
    if (id == INVALID_ENTITY)
        return;
    if (this->_componentRegistry.hasComponent<Transform>(id))
        this->_transform = _componentRegistry.getComponent<Transform>(id);
    if (this->_componentRegistry.hasComponent<Renderable>(id))
        this->_renderable = _componentRegistry.getComponent<Renderable>(id);
}

void Properties::unsetSelectedEntity() {
    this->_entityId = INVALID_ENTITY;
    this->_transform = nullptr;
    this->_renderable = nullptr;
}

void Properties::render()
{
    ImGui::SetNextWindowPos(ImVec2(1090, 80));
    ImGui::SetNextWindowSize(ImVec2(300, 220));

    if (ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoResize)) {
        if (this->_entityId == INVALID_ENTITY)
            ImGui::Text("No entity selected.");
        else {
            ImGui::Text("Entity ID: %d", this->_entityId);
            if (this->_transform) {
                bool edited = false;
                ImGui::Separator();
                ImGui::Text("Transform");
                edited |= ImGui::InputFloat3("Position", &this->_transform->position[0]);
                edited |= ImGui::InputFloat3("Rotation", &this->_transform->rotation[0]);
                edited |= ImGui::InputFloat3("Scale", &this->_transform->scale[0]);
                if (edited) this->_transform->isDirty = true;
            } else
                ImGui::Text("No Transform component.");
            if (this->_renderable) {
                ImGui::Separator();
                ImGui::Text("Renderable");
                std::string colors_str = "Color: (" +
                    std::to_string(this->_renderable->color.r) + ", " +
                    std::to_string(this->_renderable->color.g) + ", " +
                    std::to_string(this->_renderable->color.b) + ")";
                ImGui::Text("%s", colors_str.c_str());
                ImGui::Checkbox("Visible", &this->_renderable->visible);

                if (this->_renderable->material) {
                    ImGui::Text("Material:");
                    if (this->_renderable->material->shader)
                        ImGui::Text(" - Shader: Set");
                    else
                        ImGui::Text(" - Shader: None");

                    if (this->_renderable->material->texture)
                        ImGui::Text(" - Texture: Set");
                    else
                        ImGui::Text(" - Texture: None");
                } else {
                    ImGui::Text("Material: None");
                }
            } else
                ImGui::Text("No Renderable component.");
        }
    }
    ImGui::End();
}