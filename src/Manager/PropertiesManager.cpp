#include "Manager/PropertiesManager.hpp"

PropertiesManager::PropertiesManager(
    TranformPanel& tranformPanel,
    MaterialPanel& materialPanel,
    ColorPanel& ColorPanel,
    SceneManager& sceneManager,
    ComponentRegistry& componentRegistry,
    SelectionSystem& selectionSystem
) :
    _tranformPanel(tranformPanel),
    _materialPanel(materialPanel),
    _ColorPanel(ColorPanel),
    _sceneManager(sceneManager),
    _componentRegistry(componentRegistry),
    _selectionSystem(selectionSystem)
{}

void PropertiesManager::render()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.12f, 0.14f, 1.0f));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Inspector", nullptr, flags))
    {
        float halfHeight = (ofGetWindowHeight() - 60) * 0.4f;

        ImGui::BeginChild("SceneHierarchy", ImVec2(0, halfHeight), true);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Scene Hierarchy");
        ImGui::Separator();
        ImGui::Spacing();

        this->_sceneManager.render();

        ImGui::EndChild();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::BeginChild("PropertiesPanel", ImVec2(0, 0), true);
        ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "Properties");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            this->_tranformPanel.render();
            ImGui::Unindent(10.0f);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::SmallButton("x##DeleteColor"))
        this->_deleteComponnent("Color");

        ImGui::SameLine();

        if (ImGui::CollapsingHeader("Color", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            this->_ColorPanel.render();
            ImGui::Unindent(10.0f);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::SmallButton("x##DeleteMaterial"))
        this->_deleteComponnent("Material");

        ImGui::SameLine();

        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            this->_materialPanel.render();
            ImGui::Unindent(10.0f);
        }

        ImGui::EndChild();
    }

    ImGui::End();
    ImGui::PopStyleColor(1);
}

void PropertiesManager::_deleteComponnent(std::string componentName)
{
    EntityID selected = this->_sceneManager.getSelectedEntity();
    if (selected == INVALID_ENTITY) return;

    if (componentName == "Color" && this->_componentRegistry.hasComponent<Renderable>(selected))
        this->_componentRegistry.removeComponent<Renderable>(selected);

    if (componentName == "Material" && this->_componentRegistry.hasComponent<Renderable>(selected))
        this->_componentRegistry.removeComponent<Renderable>(selected);

}
