#include "UI/CurvesPanel.hpp"

CurvesPanel::CurvesPanel(
    EntityManager& entityManager,
    ComponentRegistry& componentRegistry,
    SceneManager& sceneManager,
    PrimitiveSystem& primitiveSystem
)
    : _entityManager(entityManager)
    , _componentRegistry(componentRegistry)
    , _sceneManager(sceneManager)
    , _primitiveSystem(primitiveSystem)
    , _eventLogPanel(nullptr)
    , _resolution(50)
    , _showControlPoints(true)
    , _showTangents(false)
{}

void CurvesPanel::render()
{
    if (ImGui::Begin("Parametric Curves", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Generate Curves from Selected Points");
        ImGui::Separator();
        ImGui::Spacing();

        std::vector<EntityID> selectedPoints = this->_getSelectedPointEntities();
        int numSelected = selectedPoints.size();

        ImGui::Text("Selected Points: %d", numSelected);
        ImGui::Spacing();

        ImGui::SliderInt("Resolution", &this->_resolution, 10, 200);
        ImGui::Checkbox("Show Control Points", &this->_showControlPoints);
        ImGui::Checkbox("Show Tangents", &this->_showTangents);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canGenerateBezier = (numSelected == 4);
        if (!canGenerateBezier) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Generate Bezier Curve", ImVec2(-1, 60))) {
            this->_generateBezierFromSelectedPoints();
        }

        if (!canGenerateBezier) {
            ImGui::EndDisabled();
            if (numSelected < 4) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Need exactly 4 points");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Too many points selected (need 4)");
            }
        }

        ImGui::Spacing();

        bool canGenerateCatmull = (numSelected >= 4);
        if (!canGenerateCatmull) {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Generate Catmull-Rom Spline", ImVec2(-1, 60))) {
            this->_generateCatmullRomFromSelectedPoints();
        }

        if (!canGenerateCatmull) {
            ImGui::EndDisabled();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Need at least 4 points");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("Select points in the scene, then click a button to generate a curve passing through them.");
    }
    ImGui::End();
}

void CurvesPanel::setEventLogPanel(EventLogPanel* eventLog)
{
    this->_eventLogPanel = eventLog;
}

std::vector<EntityID> CurvesPanel::_getSelectedPointEntities()
{
    std::vector<EntityID> selectedPoints;

    for (EntityID id : this->_entityManager.getAllEntities()) {
        Selectable* selectable = this->_componentRegistry.getComponent<Selectable>(id);
        Point* point = this->_componentRegistry.getComponent<Point>(id);

        if (selectable && point && selectable->isSelected) {
            EntityID parentId = this->_sceneManager.getParent(id);
            if (parentId == INVALID_ENTITY) {
                selectedPoints.push_back(id);
            }
        }
    }

    return selectedPoints;
}

void CurvesPanel::_generateBezierFromSelectedPoints()
{
    std::vector<EntityID> pointEntities = this->_getSelectedPointEntities();

    if (pointEntities.size() != 4) {
        if (this->_eventLogPanel) {
            this->_eventLogPanel->addLog("Error: Need exactly 4 points for Bezier curve", ofColor::red);
        }
        return;
    }

    std::vector<glm::vec3> controlPoints;
    controlPoints.reserve(4);

    for (EntityID pointId : pointEntities) {
        Transform* transform = this->_componentRegistry.getComponent<Transform>(pointId);
        if (transform) {
            glm::vec3 globalPos = glm::vec3(transform->globalMatrix[3]);
            controlPoints.push_back(globalPos);
        }
    }

    if (controlPoints.size() != 4) {
        if (this->_eventLogPanel) {
            this->_eventLogPanel->addLog("Error: Could not retrieve all point positions", ofColor::red);
        }
        return;
    }

    Entity entity = this->_entityManager.createEntity();

    ParametricCurve curve(ParametricCurve::Type::BEZIER_CUBIC, this->_resolution);
    curve.controlPointEntities = pointEntities;
    curve.showControlPoints = this->_showControlPoints;
    curve.showTangents = this->_showTangents;

    curve.controlPoints = controlPoints;

    this->_componentRegistry.registerComponent(entity.getId(), Transform(glm::vec3(0.0f)));
    this->_componentRegistry.registerComponent(entity.getId(), curve);
    this->_componentRegistry.registerComponent(entity.getId(), Renderable(ofMesh(), ofColor::lightGray, true, nullptr, nullptr, true));
    this->_componentRegistry.registerComponent(entity.getId(), Selectable());
    this->_componentRegistry.registerComponent(entity.getId(), BoundingBoxVisualization(BoundingBoxVisualization::Type::AABB, false));

    std::string curveName = "Bezier " + std::to_string(entity.getId());
    this->_sceneManager.registerEntity(entity.getId(), curveName);

    for (EntityID pointId : pointEntities) {
        this->_sceneManager.setParent(pointId, entity.getId());
    }

    this->_primitiveSystem.generateMeshes();

    if (this->_eventLogPanel) {
        this->_eventLogPanel->addLog("Created " + curveName + " from selected points", ofColor::cyan);
    }
}

void CurvesPanel::_generateCatmullRomFromSelectedPoints()
{
    std::vector<EntityID> pointEntities = this->_getSelectedPointEntities();

    if (pointEntities.size() < 4) {
        if (this->_eventLogPanel) {
            this->_eventLogPanel->addLog("Error: Need at least 4 points for Catmull-Rom spline", ofColor::red);
        }
        return;
    }

    std::vector<glm::vec3> controlPoints;
    controlPoints.reserve(pointEntities.size());

    for (EntityID pointId : pointEntities) {
        Transform* transform = this->_componentRegistry.getComponent<Transform>(pointId);
        if (transform) {
            glm::vec3 globalPos = glm::vec3(transform->globalMatrix[3]);
            controlPoints.push_back(globalPos);
        }
    }

    if (controlPoints.size() < 4) {
        if (this->_eventLogPanel) {
            this->_eventLogPanel->addLog("Error: Could not retrieve all point positions", ofColor::red);
        }
        return;
    }

    Entity entity = this->_entityManager.createEntity();

    ParametricCurve curve(ParametricCurve::Type::CATMULL_ROM, this->_resolution);
    curve.controlPointEntities = pointEntities;
    curve.showControlPoints = this->_showControlPoints;
    curve.showTangents = this->_showTangents;

    curve.controlPoints = controlPoints;

    this->_componentRegistry.registerComponent(entity.getId(), Transform(glm::vec3(0.0f)));
    this->_componentRegistry.registerComponent(entity.getId(), curve);
    this->_componentRegistry.registerComponent(entity.getId(), Renderable(ofMesh(), ofColor::lightGray, true, nullptr, nullptr, true));
    this->_componentRegistry.registerComponent(entity.getId(), Selectable());
    this->_componentRegistry.registerComponent(entity.getId(), BoundingBoxVisualization(BoundingBoxVisualization::Type::AABB, false));

    std::string curveName = "CatmullRom " + std::to_string(entity.getId());
    this->_sceneManager.registerEntity(entity.getId(), curveName);

    for (EntityID pointId : pointEntities) {
        this->_sceneManager.setParent(pointId, entity.getId());
    }

    this->_primitiveSystem.generateMeshes();

    if (this->_eventLogPanel) {
        this->_eventLogPanel->addLog("Created " + curveName + " from selected points", ofColor::cyan);
    }
}
