#include "UI/PrimitivesPanel.hpp"

PrimitivesPanel::PrimitivesPanel(
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
    , _selectedPrimitive(PrimitiveType::Cube)
    , _cubeSize(1.0f, 1.0f, 1.0f)
    , _sphereRadius(1.0f)
    , _spawnPosition(0.0f, 0.0f, 0.0f)
    , _primitiveColor(ofColor::white)
{}

void PrimitivesPanel::render()
{
    if (ImGui::Begin("Primitives", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("Add a Geometric Primitive");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Cube", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Cube);

        if (ImGui::Button("Sphere", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Sphere);


        ImGui::Spacing();
        ImGui::Text("Add a Vector Primitive");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Plane", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Plane);

        if (ImGui::Button("Triangle", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Triangle);

        if (ImGui::Button("Circle", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Circle);

        if (ImGui::Button("Line", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Line);

        if (ImGui::Button("Rectangle", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Rectangle);

        if (ImGui::Button("Point", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::Point);

    }
    ImGui::End();
}


void PrimitivesPanel::setEventLogPanel(EventLogPanel* eventLog)
{
    this->_eventLogPanel = eventLog;
}

void PrimitivesPanel::_createPrimitive(PrimitiveType type)
{
    Entity entity = this->_entityManager.createEntity();

    glm::vec3 defaultPos(0.0f);
    glm::vec3 defaultSize(1.0f);
    float defaultSphereRadius = 1.0f;
    ofColor defaultColor = ofColor::lightGray;
    Triangle defaultTriangle(
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    Circle defaultCircle(2.0f, 64);
    Line defaultLine(glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0));
    Rectangle defaultRect(2.0f, 3.0f);
    Point defaultPoint(1.0f);

    this->_componentRegistry.registerComponent(entity.getId(), Transform(defaultPos));

    std::string primitiveName;
    if (type == PrimitiveType::Cube) {
        this->_componentRegistry.registerComponent(entity.getId(), Box(defaultSize));
        primitiveName = "Cube " + std::to_string(entity.getId());
    } else if (type == PrimitiveType::Sphere) {
        this->_componentRegistry.registerComponent(entity.getId(), Sphere(defaultSphereRadius));
        primitiveName = "Sphere " + std::to_string(entity.getId());
    } else if (type == PrimitiveType::Plane) {
        this-> _componentRegistry.registerComponent(entity.getId(), Plane(defaultSize));
        primitiveName = "Plane " + std::to_string(entity.getId());
    } else if (type == PrimitiveType::Triangle) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultTriangle);
        primitiveName = "Triangle " + std::to_string(entity.getId());
    } else if (type == PrimitiveType::Circle) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultCircle);
        primitiveName = "Circle " + std::to_string(entity.getId());
    } else if (type == PrimitiveType::Line) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultLine);
        primitiveName = "Line " + std::to_string(entity.getId());
    } else if (type == PrimitiveType::Rectangle) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultRect);
        primitiveName = "Rectangle " + std::to_string(entity.getId());
    } else if (type == PrimitiveType::Point) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultPoint);
        primitiveName = "Point " + std::to_string(entity.getId());
    }

    this->_componentRegistry.registerComponent(entity.getId(), Renderable(ofMesh(), defaultColor, true, nullptr, nullptr, true));
    this->_componentRegistry.registerComponent(entity.getId(), Selectable());

    this->_sceneManager.registerEntity(entity.getId(), primitiveName);

    this->_primitiveSystem.generateMeshes();

    if (this->_eventLogPanel)
        this->_eventLogPanel->addLog("Created " + primitiveName + " (default parameters)", ofColor::cyan);
}
