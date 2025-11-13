#include "UI/DelaunayPanel.hpp"

DelaunayPanel::DelaunayPanel(
    ComponentRegistry& componentRegistry,
    SelectionSystem& selectionSystem,
    PrimitiveSystem& primitiveSystem
)
    : _componentRegistry(componentRegistry),
      _selectionSystem(selectionSystem),
      _primitiveSystem(primitiveSystem)
{
}

void DelaunayPanel::render()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (selectedEntities.empty()) {
        return;
    }

    EntityID primaryEntity = this->_selectionSystem.getSelectedEntity();
    DelaunayMesh* delaunayMesh = this->_componentRegistry.getComponent<DelaunayMesh>(primaryEntity);

    if (!delaunayMesh) {
        return;
    }

    ImGui::Separator();
    ImGui::Text("Delaunay Mesh");

    const char* generationModes[] = {"Random", "Grid", "Custom"};
    int currentMode = static_cast<int>(delaunayMesh->mode);

    if (ImGui::Combo("Generation Mode", &currentMode, generationModes, 3)) {
        delaunayMesh->mode = static_cast<DelaunayMesh::GenerationMode>(currentMode);
        this->_primitiveSystem.generateMeshes();
    }

    const char* displayModes[] = {"Delaunay Only", "Voronoi Only", "Both"};
    int currentDisplayMode = static_cast<int>(delaunayMesh->displayMode);

    if (ImGui::Combo("Display Mode", &currentDisplayMode, displayModes, 3)) {
        delaunayMesh->displayMode = static_cast<DelaunayMesh::DisplayMode>(currentDisplayMode);
        this->_primitiveSystem.generateMeshes();
    }

    if (delaunayMesh->mode == DelaunayMesh::GenerationMode::RANDOM) {
        int numPoints = delaunayMesh->numRandomPoints;
        if (ImGui::SliderInt("Point Count", &numPoints, 3, 100)) {
            delaunayMesh->numRandomPoints = numPoints;
            this->_primitiveSystem.generateMeshes();
        }

        glm::vec2 bounds = delaunayMesh->bounds;
        if (ImGui::DragFloat2("Bounds", &bounds.x, 0.1f, 1.0f, 100.0f)) {
            delaunayMesh->bounds = bounds;
            this->_primitiveSystem.generateMeshes();
        }
    } else if (delaunayMesh->mode == DelaunayMesh::GenerationMode::GRID) {
        int gridRes = delaunayMesh->gridResolution;
        if (ImGui::SliderInt("Grid Resolution", &gridRes, 2, 20)) {
            delaunayMesh->gridResolution = gridRes;
            this->_primitiveSystem.generateMeshes();
        }

        float perturbation = delaunayMesh->gridPerturbation;
        if (ImGui::SliderFloat("Perturbation", &perturbation, 0.0f, 1.0f)) {
            delaunayMesh->gridPerturbation = perturbation;
            this->_primitiveSystem.generateMeshes();
        }

        glm::vec2 bounds = delaunayMesh->bounds;
        if (ImGui::DragFloat2("Bounds", &bounds.x, 0.1f, 1.0f, 100.0f)) {
            delaunayMesh->bounds = bounds;
            this->_primitiveSystem.generateMeshes();
        }
    } else {
        ImGui::TextDisabled("Point Count: %zu", delaunayMesh->points.size());
        ImGui::TextWrapped("Custom mode: points are defined manually");
    }
}
