#include "UI/LightPanel.hpp"
#include "Core/Entity.hpp"
#include <cmath>

LightPanel::LightPanel(ComponentRegistry& componentRegistry, EntityManager& entityManager, SelectionSystem& selectionSystem, SceneManager& sceneManager)
    : _componentRegistry(componentRegistry), _entityManager(entityManager), _selectionSystem(selectionSystem), _sceneManager(sceneManager) {}

void LightPanel::render()
{
    if (ImGui::Begin("Lights"))
        this->renderContent();
    ImGui::End();
}

void LightPanel::renderContent()
{
    ImGui::Text("Light Management");
    ImGui::Separator();

    this->_renderLightCreation();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Existing Lights:");

    const std::set<EntityID>& selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (!selectedEntities.empty()) {
        EntityID primaryEntity = *selectedEntities.begin();
        LightSource* light = this->_componentRegistry.getComponent<LightSource>(primaryEntity);

        if (light)
            this->_renderLightProperties(primaryEntity, light);
        else {
            ImGui::TextDisabled("Selected entity is not a light");
            ImGui::Text("Select a light entity to edit its properties");
        }
    } else {
        ImGui::TextDisabled("No light selected");
        ImGui::Text("Select a light entity from the scene to edit");
    }
}

void LightPanel::renderProperties()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (selectedEntities.empty()) {
        ImGui::TextDisabled("No entity selected");
        return;
    }

    EntityID primaryEntity = *selectedEntities.begin();
    LightSource* light = this->_componentRegistry.getComponent<LightSource>(primaryEntity);

    if (!light) {
        ImGui::TextDisabled("Selected entity is not a light");
        return;
    }

    this->_renderLightProperties(primaryEntity, light);
}

void LightPanel::_renderLightCreation()
{
    ImGui::Text("Create New Light:");

    static int selectedType = 1;
    const char* lightTypes[] = {"Ambient", "Directional", "Point", "Spot"};
    ImGui::Combo("Light Type", &selectedType, lightTypes, IM_ARRAYSIZE(lightTypes));

    if (ImGui::Button("Create Light", ImVec2(-1, 0))) {
        Entity lightEntity = this->_entityManager.createEntity();
        EntityID lightEntityId = lightEntity.getId();

        Transform transform;
        transform.position = glm::vec3(0.0f, 5.0f, 0.0f);
        this->_componentRegistry.registerComponent(lightEntityId, transform);

        LightSource light;
        light.type = static_cast<LightType>(selectedType);
        light.position = transform.position;
        light.color = glm::vec3(1.0f, 1.0f, 1.0f);
        light.intensity = 1.0f;
        light.enabled = true;

        switch (light.type) {
            case LightType::DIRECTIONAL:
                light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
                light.attenuation = 0.0f;
                break;
            case LightType::POINT:
                light.attenuation = 0.1f;
                break;
            case LightType::SPOT:
                light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
                light.spotAngle = 45.0f;
                light.attenuation = 0.1f;
                break;
            default:
                break;
        }

        this->_componentRegistry.registerComponent(lightEntityId, light);

        ofSpherePrimitive sphere;
        sphere.set(0.3f, 16);
        ofMesh visualMesh = sphere.getMesh();

        ofColor lightColor(light.color.x * 255, light.color.y * 255, light.color.z * 255);
        this->_componentRegistry.registerComponent(lightEntityId, Renderable(visualMesh, lightColor, true, nullptr, nullptr, true));

        this->_componentRegistry.registerComponent(lightEntityId, Selectable());

        std::string lightName = std::string(lightTypes[selectedType]) + " Light";
        this->_sceneManager.registerEntity(lightEntityId, lightName);

        this->_selectionSystem.clearSelection();
        this->_selectionSystem.addToSelection(lightEntityId);
    }
}

void LightPanel::_renderLightProperties(EntityID lightId, LightSource* light)
{
    ImGui::PushID(lightId);

    ImGui::Text("Type: %s", this->_getLightTypeName(light->type));
    ImGui::Spacing();

    if (ImGui::Checkbox("Enabled", &light->enabled)) {}

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Appearance");
    ImGui::Spacing();

    if (ImGui::ColorEdit3("Color", &light->color.x)) {}

    if (ImGui::SliderFloat("Intensity", &light->intensity, 0.0f, 10.0f, "%.2f")) {}

    if (light->type == LightType::DIRECTIONAL || light->type == LightType::SPOT) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Direction");
        ImGui::Spacing();

        glm::vec3 dirDegrees = glm::degrees(glm::vec3(
            asin(glm::clamp(light->direction.y, -1.0f, 1.0f)),
            atan2(light->direction.x, light->direction.z),
            0.0f
        ));

        if (ImGui::SliderFloat("Pitch", &dirDegrees.x, -89.0f, 89.0f, "%.1f deg")) {
            float pitch = glm::radians(dirDegrees.x);
            float yaw = glm::radians(dirDegrees.y);
            light->direction.x = cos(pitch) * sin(yaw);
            light->direction.y = sin(pitch);
            light->direction.z = cos(pitch) * cos(yaw);
            light->direction = glm::normalize(light->direction);
        }

        if (ImGui::SliderFloat("Yaw", &dirDegrees.y, -180.0f, 180.0f, "%.1f deg")) {
            float pitch = glm::radians(dirDegrees.x);
            float yaw = glm::radians(dirDegrees.y);
            light->direction.x = cos(pitch) * sin(yaw);
            light->direction.y = sin(pitch);
            light->direction.z = cos(pitch) * cos(yaw);
            light->direction = glm::normalize(light->direction);
        }

        if (ImGui::TreeNode("Advanced Direction")) {
            if (ImGui::DragFloat3("Direction Vector", &light->direction.x, 0.01f, -1.0f, 1.0f)) {
                light->direction = glm::normalize(light->direction);
            }
            ImGui::TreePop();
        }
    }

    if (light->type == LightType::SPOT) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Spot Light Settings");
        ImGui::Spacing();

        if (ImGui::SliderFloat("Spot Angle", &light->spotAngle, 1.0f, 90.0f, "%.1f deg")) {}
    }

    if (light->type == LightType::POINT || light->type == LightType::SPOT) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Attenuation");
        ImGui::Spacing();

        if (ImGui::SliderFloat("Falloff", &light->attenuation, 0.0f, 1.0f, "%.3f")) {}
    }

    ImGui::PopID();
}

const char* LightPanel::_getLightTypeName(LightType type)
{
    switch (type) {
        case LightType::AMBIENT: return "Ambient";
        case LightType::DIRECTIONAL: return "Directional";
        case LightType::POINT: return "Point";
        case LightType::SPOT: return "Spot";
        default: return "Unknown";
    }
}

ofMesh LightPanel::_createLightVisualizationMesh(LightType type)
{
    ofMesh mesh;

    switch (type) {
        case LightType::AMBIENT: {
            ofSpherePrimitive sphere;
            sphere.set(0.3f, 12);
            mesh = sphere.getMesh();
            break;
        }
        case LightType::DIRECTIONAL: {
            ofPlanePrimitive plane;
            plane.set(0.5f, 0.5f, 2, 2);
            mesh = plane.getMesh();
            break;
        }
        case LightType::POINT: {
            ofSpherePrimitive sphere;
            sphere.set(0.2f, 12);
            mesh = sphere.getMesh();
            break;
        }
        case LightType::SPOT: {
            ofConePrimitive cone;
            cone.set(0.3f, 0.6f, 12, 2);
            mesh = cone.getMesh();
            break;
        }
        default: {
            ofBoxPrimitive box;
            box.set(0.2f, 0.2f, 0.2f);
            mesh = box.getMesh();
            break;
        }
    }

    return mesh;
}

ofMesh LightPanel::_createDirectionIndicatorMesh(LightType type)
{
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_LINES);

    float arrowLength = 1.0f;
    float arrowHeadSize = 0.15f;
    int numArrows = 5;

    if (type == LightType::SPOT) {
        float coneAngle = 30.0f;
        float radius = tan(glm::radians(coneAngle)) * arrowLength;

        for (int i = 0; i < numArrows; i++) {
            float angle = (i / (float)numArrows) * TWO_PI;
            float x = cos(angle) * radius;
            float z = sin(angle) * radius;

            mesh.addVertex(glm::vec3(0, 0, 0));
            mesh.addVertex(glm::vec3(x, -arrowLength, z));

            glm::vec3 arrowTip(x, -arrowLength, z);
            glm::vec3 perpX = glm::normalize(glm::cross(glm::vec3(0, -1, 0), glm::vec3(x, 0, z))) * arrowHeadSize;
            glm::vec3 perpY = glm::vec3(0, arrowHeadSize, 0);

            mesh.addVertex(arrowTip);
            mesh.addVertex(arrowTip + perpY);

            mesh.addVertex(arrowTip);
            mesh.addVertex(arrowTip + perpX);
        }
    } else if (type == LightType::DIRECTIONAL) {
        for (int i = 0; i < 3; i++) {
            float offset = (i - 1) * 0.3f;

            mesh.addVertex(glm::vec3(offset, 0, 0));
            mesh.addVertex(glm::vec3(offset, -arrowLength, 0));

            glm::vec3 arrowTip(offset, -arrowLength, 0);
            mesh.addVertex(arrowTip);
            mesh.addVertex(arrowTip + glm::vec3(arrowHeadSize, arrowHeadSize, 0));

            mesh.addVertex(arrowTip);
            mesh.addVertex(arrowTip + glm::vec3(-arrowHeadSize, arrowHeadSize, 0));
        }
    }

    return mesh;
}
