#include "UI/TranformPanel.hpp"

TranformPanel::TranformPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem) {}

void TranformPanel::render()
{
    EntityID entityId = this->_selectionSystem.getSelectedEntity();

    if (entityId == INVALID_ENTITY)
        return;

        Transform* transform = this->_componentRegistry.getComponent<Transform>(entityId);

    if (!transform) {
        ImGui::Button("Add Color Component");
        if (ImGui::IsItemClicked()) _addTransformComponent(entityId);

        return;
    }

    bool edited = false;

    glm::vec3 rotationDegrees = glm::degrees(transform->rotation);
    glm::vec3 scalePercent = transform->scale * 100.0f;

    ImGui::Separator();
    ImGui::Text("Transform");
    ImGui::Text("( x )\t\t\t\t( y )\t\t\t\t( z )");

    edited |= ImGui::InputFloat3("Position", &transform->position[0]);
    edited |= ImGui::InputFloat3("Rotation (°)", &rotationDegrees[0]);
    edited |= ImGui::InputFloat3("Scale (%)", &scalePercent[0]);

    if (edited) {
        transform->rotation = glm::radians(rotationDegrees);
        transform->scale = scalePercent / 100.0f;
        transform->isDirty = true;
    }
}

void TranformPanel::_addTransformComponent(EntityID entityId)
{
    if (entityId == INVALID_ENTITY) return;

    this->_componentRegistry.registerComponent<Transform>(entityId, Transform(glm::vec3(0, 0, 0)));
}
