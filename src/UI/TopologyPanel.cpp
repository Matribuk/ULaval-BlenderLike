#include "UI/TopologyPanel.hpp"

TopologyPanel::TopologyPanel(
    ComponentRegistry& registry,
    EntityManager& entityMgr,
    PrimitiveSystem& primitiveSys,
    SelectionSystem& selectionSys,
    SceneManager& sceneMgr
)
    : _registry(registry),
      _entityManager(entityMgr),
      _primitiveSystem(primitiveSys),
      _selectionSystem(selectionSys),
      _sceneManager(sceneMgr)
{
}

void TopologyPanel::draw()
{
    ImGui::Begin("Topology");

    ImGui::SeparatorText("Delaunay / Voronoi");
    this->_drawDelaunayControls();

    ImGui::End();
}

void TopologyPanel::_drawDelaunayControls()
{
    auto selectedEntities = this->_selectionSystem.getSelectedEntities();

    DelaunayMesh* selectedDelaunay = nullptr;
    EntityID selectedDelaunayId = 0;

    for (EntityID entityId : selectedEntities) {
        DelaunayMesh* delaunay = this->_registry.getComponent<DelaunayMesh>(entityId);
        if (delaunay) {
            selectedDelaunay = delaunay;
            selectedDelaunayId = entityId;
            break;
        }
    }

    if (selectedDelaunay) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "DelaunayMesh selected");

        const char* displayModes[] = {"Delaunay Only", "Voronoi Only", "Both"};
        int currentDisplayMode = static_cast<int>(selectedDelaunay->displayMode);

        if (ImGui::Combo("Display Mode", &currentDisplayMode, displayModes, 3)) {
            selectedDelaunay->displayMode = static_cast<DelaunayMesh::DisplayMode>(currentDisplayMode);
            this->_primitiveSystem.generateMeshes();
        }
    } else {
        ImGui::TextDisabled("No DelaunayMesh selected");
        ImGui::TextWrapped("Select a Delaunay entity to change display mode.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Generate from Selected Points", ImVec2(-1, 0))) {
        this->_generateFromSelectedPoints();
    }

    ImGui::TextWrapped("Select Point entities in the scene, then click to create a triangulation from them.");
}

void TopologyPanel::_generateFromSelectedPoints()
{
    auto selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (selectedEntities.empty()) {
        return;
    }

    std::vector<glm::vec2> points;

    for (EntityID entityId : selectedEntities) {
        Point* point = this->_registry.getComponent<Point>(entityId);
        Transform* transform = this->_registry.getComponent<Transform>(entityId);

        if (point && transform) {
            glm::vec3 pos = transform->position;
            points.push_back(glm::vec2(pos.x, pos.y));
        }
    }

    if (points.size() < 3) {
        return;
    }

    Entity entity = this->_entityManager.createEntity();

    DelaunayMesh delaunayMesh(points);
    delaunayMesh.mode = DelaunayMesh::GenerationMode::CUSTOM;

    this->_registry.registerComponent(entity.getId(), delaunayMesh);
    this->_registry.registerComponent(entity.getId(), Transform());
    this->_registry.registerComponent(entity.getId(), Renderable(ofMesh(), ofColor::white, true, nullptr, nullptr, true));
    this->_registry.registerComponent(entity.getId(), Selectable());

    this->_registry.registerComponent(entity.getId(), BoundingBoxVisualization(BoundingBoxVisualization::Type::AABB, false));

    std::string primitiveName = "Delaunay from Points " + std::to_string(entity.getId());
    this->_sceneManager.registerEntity(entity.getId(), primitiveName);

    this->_primitiveSystem.generateMeshes();
}
