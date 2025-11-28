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

            if (ImGui::TreeNode("Shadows")) {
                bool rtShadows = _renderSystem.getRaytracedShadowsEnabled();

                if (ImGui::RadioButton("Shadow Maps (Fast)", !rtShadows)) {
                    _renderSystem.setRaytracedShadowsEnabled(false);
                    _renderSystem.setShadowsEnabled(true);
                }

                if (ImGui::RadioButton("Raytraced Shadows (Precise)", rtShadows)) {
                    _renderSystem.setRaytracedShadowsEnabled(true);
                    _renderSystem.setShadowsEnabled(false);
                }

                ImGui::TreePop();
            }

            bool rtReflections = _renderSystem.getRaytracedReflectionsEnabled();
            if (ImGui::Checkbox("Raytraced Reflections", &rtReflections)) {
                _renderSystem.setRaytracedReflectionsEnabled(rtReflections);
            }

            if (rtReflections) {
                int maxBounces = _renderSystem.getMaxRayBounces();
                if (ImGui::SliderInt("Max Ray Bounces", &maxBounces, 1, 3)) {
                    _renderSystem.setMaxRayBounces(maxBounces);
                }
            }

            ImGui::Unindent();
        }
    }
    ImGui::End();
}
