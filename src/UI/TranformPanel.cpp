#include "UI/TranformPanel.hpp"
#include <functional>

TranformPanel::TranformPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem) {}

void TranformPanel::render()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (selectedEntities.empty()) {
        this->_hasStoredValues = false;
        this->_prevSelectedEntities.clear();
        return;
    }

    bool allHaveTransform = true;
    int tmpCount = 0;
    for (EntityID id : selectedEntities) {
        tmpCount++;
        if (!this->_componentRegistry.getComponent<Transform>(id))
            allHaveTransform = false;
    }

    if (!allHaveTransform && tmpCount > 1) {
        ImGui::Separator();
        ImGui::Text("Some selected entities don't have Transform component");
        this->_hasStoredValues = false;
        this->_prevSelectedEntities.clear();
        return;
    } else if (!allHaveTransform && tmpCount == 1) {
        ImGui::Button("Add Transform Component");
        if (ImGui::IsItemClicked()) {
            EntityID onlyEntity = *selectedEntities.begin();
            this->_addTransformComponent(onlyEntity);
        }
    }

    EntityID primaryEntity = this->_selectionSystem.getSelectedEntity();
    Transform* primaryTransform = this->_componentRegistry.getComponent<Transform>(primaryEntity);

    if (!primaryTransform) {
        this->_hasStoredValues = false;
        this->_prevSelectedEntities.clear();
        return;
    }

    bool selectionChanged = (this->_prevSelectedEntities != selectedEntities);
    if (selectionChanged) {
        this->_prevPosition = primaryTransform->position;
        this->_prevRotation = primaryTransform->rotation;
        this->_prevScale = primaryTransform->scale;
        this->_prevPrimaryEntity = primaryEntity;
        this->_prevSelectedEntities = selectedEntities;
        this->_hasStoredValues = true;
    }

    glm::bvec3 positionComponentsSame = this->_checkWhichComponentsAreSame(selectedEntities,
        [](Transform* t) { return t->position; });
    glm::bvec3 rotationComponentsSame = this->_checkWhichComponentsAreSame(selectedEntities,
        [](Transform* t) { return t->rotation; });
    glm::bvec3 scaleComponentsSame = this->_checkWhichComponentsAreSame(selectedEntities,
        [](Transform* t) { return t->scale; });

    glm::vec3 editPosition = primaryTransform->position;
    glm::vec3 editRotationDegrees = glm::degrees(primaryTransform->rotation);
    glm::vec3 editScalePercent = primaryTransform->scale * 100.0f;

    ImGui::Separator();

    if (selectedEntities.size() > 1) ImGui::Text("Transform (%zu entities selected)", selectedEntities.size());
    else ImGui::Text("Transform");

    ImGui::Columns(4, nullptr, false);
    ImGui::SetColumnWidth(0, 80);
    ImGui::SetColumnWidth(1, 80);
    ImGui::SetColumnWidth(2, 80);
    ImGui::SetColumnWidth(3, 80);

    ImGui::NextColumn();
    ImGui::Text("X");
    ImGui::NextColumn();
    ImGui::Text("Y");
    ImGui::NextColumn();
    ImGui::Text("Z");
    ImGui::NextColumn();

    bool positionEdited = this->_displayRowPos(editPosition, positionComponentsSame);
    bool rotationEdited = this->_displayRowRot(editRotationDegrees, rotationComponentsSame);
    bool scaleEdited = this->_displayRowScale(editScalePercent, scaleComponentsSame);

    ImGui::Columns(1);

    if (!selectionChanged && (positionEdited || rotationEdited || scaleEdited)) {
        glm::vec3 deltaPosition(0.0f);
        glm::vec3 deltaRotation(0.0f);
        glm::vec3 deltaScale(0.0f);

        if (positionEdited) deltaPosition = editPosition - this->_prevPosition;
        if (rotationEdited) deltaRotation = glm::radians(editRotationDegrees) - this->_prevRotation;
        if (scaleEdited) deltaScale = (editScalePercent / 100.0f) - this->_prevScale;

        for (EntityID id : selectedEntities) {
            Transform* transform = this->_componentRegistry.getComponent<Transform>(id);
            if (transform) {
                if (positionEdited) transform->position += deltaPosition;
                if (rotationEdited) transform->rotation += deltaRotation;
                if (scaleEdited) transform->scale += deltaScale;
                transform->isDirty = true;
            }
        }

        if (positionEdited) this->_prevPosition = editPosition;
        if (rotationEdited) this->_prevRotation = glm::radians(editRotationDegrees);
        if (scaleEdited) this->_prevScale = editScalePercent / 100.0f;
    }
}

void TranformPanel::_addTransformComponent(EntityID entityId)
{
    if (entityId == INVALID_ENTITY) return;

    this->_componentRegistry.registerComponent<Transform>(entityId, Transform(glm::vec3(0, 0, 0)));
}

bool TranformPanel::_checkAllEntitiesHaveSameValue(
    const std::set<EntityID>& entities,
    std::function<glm::vec3(Transform*)> getter,
    glm::vec3& outValue) const
{
    if (entities.empty()) return false;

    bool first = true;
    glm::vec3 referenceValue;

    for (EntityID id : entities) {
        Transform* transform = this->_componentRegistry.getComponent<Transform>(id);
        if (!transform) return false;

        glm::vec3 value = getter(transform);

        if (first) {
            referenceValue = value;
            first = false;
        } else {
            const float epsilon = 0.0001f;
            if (glm::length(value - referenceValue) > epsilon)
                return false;
        }
    }

    outValue = referenceValue;
    return true;
}

glm::bvec3 TranformPanel::_checkWhichComponentsAreSame(
    const std::set<EntityID>& entities,
    std::function<glm::vec3(Transform*)> getter) const
{
    glm::bvec3 result(true, true, true);

    if (entities.empty()) return glm::bvec3(false, false, false);

    bool first = true;
    glm::vec3 referenceValue;
    const float epsilon = 0.0001f;

    for (EntityID id : entities) {
        Transform* transform = this->_componentRegistry.getComponent<Transform>(id);
        if (!transform) return glm::bvec3(false, false, false);

        glm::vec3 value = getter(transform);

        if (first) {
            referenceValue = value;
            first = false;
        } else {
            if (fabs(value.x - referenceValue.x) > epsilon) result.x = false;
            if (fabs(value.y - referenceValue.y) > epsilon) result.y = false;
            if (fabs(value.z - referenceValue.z) > epsilon) result.z = false;
        }
    }

    return result;
}

bool TranformPanel::_displayRowPos(glm::vec3& editPosition, const glm::bvec3& componentsSame)
{
    ImGui::Text("Pos");
    ImGui::NextColumn();

    ImGui::PushID("position");
    bool posXEdited = false, posYEdited = false, posZEdited = false;

    ImGui::BeginDisabled(!componentsSame.x);
    ImGui::SetNextItemWidth(-1);
    posXEdited = ImGui::InputFloat("##posX", &editPosition.x);
    ImGui::EndDisabled();
    if (!componentsSame.x) posXEdited = false;
    ImGui::NextColumn();

    ImGui::BeginDisabled(!componentsSame.y);
    ImGui::SetNextItemWidth(-1);
    posYEdited = ImGui::InputFloat("##posY", &editPosition.y);
    ImGui::EndDisabled();
    if (!componentsSame.y) posYEdited = false;
    ImGui::NextColumn();

    ImGui::BeginDisabled(!componentsSame.z);
    ImGui::SetNextItemWidth(-1);
    posZEdited = ImGui::InputFloat("##posZ", &editPosition.z);
    ImGui::EndDisabled();
    if (!componentsSame.z) posZEdited = false;
    ImGui::NextColumn();
    ImGui::PopID();

    return posXEdited || posYEdited || posZEdited;
}

bool TranformPanel::_displayRowRot(glm::vec3& editRotationDegrees, const glm::bvec3& componentsSame)
{
    ImGui::Text("Rot (°)");
    ImGui::NextColumn();

    ImGui::PushID("rotation");
    bool rotXEdited = false, rotYEdited = false, rotZEdited = false;

    ImGui::BeginDisabled(!componentsSame.x);
    ImGui::SetNextItemWidth(-1);
    rotXEdited = ImGui::InputFloat("##rotX", &editRotationDegrees.x);
    ImGui::EndDisabled();
    if (!componentsSame.x) rotXEdited = false;
    ImGui::NextColumn();

    ImGui::BeginDisabled(!componentsSame.y);
    ImGui::SetNextItemWidth(-1);
    rotYEdited = ImGui::InputFloat("##rotY", &editRotationDegrees.y);
    ImGui::EndDisabled();
    if (!componentsSame.y) rotYEdited = false;
    ImGui::NextColumn();

    ImGui::BeginDisabled(!componentsSame.z);
    ImGui::SetNextItemWidth(-1);
    rotZEdited = ImGui::InputFloat("##rotZ", &editRotationDegrees.z);
    ImGui::EndDisabled();
    if (!componentsSame.z) rotZEdited = false;
    ImGui::NextColumn();
    ImGui::PopID();

    return rotXEdited || rotYEdited || rotZEdited;
}

bool TranformPanel::_displayRowScale(glm::vec3& editScalePercent, const glm::bvec3& componentsSame)
{
    ImGui::Text("Scale (%%)");
    ImGui::NextColumn();

    ImGui::PushID("scale");
    bool scaleXEdited = false, scaleYEdited = false, scaleZEdited = false;

    ImGui::BeginDisabled(!componentsSame.x);
    ImGui::SetNextItemWidth(-1);
    scaleXEdited = ImGui::InputFloat("##scaleX", &editScalePercent.x);
    ImGui::EndDisabled();
    if (!componentsSame.x) scaleXEdited = false;
    ImGui::NextColumn();

    ImGui::BeginDisabled(!componentsSame.y);
    ImGui::SetNextItemWidth(-1);
    scaleYEdited = ImGui::InputFloat("##scaleY", &editScalePercent.y);
    ImGui::EndDisabled();
    if (!componentsSame.y) scaleYEdited = false;
    ImGui::NextColumn();

    ImGui::BeginDisabled(!componentsSame.z);
    ImGui::SetNextItemWidth(-1);
    scaleZEdited = ImGui::InputFloat("##scaleZ", &editScalePercent.z);
    ImGui::EndDisabled();
    if (!componentsSame.z) scaleZEdited = false;
    ImGui::NextColumn();
    ImGui::PopID();

    return scaleXEdited || scaleYEdited || scaleZEdited;
}
