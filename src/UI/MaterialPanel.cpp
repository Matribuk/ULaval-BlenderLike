#include "UI/MaterialPanel.hpp"

MaterialPanel::MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem){}

void MaterialPanel::render()
{
    EntityID entityId = this->_selectionSystem.getSelectedEntity();

    if (entityId == INVALID_ENTITY)
        return;

    Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(entityId);

    if (!renderable) {
        ImGui::Button("Add Material Component");
        if (ImGui::IsItemClicked()) _addMaterialComponent(entityId);

        return;
    }

    ImGui::Checkbox("Visible", &renderable->visible);

    if (renderable->material) {
        ImGui::Text("Material:");

        if (renderable->material->shader)
            ImGui::Text(" - Shader: Set");
        else
            ImGui::Text(" - Shader: None");

        if (renderable->material->texture) {
            ofTexture* tex = renderable->material->texture;
            ImGui::Text(" - Texture: Set");
            ImVec2 thumbSize = ImVec2(24, 24);
            GLuint texID = tex->getTextureData().textureID;
            ImGui::Image((ImTextureID)(uintptr_t)texID, thumbSize, ImVec2(0,1), ImVec2(1,0));
        } else
            ImGui::Text(" - Texture: None");

        ImGui::Separator();

    }
}

void MaterialPanel::_addMaterialComponent(EntityID entityId)
{
    if (entityId == INVALID_ENTITY) return;

    this->_componentRegistry.registerComponent<Renderable>(entityId, Renderable(ofMesh(), ofColor::white));
}
