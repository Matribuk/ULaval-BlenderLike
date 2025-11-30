#include "UI/RaytracingPanel.hpp"

RaytracingPanel::RaytracingPanel(RenderSystem& renderSystem)
    : _renderSystem(renderSystem) {}

void RaytracingPanel::render() {
    if (ImGui::Begin("Raytracing")) {
        bool rtEnabled = _renderSystem.getRaytracingEnabled();
        if (ImGui::Checkbox("Enable Raytracing", &rtEnabled)) {
            _renderSystem.setRaytracingEnabled(rtEnabled);
        }

        if (rtEnabled) {
            ImGui::Indent();
            ImGui::TextWrapped("Each shader enhances its own features:");
            ImGui::BulletText("lambert: adds raytraced shadows");
            ImGui::BulletText("phong: adds raytraced shadows");
            ImGui::BulletText("phong_shadow: shadow maps -> raytraced shadows");
            ImGui::BulletText("reflective_phong: shadows + cubemap -> raytraced (both)");
            ImGui::BulletText("mirror: cubemap -> raytraced reflections");
            ImGui::Unindent();
        }
    }
    ImGui::End();
}
