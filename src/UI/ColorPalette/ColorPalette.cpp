#include "UI/ColorPalette/ColorPalette.hpp"

ColorPalette::ColorPalette(EntityID entityId, ComponentRegistry& componentRegistry)
    : _color(ofColor::white), _entityId(entityId), _componentRegistry(componentRegistry), _renderable(nullptr)
{
    if (this->_componentRegistry.hasComponent<Renderable>(this->_entityId)) {
        this->_renderable = this->_componentRegistry.getComponent<Renderable>(this->_entityId);
        if (this->_renderable)
            this->_color = this->_renderable->color;
        else
            this->_color = ofColor::white;
    } else
        this->_color = ofColor::white;
}

void ColorPalette::render()
{
    if (!this->_renderable)
        return;

    ImVec4 color = ImVec4(
        this->_color.r / 255.0f,
        this->_color.g / 255.0f,
        this->_color.b / 255.0f,
        this->_color.a / 255.0f
    );

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
        setSelectedColor(this->_color);
    }
}

void ColorPalette::setSelectedColor(ofColor color)
{
    this->_color = color;

    if (this->_renderable) this->_renderable->color = this->_color;
}

const ofColor& ColorPalette::getSelectedColor() const
{
    return this->_color;
}

void ColorPalette::setEntity(EntityID entityId)
{
    this->_entityId = entityId;
    if (this->_componentRegistry.hasComponent<Renderable>(this->_entityId)) {
        this->_renderable = this->_componentRegistry.getComponent<Renderable>(this->_entityId);
        this->_color = this->_renderable->color;
    } else {
        this->_renderable = nullptr;
        this->_color = ofColor::white;
    }
}
