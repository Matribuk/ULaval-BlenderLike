#include "UI/SkyboxPanel.hpp"

SkyboxPanel::SkyboxPanel(RenderSystem& renderSystem) : _renderSystem(renderSystem) {}

void SkyboxPanel::render()
{
    if (ImGui::Begin("Skybox Settings", nullptr, ImGuiWindowFlags_NoCollapse)) {

        ImGui::Text("Cubemap Skybox");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Load: parc", ImVec2(-1, 0)))
            this->_renderSystem.loadCubemap("cubemaps/parc");

        if (ImGui::Button("Load: city", ImVec2(-1, 0)))
            this->_renderSystem.loadCubemap("cubemaps/city");
    }
    ImGui::End();
}
