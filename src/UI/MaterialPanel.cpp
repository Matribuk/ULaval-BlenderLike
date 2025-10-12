#include "UI/MaterialPanel.hpp"


MaterialPanel::MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem, ResourceManager& resourceManager)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem), _resourceManager(resourceManager){}

bool MaterialPanel::_checkAllEntitiesHaveSameVisibility(const std::set<EntityID>& entities, bool& outVisibility) const
{
    if (entities.empty()) return false;

    bool first = true;
    bool referenceVisibility;

    for (EntityID id : entities) {
        Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
        if (!renderable) return false;

        if (first) {
            referenceVisibility = renderable->visible;
            first = false;
        } else {
            if (renderable->visible != referenceVisibility)
                return false;
        }
    }

    outVisibility = referenceVisibility;
    return true;
}

void MaterialPanel::render()
{
    const std::set<EntityID>& selectedEntities = this->_selectionSystem.getSelectedEntities();

    if (selectedEntities.empty()) {
        this->_prevSelectedEntities.clear();
        return;
    }

    bool allHaveRenderable = true;
    int tmpCount = 0;
    for (EntityID id : selectedEntities) {
        tmpCount++;
        if (!this->_componentRegistry.getComponent<Renderable>(id))
            allHaveRenderable = false;
    }

    if (!allHaveRenderable && tmpCount > 1) {
        ImGui::Text("Some selected entities don't have Renderable component");
        this->_prevSelectedEntities.clear();
        return;
    } else if (!allHaveRenderable && tmpCount == 1) {
        ImGui::Button("Add Renderable Component");
        if (ImGui::IsItemClicked()) {
            EntityID onlyEntity = *selectedEntities.begin();
            this->_addMaterialComponent(onlyEntity);
        }
    }

    EntityID primaryEntity = this->_selectionSystem.getSelectedEntity();
    Renderable* primaryRenderable = this->_componentRegistry.getComponent<Renderable>(primaryEntity);

    if (!primaryRenderable) {
        this->_prevSelectedEntities.clear();
        return;
    }

    if (this->_prevSelectedEntities != selectedEntities)
        this->_prevSelectedEntities = selectedEntities;

    bool commonVisibility;
    bool visibilitySame = this->_checkAllEntitiesHaveSameVisibility(selectedEntities, commonVisibility);

    bool editVisibility = visibilitySame ? commonVisibility : primaryRenderable->visible;
    bool visibilityChanged = false;

    ImGui::BeginDisabled(!visibilitySame);
    visibilityChanged = ImGui::Checkbox("Visible", &editVisibility);
    ImGui::EndDisabled();
    if (!visibilitySame) visibilityChanged = false;
    if (visibilityChanged) {
        for (EntityID id : selectedEntities) {
            Renderable* renderable = this->_componentRegistry.getComponent<Renderable>(id);
            if (renderable) {
                renderable->visible = editVisibility;
            }
        }
    }

    if (selectedEntities.size() > 1) {
        ImGui::Text("(%zu entities selected)", selectedEntities.size());
    }

    if (primaryRenderable->material) {
        ImGui::Text("Material:");

        if (primaryRenderable->material->shader) {
            ofShader* shader = primaryRenderable->material->shader;
            std::string shaderName = this->_resourceManager.getShaderPath(*shader);
            ImGui::Text(" - Shader: Set");

            this->_loadShaders(primaryRenderable);

            ImGui::SameLine();
            if (ImGui::Button("Clear Shader")) {
                primaryRenderable->material->shader = nullptr;
            }
        }
        else {
            ImGui::Text(" - Shader: None");
            ImGui::SameLine();
            this->_loadShaders(primaryRenderable);
        }

        if (primaryRenderable->material->texture) {
            ofTexture* tex = primaryRenderable->material->texture;
            std::string texName = this->_resourceManager.getTexturePath(*tex);
            ImGui::Text(" - Texture: Set %s", texName.c_str());
            ImVec2 thumbSize = ImVec2(24, 24);
            GLuint texID = tex->getTextureData().textureID;
            ImGui::Image((ImTextureID)(uintptr_t)texID, thumbSize, ImVec2(0,1), ImVec2(1,0));

            ImGui::SameLine();
            this->_loadTextures(primaryRenderable);

            ImGui::SameLine();
            if (ImGui::Button("Clear Texture")) {
                primaryRenderable->material->texture = nullptr;
            }
        } else {
            ImGui::Text(" - Texture: None");
            ImGui::SameLine();
            this->_loadTextures(primaryRenderable);

        }

        if (primaryRenderable->mesh.getNumVertices() > 0) {
            ofMesh mesh = primaryRenderable->mesh;
            std::string meshName = this->_resourceManager.getMeshPath(mesh);
            ImGui::Text(" - Mesh: %s", meshName.c_str());
            ImGui::SameLine();
            this->_loadMeshes(primaryRenderable);
        } else {
            ImGui::Text(" - Mesh: None");
            ImGui::SameLine();
            this->_loadMeshes(primaryRenderable);
        }

        ImGui::Separator();

    }
}

void MaterialPanel::_addMaterialComponent(EntityID entityId)
{
    if (entityId == INVALID_ENTITY) return;

    this->_componentRegistry.registerComponent<Renderable>(entityId, Renderable(ofMesh(), ofColor::white));
}

void MaterialPanel::_loadShaders(Renderable* primaryRenderable) {
    if (ImGui::Button("Load Shaders")) {
                ImGui::OpenPopup("LoadShadersPopup");
            }

            if (ImGui::BeginPopup("LoadShadersPopup")) {
                std::filesystem::path shaderDir = std::filesystem::path("data") / "shaders";
                std::vector<std::string> names;
                if (std::filesystem::exists(shaderDir) && std::filesystem::is_directory(shaderDir)) {
                    for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(shaderDir)) {
                        if (!entry.is_regular_file()) continue;
                        std::string ext = entry.path().extension().string();
                        if (ext == ".vert" || ext == ".frag") {
                            names.push_back(entry.path().stem().string());
                        }
                    }
                }
                std::sort(names.begin(), names.end());
                names.erase(std::unique(names.begin(), names.end()), names.end());

                if (names.empty()) {
                    ImGui::TextDisabled("No shaders found in data/shaders");
                } else {
                    for (const std::string &n : names) {
                        if (ImGui::Selectable(n.c_str())) {
                            std::filesystem::path vert = shaderDir / (n + ".vert");
                            std::filesystem::path frag = shaderDir / (n + ".frag");
                            if (std::filesystem::exists(vert) && std::filesystem::exists(frag)) {
                                ofShader& loaded = this->_resourceManager.loadShader(vert.string(), frag.string());
                                primaryRenderable->material->shader = &loaded;
                            }
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }

                ImGui::EndPopup();
            }
}

void MaterialPanel::_loadTextures(Renderable* primaryRenderable) {
    if (ImGui::Button("Load Texture")) {
        ofFileDialogResult result = ofSystemLoadDialog("Choose texture to load", false);
        if (result.bSuccess) {
            std::string path = result.getPath();
            ofTexture& newTex = this->_resourceManager.loadTexture(path);
            primaryRenderable->material->texture = &newTex;
        }
    }
}

void MaterialPanel::_loadMeshes(Renderable* primaryRenderable) {
    if (ImGui::Button("Load Mesh")) {
        ofFileDialogResult result = ofSystemLoadDialog("Choose mesh to load", false);
        if (result.bSuccess) {
            std::string path = result.getPath();
            ofMesh& newMesh = this->_resourceManager.loadMesh(path);
            primaryRenderable->mesh = newMesh;
        }
    }
}
