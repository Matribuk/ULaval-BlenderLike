#include "UI/ColorPanel.hpp"

ColorPanel::ColorPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem)
    : _color(ofColor::white), _componentRegistry(componentRegistry), _selectionSystem(selectionSystem),
      _prevColor(ofColor::white)
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

bool ColorPanel::_checkAllEntitiesHaveSameColor(const std::set<EntityID>& entities, ofColor& outColor) const
{
    if (entities.empty()) return false;

    bool first = true;
    ofColor referenceColor;

    for (EntityID id : entities) {
        Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
        if (!renderable) return false;

        if (first) {
            referenceColor = renderable->color;
            first = false;
        } else
            if (renderable->color != referenceColor) return false;
    }

    outColor = referenceColor;
    return true;
}

void ColorPanel::render()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (selectedEntities.empty()) {
        this->_prevSelectedEntities.clear();
        return;
    }

    bool allHaveRenderable = true;
    int tmpCount = 0;
    for (EntityID id : selectedEntities) {
        tmpCount++;
        if (!this->_componentRegistry.getComponent<Renderable>(id))
            allHaveRenderable = false;
    }

    if (!allHaveRenderable && tmpCount > 1) {
        ImGui::Text("Some selected entities don't have Renderable component");
        this->_prevSelectedEntities.clear();
        return;
    } else if (!allHaveRenderable && tmpCount == 1) {
        ImGui::Button("Add Renderable Component");
        if (ImGui::IsItemClicked()) {
            EntityID onlyEntity = *selectedEntities.begin();
            this->_addColorComponent(onlyEntity);
        }
    }

    EntityID primaryEntity = this->_selectionSystem.getSelectedEntity();
    Renderable* primaryRenderable = this->_componentRegistry.getComponent<Renderable>(primaryEntity);

    if (!primaryRenderable) {
        this->_prevSelectedEntities.clear();
        return;
    }

    if (this->_prevSelectedEntities != selectedEntities) {
        this->_prevColor = primaryRenderable->color;
        this->_prevSelectedEntities = selectedEntities;
    }

    ofColor commonColor;
    bool colorSame = this->_checkAllEntitiesHaveSameColor(selectedEntities, commonColor);

    ImVec4 color = colorSame ? ImVec4(commonColor) : ImVec4(primaryRenderable->color);

    bool edited = false;
    if (selectedEntities.size() > 1) ImGui::Text("Color (%zu entities selected)", selectedEntities.size());
    else ImGui::Text("Color");

    ImGui::BeginDisabled(!colorSame);
    edited |= ImGui::ColorEdit4("RGBA", (float*)&color);

    ImGui::Spacing();
    ImGui::Text("Picker");
    edited |= ImGui::ColorPicker4("##picker", (float*)&color);
    ImGui::EndDisabled();

    if (!colorSame) edited = false;

    if (edited) {
        this->_color = ofColor(
            static_cast<unsigned char>(color.x * 255.0f),
            static_cast<unsigned char>(color.y * 255.0f),
            static_cast<unsigned char>(color.z * 255.0f),
            static_cast<unsigned char>(color.w * 255.0f)
        );

        for (EntityID id : selectedEntities) {
            Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
            if (renderable) renderable->color = this->_color;
        }

        this->_prevColor = this->_color;
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
