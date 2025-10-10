#include "UI/ColorPanel.hpp"

ColorPanel::ColorPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem)
    : _color(ofColor::white), _componentRegistry(componentRegistry), _selectionSystem(selectionSystem)
{
    if (this->_componentRegistry.hasComponent<Renderable>(selectionSystem.getSelectedEntity())) {
        Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(selectionSystem.getSelectedEntity());
        if (renderable)
            this->_color = renderable->color;
        else
            this->_color = ofColor::white;
    } else
        this->_color = ofColor::white;
}

void ColorPanel::render()
{
    EntityID entityId = this->_selectionSystem.getSelectedEntity();

    if (entityId == INVALID_ENTITY)
        return;

    Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(entityId);

    if (!renderable) {
        ImGui::Button("Add Color Component");
        if (ImGui::IsItemClicked()) _addColorComponent(entityId);

        return;
    }

    ImVec4 color = renderable->color;

    bool edited = false;
    ImGui::Text("Color");
    edited |= ImGui::ColorEdit4("RGBA", (float*)&color);

    ImGui::Spacing();
    ImGui::Text("Picker");
    edited |= ImGui::ColorPicker4("##picker", (float*)&color);

    if (edited) {
        this->_color = ofColor(
            static_cast<unsigned char>(color.x * 255.0f),
            static_cast<unsigned char>(color.y * 255.0f),
            static_cast<unsigned char>(color.z * 255.0f),
            static_cast<unsigned char>(color.w * 255.0f)
        );
        setSelectedColor(this->_color, renderable);
    }
}

void ColorPanel::setSelectedColor(ofColor color, Renderable* renderable)
{
    this->_color = color;
    renderable->color = color;
}

const ofColor& ColorPanel::getSelectedColor() const
{
    return this->_color;
}

void ColorPanel::_addColorComponent(EntityID entityId)
{
    if (entityId == INVALID_ENTITY) return;

    this->_componentRegistry.registerComponent<Renderable>(entityId, Renderable(ofMesh(), ofColor::white));
}
