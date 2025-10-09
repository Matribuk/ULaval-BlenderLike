#include "UI/TranformPanel/TranformPanel.hpp"

TranformPanel::TranformPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem)
{
}

void TranformPanel::render()
{
    EntityID selectedEntity = this->_selectionSystem.getSelectedEntity();
    Transform* transform = this->_componentRegistry.getComponent<Transform>(selectedEntity);
    if (selectedEntity == INVALID_ENTITY)
        return;

    if (transform) {
        bool edited = false;

        glm::vec3 rotationDegrees = glm::degrees(transform->rotation);
        glm::vec3 scalePercent = transform->scale * 100.0f;

        ImGui::Separator();
        ImGui::Text("Transform");
        ImGui::Text("( x )\t\t\t\t( y )\t\t\t\t( z )");
        edited |= ImGui::InputFloat3("Position", &transform->position[0]);
        edited |= ImGui::InputFloat3("Rotation (°)", &transform->rotation[0]);
        edited |= ImGui::InputFloat3("Scale (%)", &transform->scale[0]);

        if (edited) {
            transform->rotation = glm::radians(rotationDegrees);
            transform->scale = scalePercent / 100.0f;
            transform->isDirty = true;
        }
    }
}
