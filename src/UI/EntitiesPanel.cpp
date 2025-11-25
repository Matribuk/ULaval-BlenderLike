#include "UI/EntitiesPanel.hpp"

EntitiesPanel::EntitiesPanel()
    : _assetsPanel(nullptr),
      _lightPanel(nullptr),
      _primitivesPanel(nullptr),
      _topologyPanel(nullptr)
{}

void EntitiesPanel::setupPanels(
    AssetsPanel& assetsPanel,
    LightPanel& lightPanel,
    PrimitivesPanel& primitivesPanel,
    TopologyPanel& topologyPanel
)
{
    this->_assetsPanel = &assetsPanel;
    this->_lightPanel = &lightPanel;
    this->_primitivesPanel = &primitivesPanel;
    this->_topologyPanel = &topologyPanel;
}

void EntitiesPanel::render()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.14f, 1.0f));

    if (ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "Entity Management");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            if (this->_assetsPanel) this->_assetsPanel->renderContent();
            ImGui::Unindent(10.0f);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            if (this->_lightPanel) this->_lightPanel->renderContent();
            ImGui::Unindent(10.0f);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Primitives", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            if (this->_primitivesPanel) this->_primitivesPanel->renderContent();
            ImGui::Unindent(10.0f);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Topology", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            if (this->_topologyPanel) this->_topologyPanel->drawContent();
            ImGui::Unindent(10.0f);
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(1);
}
