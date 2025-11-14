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

        ImGui::Spacing();
        ImGui::Text("Add a Computational Geometry Primitive");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Delaunay Triangulation", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::DelaunayTriangulation);

        ImGui::Spacing();
        ImGui::Text("Add a Parametric Curve");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Bezier Curve", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::BezierCurve);

        if (ImGui::Button("Catmull-Rom Spline", ImVec2(-1, 60)))
            this->_createPrimitive(PrimitiveType::CatmullRomCurve);

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
    BoundingBoxVisualization::Type bboxType = BoundingBoxVisualization::Type::AABB;

    if (type == PrimitiveType::Cube) {
        this->_componentRegistry.registerComponent(entity.getId(), Box(defaultSize));
        primitiveName = "Cube " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::Sphere) {
        this->_componentRegistry.registerComponent(entity.getId(), Sphere(defaultSphereRadius));
        primitiveName = "Sphere " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::SPHERE;
    } else if (type == PrimitiveType::Plane) {
        this-> _componentRegistry.registerComponent(entity.getId(), Plane(defaultSize));
        primitiveName = "Plane " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::Triangle) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultTriangle);
        primitiveName = "Triangle " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::Circle) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultCircle);
        primitiveName = "Circle " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::Line) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultLine);
        primitiveName = "Line " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::Rectangle) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultRect);
        primitiveName = "Rectangle " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::Point) {
        this-> _componentRegistry.registerComponent(entity.getId(), defaultPoint);
        primitiveName = "Point " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::SPHERE;
    } else if (type == PrimitiveType::DelaunayTriangulation) {
        std::vector<glm::vec2> points2D = {
            glm::vec2(-3.0f, -2.0f),
            glm::vec2(-1.0f, -2.5f),
            glm::vec2(1.0f, -2.0f),
            glm::vec2(3.0f, -1.5f),
            glm::vec2(-2.5f, 0.5f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(2.5f, 1.0f),
            glm::vec2(-1.5f, 2.5f),
            glm::vec2(1.0f, 3.0f)
        };
        DelaunayMesh defaultDelaunay(points2D);
        defaultDelaunay.mode = DelaunayMesh::GenerationMode::CUSTOM;
        defaultDelaunay.showControlPoints = true;

        std::vector<EntityID> pointEntities;
        for (const auto& pos2D : points2D) {
            Entity pointEntity = this->_entityManager.createEntity();
            glm::vec3 pos3D(pos2D.x, pos2D.y, 0.0f);
            this->_componentRegistry.registerComponent(pointEntity.getId(), Transform(pos3D));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Point(0.4f));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Renderable(ofMesh(), ofColor::yellow, true, nullptr, nullptr, true));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Selectable());
            this->_componentRegistry.registerComponent(pointEntity.getId(), BoundingBoxVisualization(BoundingBoxVisualization::Type::SPHERE, false));

            std::string pointName = "DelaunayCP " + std::to_string(pointEntity.getId());
            this->_sceneManager.registerEntity(pointEntity.getId(), pointName);
            pointEntities.push_back(pointEntity.getId());
        }

        defaultDelaunay.controlPointEntities = pointEntities;
        this->_componentRegistry.registerComponent(entity.getId(), defaultDelaunay);
        primitiveName = "Delaunay " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::BezierCurve) {
        std::vector<glm::vec3> controlPoints = {
            glm::vec3(-2.0f, -1.0f, 0.0f),
            glm::vec3(-1.0f, 2.0f, 0.0f),
            glm::vec3(1.0f, 2.0f, 0.0f),
            glm::vec3(2.0f, -1.0f, 0.0f)
        };
        ParametricCurve bezier(ParametricCurve::Type::BEZIER_CUBIC, controlPoints, 50);
        bezier.showControlPoints = true;

        std::vector<EntityID> pointEntities;
        for (const auto& pos : controlPoints) {
            Entity pointEntity = this->_entityManager.createEntity();
            this->_componentRegistry.registerComponent(pointEntity.getId(), Transform(pos));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Point(0.5f));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Renderable(ofMesh(), ofColor::cyan, true, nullptr, nullptr, true));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Selectable());
            this->_componentRegistry.registerComponent(pointEntity.getId(), BoundingBoxVisualization(BoundingBoxVisualization::Type::SPHERE, false));

            std::string pointName = "BezierCP " + std::to_string(pointEntity.getId());
            this->_sceneManager.registerEntity(pointEntity.getId(), pointName);
            pointEntities.push_back(pointEntity.getId());
        }

        bezier.controlPointEntities = pointEntities;
        this->_componentRegistry.registerComponent(entity.getId(), bezier);
        primitiveName = "Bezier " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    } else if (type == PrimitiveType::CatmullRomCurve) {
        std::vector<glm::vec3> controlPoints = {
            glm::vec3(-3.0f, 0.0f, 0.0f),
            glm::vec3(-1.5f, 2.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.5f, 2.0f, 0.0f),
            glm::vec3(3.0f, 0.0f, 0.0f)
        };
        ParametricCurve catmull(ParametricCurve::Type::CATMULL_ROM, controlPoints, 50);
        catmull.showControlPoints = true;

        std::vector<EntityID> pointEntities;
        for (const auto& pos : controlPoints) {
            Entity pointEntity = this->_entityManager.createEntity();
            this->_componentRegistry.registerComponent(pointEntity.getId(), Transform(pos));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Point(0.5f));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Renderable(ofMesh(), ofColor::magenta, true, nullptr, nullptr, true));
            this->_componentRegistry.registerComponent(pointEntity.getId(), Selectable());
            this->_componentRegistry.registerComponent(pointEntity.getId(), BoundingBoxVisualization(BoundingBoxVisualization::Type::SPHERE, false));

            std::string pointName = "CatmullCP " + std::to_string(pointEntity.getId());
            this->_sceneManager.registerEntity(pointEntity.getId(), pointName);
            pointEntities.push_back(pointEntity.getId());
        }

        catmull.controlPointEntities = pointEntities;
        this->_componentRegistry.registerComponent(entity.getId(), catmull);
        primitiveName = "CatmullRom " + std::to_string(entity.getId());
        bboxType = BoundingBoxVisualization::Type::AABB;
    }

    this->_componentRegistry.registerComponent(entity.getId(), Renderable(ofMesh(), defaultColor, true, nullptr, nullptr, true));
    this->_componentRegistry.registerComponent(entity.getId(), Selectable());

    this->_componentRegistry.registerComponent(entity.getId(), BoundingBoxVisualization(bboxType, false));

    this->_sceneManager.registerEntity(entity.getId(), primitiveName);

    this->_primitiveSystem.generateMeshes();

    if (type == PrimitiveType::BezierCurve || type == PrimitiveType::CatmullRomCurve) {
        ParametricCurve* curve = this->_componentRegistry.getComponent<ParametricCurve>(entity.getId());
        if (curve && !curve->controlPointEntities.empty()) {
            for (EntityID pointId : curve->controlPointEntities) {
                this->_sceneManager.setParent(pointId, entity.getId());
            }
        }
    } else if (type == PrimitiveType::DelaunayTriangulation) {
        DelaunayMesh* delaunay = this->_componentRegistry.getComponent<DelaunayMesh>(entity.getId());
        if (delaunay && !delaunay->controlPointEntities.empty()) {
            for (EntityID pointId : delaunay->controlPointEntities) {
                this->_sceneManager.setParent(pointId, entity.getId());
            }
        }
    }

    this->_primitiveSystem.generateMeshes();

    if (this->_eventLogPanel)
        this->_eventLogPanel->addLog("Created " + primitiveName + " (default parameters)", ofColor::cyan);
}
