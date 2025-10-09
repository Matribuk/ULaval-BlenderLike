#include "UI/SkyboxPanel/SkyboxPanel.hpp"

SkyboxPanel::SkyboxPanel(RenderSystem& renderSystem)
    : _renderSystem(renderSystem),
      _topColor(0.529f, 0.808f, 0.922f),
      _bottomColor(0.9f, 0.95f, 1.0f)
{
    this->_applyColors();
}

void SkyboxPanel::render()
{
    if (ImGui::Begin("Skybox Settings", nullptr, ImGuiWindowFlags_NoCollapse)) {

        float top[3]    = {this->_topColor.r, this->_topColor.g, this->_topColor.b};
        float bottom[3] = {this->_bottomColor.r, this->_bottomColor.g, this->_bottomColor.b};

        bool changed = false;

        if (ImGui::ColorEdit3("Top Color", top)) {
            this->_topColor = glm::vec3(top[0], top[1], top[2]);
            changed = true;
        }

        if (ImGui::ColorEdit3("Bottom Color", bottom)) {
            this->_bottomColor = glm::vec3(bottom[0], bottom[1], bottom[2]);
            changed = true;
        }

        if (changed) this->_applyColors();

        if (ImGui::Button("Preset: Day")) this->_setPresetDay();

        ImGui::SameLine();

        if (ImGui::Button("Preset: Night")) this->_setPresetNight();
    }
    ImGui::End();
}

void SkyboxPanel::_applyColors() const
{
    this->_renderSystem.setSkyColors(this->_topColor, this->_bottomColor);
}

void SkyboxPanel::_setPresetDay()
{
    this->_topColor = glm::vec3(0.529f, 0.808f, 0.922f);
    this->_bottomColor = glm::vec3(0.9f, 0.95f, 1.0f);
    this->_applyColors();
}

void SkyboxPanel::_setPresetNight()
{
    this->_topColor = glm::vec3(0.05f, 0.05f, 0.2f);
    this->_bottomColor = glm::vec3(0.1f, 0.1f, 0.3f);
    this->_applyColors();
}

void SkyboxPanel::setTopColor(const glm::vec3& color)
{
    this->_topColor = color;
    this->_applyColors();
}

void SkyboxPanel::setBottomColor(const glm::vec3& color)
{
    this->_bottomColor = color;
    this->_applyColors();
}

glm::vec3 SkyboxPanel::getTopColor() const
{
    return this->_topColor;
}

glm::vec3 SkyboxPanel::getBottomColor() const
{
    return this->_bottomColor;
}
