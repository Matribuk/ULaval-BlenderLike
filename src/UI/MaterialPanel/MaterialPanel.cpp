#include "UI/MaterialPanel/MaterialPanel.hpp"

MaterialPanel::MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem)
{
}

void MaterialPanel::render()
{
    EntityID selectedEntity = this->_selectionSystem.getSelectedEntity();
    Renderable* renderable = _componentRegistry.getComponent<Renderable>(selectedEntity);
    if (this->_selectionSystem.getSelectedEntity() == INVALID_ENTITY)
        return;

    if (this->_renderable) {
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

            ImGui::Separator();

            ImGui::Checkbox("Visible", &this->_renderable->visible);
        }
    }
}
