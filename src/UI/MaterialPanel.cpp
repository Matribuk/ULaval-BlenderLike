#include "UI/MaterialPanel.hpp"

MaterialPanel::MaterialPanel(ComponentRegistry& componentRegistry, SelectionSystem& selectionSystem, ResourceManager& resourceManager)
    : _componentRegistry(componentRegistry), _selectionSystem(selectionSystem), _resourceManager(resourceManager){}

bool MaterialPanel::_checkAllEntitiesHaveSameVisibility(const std::set<EntityID>& entities, bool& outVisibility) const
{
    if (entities.empty()) return false;

    bool first = true;
    bool referenceVisibility = false;

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
    for (EntityID id : selectedEntities) {
        if (!this->_componentRegistry.getComponent<Renderable>(id))
            allHaveRenderable = false;
    }

    if (!allHaveRenderable) {
        if (selectedEntities.size() > 1) {
            ImGui::Text("Some selected entities don't have Renderable component");
        } else {
            ImGui::Button("Add Renderable Component");
            if (ImGui::IsItemClicked()) this->_addMaterialComponent(*selectedEntities.begin());
        }
        this->_prevSelectedEntities.clear();
        return;
    }

    EntityID primaryEntity = this->_selectionSystem.getSelectedEntity();
    Renderable* primaryRenderable = this->_componentRegistry.getComponent<Renderable>(primaryEntity);

    if (!primaryRenderable) {
        this->_prevSelectedEntities.clear();
        return;
    }

    if (this->_prevSelectedEntities != selectedEntities)
        this->_prevSelectedEntities = selectedEntities;

    this->_renderVisibilityControl(selectedEntities, primaryRenderable);

    if (selectedEntities.size() > 1)
        ImGui::Text("(%zu entities selected)", selectedEntities.size());

    if (primaryRenderable->material) {
        ImGui::Text("Material:");
        this->_renderShaderSection(primaryEntity, primaryRenderable);
        this->_renderTextureSection(primaryEntity, primaryRenderable);
        this->_renderMeshSection(primaryEntity, primaryRenderable);

        ImGui::Separator();
        this->_renderLightingParameters(selectedEntities, primaryRenderable);
    }
}

void MaterialPanel::_addMaterialComponent(EntityID entityId)
{
    if (entityId == INVALID_ENTITY) return;

    this->_componentRegistry.registerComponent<Renderable>(entityId, Renderable(ofMesh(), ofColor::white));
}

void MaterialPanel::_loadShaders(Renderable* primaryRenderable)
{
    if (ImGui::Button("Load Shaders"))
        ImGui::OpenPopup("LoadShadersPopup");

    if (ImGui::BeginPopup("LoadShadersPopup")) {
        std::filesystem::path shaderDir = std::filesystem::path(ofToDataPath("shaders"));
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
                if (n == "skycube") continue;

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

void MaterialPanel::_loadFile(EntityID entityId, Renderable* primaryRenderable, std::string type)
{
    std::string title = type.compare("MESH") == 0 ? "Load mesh" : "Load texture";
    if (ImGui::Button(title.c_str())) {
        ofFileDialogResult result = ofSystemLoadDialog("Choose a file to load", false);
        if (result.bSuccess) {
            std::string path = result.getPath();
            if (type.compare("MESH") == 0 ) {
                ofMesh& newMesh = this->_resourceManager.loadMesh(path);
                primaryRenderable->mesh = newMesh;

                primaryRenderable->isPrimitive = false;

                glm::vec3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
                glm::vec3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

                for (const auto& vertex : newMesh.getVertices()) {
                    minBounds.x = std::min(minBounds.x, vertex.x);
                    minBounds.y = std::min(minBounds.y, vertex.y);
                    minBounds.z = std::min(minBounds.z, vertex.z);
                    maxBounds.x = std::max(maxBounds.x, vertex.x);
                    maxBounds.y = std::max(maxBounds.y, vertex.y);
                    maxBounds.z = std::max(maxBounds.z, vertex.z);
                }

                glm::vec3 size = maxBounds - minBounds;

                this->_componentRegistry.removeComponent<Box>(entityId);
                this->_componentRegistry.removeComponent<Sphere>(entityId);
                this->_componentRegistry.removeComponent<Plane>(entityId);
                this->_componentRegistry.removeComponent<Triangle>(entityId);
                this->_componentRegistry.removeComponent<Circle>(entityId);
                this->_componentRegistry.removeComponent<Line>(entityId);
                this->_componentRegistry.removeComponent<Rectangle>(entityId);
                this->_componentRegistry.removeComponent<Point>(entityId);

                this->_componentRegistry.registerComponent(entityId, Box(size));
            } else {
                ofTexture& newTex = this->_resourceManager.loadTexture(path);
                primaryRenderable->material->texture = &newTex;
            }
        }
    }
}

void MaterialPanel::_generateProceduralTexture(Renderable* primaryRenderable)
{
    if (ImGui::Button("Generate Procedural"))
        ImGui::OpenPopup("ProceduralTexturePopup");

    if (ImGui::BeginPopup("ProceduralTexturePopup")) {
        ImGui::Text("Generate Procedural Texture");
        ImGui::Separator();

        static int selectedType = 0;
        const char* types[] = { "Perlin Noise", "Voronoi", "Checkerboard", "Gradient" };
        ImGui::Combo("Type", &selectedType, types, IM_ARRAYSIZE(types));

        static float color1[3] = {0.2f, 0.2f, 0.8f};
        static float color2[3] = {0.9f, 0.9f, 0.9f};
        ImGui::ColorEdit3("Color 1", color1);
        ImGui::ColorEdit3("Color 2", color2);

        static int resolution = 512;
        ImGui::SliderInt("Resolution", &resolution, 128, 2048);

        if (ImGui::Button("Generate", ImVec2(-1, 0))) {
            ProceduralTexture::Type type;
            switch (selectedType) {
                case 0: type = ProceduralTexture::Type::PERLIN_NOISE; break;
                case 1: type = ProceduralTexture::Type::VORONOI; break;
                case 2: type = ProceduralTexture::Type::CHECKERBOARD; break;
                case 3: type = ProceduralTexture::Type::GRADIENT; break;
                default: type = ProceduralTexture::Type::PERLIN_NOISE; break;
            }

            ofColor col1(color1[0] * 255, color1[1] * 255, color1[2] * 255);
            ofColor col2(color2[0] * 255, color2[1] * 255, color2[2] * 255);

            ofTexture generatedTex = this->_proceduralTextureGenerator.generate(type, resolution, resolution, col1, col2);

            std::string texName = "procedural_" + std::string(types[selectedType]) + "_" + std::to_string(resolution);
            ofTexture& storedTex = this->_resourceManager.storeTexture(texName, generatedTex);
            primaryRenderable->material->texture = &storedTex;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void MaterialPanel::_renderVisibilityControl(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable)
{
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
            if (renderable) renderable->visible = editVisibility;
        }
    }
}

void MaterialPanel::_renderShaderSection(EntityID primaryEntity, Renderable* primaryRenderable)
{
    if (primaryRenderable->material->shader) {
        ImGui::Text(" - Shader: Set");
        this->_loadShaders(primaryRenderable);
        ImGui::SameLine();
        if (ImGui::Button("Clear Shader"))
            primaryRenderable->material->shader = nullptr;
    } else {
        ImGui::Text(" - Shader: None");
        ImGui::SameLine();
        this->_loadShaders(primaryRenderable);
    }
}

void MaterialPanel::_renderTextureSection(EntityID primaryEntity, Renderable* primaryRenderable)
{
    if (primaryRenderable->material->texture) {
        ofTexture* tex = primaryRenderable->material->texture;
        std::string texName = this->_resourceManager.getTexturePath(*tex);
        ImGui::Text(" - Texture: Set %s", texName.c_str());

        ImVec2 thumbSize = ImVec2(24, 24);
        GLuint texID = tex->getTextureData().textureID;
        ImGui::Image((ImTextureID)(uintptr_t)texID, thumbSize, ImVec2(0,1), ImVec2(1,0));

        ImGui::SameLine();
        this->_loadFile(primaryEntity, primaryRenderable, "TEX");

        ImGui::SameLine();
        if (ImGui::Button("Clear Texture"))
            primaryRenderable->material->texture = nullptr;
    } else {
        ImGui::Text(" - Texture: None");
        this->_loadFile(primaryEntity, primaryRenderable, "TEX");
        ImGui::SameLine();
        this->_generateProceduralTexture(primaryRenderable);
    }
}

void MaterialPanel::_renderMeshSection(EntityID primaryEntity, Renderable* primaryRenderable)
{
    if (primaryRenderable->mesh.getNumVertices() > 0) {
        ofMesh mesh = primaryRenderable->mesh;
        std::string meshName = this->_resourceManager.getMeshPath(mesh);
        ImGui::Text(" - Mesh: %s", meshName.c_str());
        ImGui::SameLine();
        this->_loadFile(primaryEntity, primaryRenderable, "MESH");
    } else {
        ImGui::Text(" - Mesh: None");
        ImGui::SameLine();
        this->_loadFile(primaryEntity, primaryRenderable, "MESH");
    }
}

void MaterialPanel::_renderLightingParameters(const std::set<EntityID>& selectedEntities, Renderable* primaryRenderable)
{
    if (!primaryRenderable->material->shader) return;

    ImGui::Text("Lighting Parameters:");

    if (ImGui::DragFloat3("Light Position", &primaryRenderable->material->lightPosition.x, 0.1f))
        this->_syncMaterialProperty(selectedEntities, &Material::lightPosition, primaryRenderable->material->lightPosition);

    if (ImGui::ColorEdit3("Light Color", &primaryRenderable->material->lightColor.x))
        this->_syncMaterialProperty(selectedEntities, &Material::lightColor, primaryRenderable->material->lightColor);

    if (ImGui::SliderFloat("Light Intensity", &primaryRenderable->material->lightIntensity, 0.0f, 5.0f))
        this->_syncMaterialProperty(selectedEntities, &Material::lightIntensity, primaryRenderable->material->lightIntensity);

    if (ImGui::ColorEdit3("Ambient Color", &primaryRenderable->material->ambientColor.x))
        this->_syncMaterialProperty(selectedEntities, &Material::ambientColor, primaryRenderable->material->ambientColor);

    if (ImGui::SliderFloat("Shininess", &primaryRenderable->material->shininess, 1.0f, 128.0f))
        this->_syncMaterialProperty(selectedEntities, &Material::shininess, primaryRenderable->material->shininess);

    ImGui::Separator();
}
